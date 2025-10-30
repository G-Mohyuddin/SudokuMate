#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#endif
#include "file_io.h"

static void copyString(char *dst, const char *src, size_t dstSize) {
	if (!dst || !src || dstSize == 0) return;
	size_t i = 0;
	while (i + 1 < dstSize && src[i] != '\0') {
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
}

bool saveGameState(const GameState *state, const char *path) {
	FILE *f = fopen(path, "wb");
	if (!f) return false;
	size_t wrote = fwrite(state, 1, sizeof(*state), f);
	fclose(f);
	return wrote == sizeof(*state);
}

bool loadGameState(GameState *state, const char *path) {
	FILE *f = fopen(path, "rb");
	if (!f) return false;
	size_t read = fread(state, 1, sizeof(*state), f);
	fclose(f);
	return read == sizeof(*state);
}

bool appendHighScore(const char *difficulty, const char *puzzleName, unsigned int seconds, const char *path) {
	FILE *f = fopen(path, "a");
	if (!f) return false;
	int ok = fprintf(f, "%s\t%s\t%u\n", difficulty, puzzleName, seconds);
	fclose(f);
	return ok > 0;
}

void printHighScores(const char *path) {
	FILE *f = fopen(path, "r");
	if (!f) {
		printf("No high scores yet.\n");
		return;
	}
	char line[256];
	printf("High Scores (difficulty\tpuzzle\tseconds)\n");
	while (fgets(line, sizeof(line), f)) {
		printf("%s", line);
	}
	fclose(f);
}

typedef struct ParsedScore { char diff[32]; char name[64]; unsigned int sec; } ParsedScore;

static int cmpScore(const void *a, const void *b) {
	const ParsedScore *sa = (const ParsedScore*)a;
	const ParsedScore *sb = (const ParsedScore*)b;
	if (sa->sec < sb->sec) return -1;
	if (sa->sec > sb->sec) return 1;
	return strcmp(sa->name, sb->name);
}

void printTopHighScores(const char *path, int topN) {
	FILE *f = fopen(path, "r");
	if (!f) { printf("No high scores yet.\n"); return; }
	ParsedScore arr[256]; int n = 0;
	char line[256];
	while (fgets(line, sizeof(line), f) && n < 256) {
		char diff[32], name[64]; unsigned int sec;
		if (sscanf(line, "%31[^\t]\t%63[^\t]\t%u", diff, name, &sec) == 3) {
            copyString(arr[n].diff, diff, sizeof(arr[n].diff));
            copyString(arr[n].name, name, sizeof(arr[n].name));
			arr[n].sec = sec; n++;
		}
	}
	fclose(f);
	qsort(arr, n, sizeof(arr[0]), cmpScore);
	if (n == 0) { printf("No high scores yet.\n"); return; }
	if (topN <= 0) topN = 10;
	printf("Top %d High Scores (best times)\n", topN);
	for (int i = 0; i < n && i < topN; i++) {
		unsigned int m = (arr[i].sec / 60) % 60, s = arr[i].sec % 60;
		printf("%2d) %s | %s | %02u:%02u\n", i + 1, arr[i].diff, arr[i].name, m, s);
	}
}

static int parseGrid81(const char *digits, int grid[GRID_SIZE][GRID_SIZE]) {
	int len = (int)strlen(digits);
	if (len < 81) return 0;
	int k = 0;
	for (int r = 0; r < GRID_SIZE; r++) {
		for (int c = 0; c < GRID_SIZE; c++) {
			char ch = digits[k++];
			if (ch < '0' || ch > '9') return 0;
			grid[r][c] = ch - '0';
		}
	}
	return 1;
}

bool loadPuzzleByDifficulty(Puzzle *out, const char *path, const char *difficulty) {
	FILE *f = fopen(path, "r");
	if (!f) return false;
	char line[256];
	while (fgets(line, sizeof(line), f)) {
		// name|difficulty|digits
		char *p1 = strchr(line, '|');
		if (!p1) continue;
		*p1 = '\0';
		char *p2 = strchr(p1 + 1, '|');
		if (!p2) continue;
		*p2 = '\0';
		char *name = line;
		char *diff = p1 + 1;
		char *digits = p2 + 1;
		char *nl = strchr(digits, '\n'); if (nl) *nl = '\0';
		if (strcmp(diff, difficulty) != 0) continue;
		memset(out, 0, sizeof(*out));
        copyString(out->name, name, sizeof(out->name));
        copyString(out->difficulty, diff, sizeof(out->difficulty));
		if (!parseGrid81(digits, out->grid)) continue;
		fclose(f);
		return true;
	}
	fclose(f);
	return false;
}

static void sanitizeFilename(char *s) {
	for (char *p = s; *p; ++p) {
		if (*p == ' ' || *p == '/' || *p == '\\' || *p == '|' || *p == ':' || *p == '*' || *p == '?' || *p == '"' || *p == '<' || *p == '>' ) {
			*p = '_';
		}
	}
}

bool saveGameStateAuto(const GameState *state, char *outPath, size_t outPathSize) {
	#ifdef _WIN32
		_mkdir("saves");
	#else
		mkdir("saves", 0777);
	#endif
	char filename[256];
    copyString(filename, state->activePuzzleName[0] ? state->activePuzzleName : "unnamed", sizeof(filename));
	sanitizeFilename(filename);
	char path[320];
	snprintf(path, sizeof(path), "saves/%s.sav", filename);
	bool ok = saveGameState(state, path);
	if (outPath && outPathSize) {
        copyString(outPath, path, outPathSize);
	}
	return ok;
}

int listSaveFiles(char names[][260], int maxCount) {
	int count = 0;
#ifdef _WIN32
	WIN32_FIND_DATAA ffd;
	HANDLE hFind = FindFirstFileA("saves\\*.sav", &ffd);
	if (hFind == INVALID_HANDLE_VALUE) return 0;
	do {
		if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            copyString(names[count], ffd.cFileName, 260);
			count++;
			if (count >= maxCount) break;
		}
	} while (FindNextFileA(hFind, &ffd));
	FindClose(hFind);
#else
	DIR *dir = opendir("saves");
	if (!dir) return 0;
	struct dirent *ent;
	while ((ent = readdir(dir)) != NULL && count < maxCount) {
		if (ent->d_type == DT_REG) {
			const char *n = ent->d_name;
			size_t len = strlen(n);
			if (len > 4 && strcmp(n + len - 4, ".sav") == 0) {
                copyString(names[count], n, 260);
				count++;
			}
		}
	}
	closedir(dir);
#endif
	return count;
}

bool deleteSaveFile(const char *baseName) {
	char path[320];
	snprintf(path, sizeof(path), "saves/%s", baseName);
	int rc = remove(path);
	return rc == 0;
}


