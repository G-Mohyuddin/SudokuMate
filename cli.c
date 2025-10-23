#include <stdio.h>
#include <string.h>
#include "cli.h"

static void printHorizontalSeparator(void) {
	printf("  +-------+-------+-------+\n");
}

void printBoard(const int grid[GRID_SIZE][GRID_SIZE]) {
	// Print column numbers (1-9)
	printf("\n    ");
	for (int c = 0; c < GRID_SIZE; c++) {
		printf("%d", c + 1);
		if ((c + 1) % 3 == 0) {
			printf("   ");
		} else {
			printf(" ");
		}
	}
	printf("\n");
	
	printHorizontalSeparator();
	for (int r = 0; r < GRID_SIZE; r++) {
		printf("%d | ", r + 1);  // Row number (1-9)
		for (int c = 0; c < GRID_SIZE; c++) {
			int value = grid[r][c];
			char ch = value ? ('0' + value) : '.';
			printf("%c", ch);
			if ((c + 1) % 3 == 0) {
				printf(" | ");
			} else {
				printf(" ");
			}
		}
		printf("\n");
		if ((r + 1) % 3 == 0) {
			printHorizontalSeparator();
		}
	}
}

// Prompt the player for an action while inside a Sudoku game.
// Returns:
//   0 = quit game
//   1 = place value at row/col
//   2 = auto-solve
//   3 = save game
//   4 = check possibilities at row/col
//   5 = undo last move
//   6 = redo last undone move
int promptMove(int *row, int *col, int *value) {
	printf("\nEnter 'row col value' to place a number.\n");
	printf("Or type: 'check r c' | 'undo' | 'redo' | 'solve' | 'save' | 'quit'\n> ");
	char buffer[64];
	if (!fgets(buffer, sizeof(buffer), stdin)) {
		return 0;
	}
	// Allow typing 'quit' (case-insensitive) to exit the current game
	if ((buffer[0] == 'q' || buffer[0] == 'Q') &&
		(buffer[1] == 'u' || buffer[1] == 'U') &&
		(buffer[2] == 'i' || buffer[2] == 'I') &&
		(buffer[3] == 't' || buffer[3] == 'T') &&
		(buffer[4] == '\0' || buffer[4] == ' ' || buffer[4] == '\n')) {
		return 0;
	}
	if (buffer[0] == 's' || buffer[0] == 'S') {
		if (buffer[1] == 'a' || buffer[1] == 'A') {
			return 3; // 'save'
		}
		return 2; // 'solve'
	}
	if (buffer[0] == 'c' || buffer[0] == 'C') {
		int r, c;
		if (sscanf(buffer, "%*s %d %d", &r, &c) == 2) {
			*row = r - 1;
			*col = c - 1;
			*value = 0;
			return 4; // check
		}
		printf("Usage: check r c\n");
		return 1;
	}
	if (buffer[0] == 'u' || buffer[0] == 'U') return 5; // undo
	if (buffer[0] == 'r' || buffer[0] == 'R') return 6; // redo
	int r, c, v;
	if (sscanf(buffer, "%d %d %d", &r, &c, &v) == 3) {
		*row = r - 1;
		*col = c - 1;
		*value = v;
		return 1;
	}
	printf("Invalid input. Please try again.\n");
	return 1; // keep going, ask again by caller
}

void printMessage(const char *msg) {
	printf("\n%s\n", msg);
}

int showStartMenu(void) {
	printf("\n");
	printf("+=====================================+\n");
	printf("|           SudokuMate Menu           |\n");
	printf("+=====================================+\n");
	printf("|  1) Play Sudoku                     |\n");
	printf("|  2) Enter custom puzzle             |\n");
	printf("|  3) Load saved game                 |\n");
	printf("|  4) View high scores (top 10)       |\n");
	printf("|  q) Quit                            |\n");
	printf("+-------------------------------------+\n");
	printf("Select an option: ");
	char buf[32];
	if (!fgets(buf, sizeof(buf), stdin)) return 0;
	if (buf[0] == '1') return 1;
	if (buf[0] == '2') return 2;
	if (buf[0] == '3') return 3;
	if (buf[0] == '4') return 4;
	return 0;
}

int showDifficultyMenu(void) {
	printf("\n");
	printf("+-----------------------+\n");
	printf("|   Select Difficulty   |\n");
	printf("+-----------------------+\n");
	printf("| 1) Easy               |\n");
	printf("| 2) Medium             |\n");
	printf("| 3) Hard               |\n");
	printf("| 4) Expert             |\n");
	printf("| 0) Back               |\n");
	printf("+-----------------------+\n");
	printf("Select an option: ");
	char buf[32];
	if (!fgets(buf, sizeof(buf), stdin)) return 0;
	if (buf[0] == '1') return 1;
	if (buf[0] == '2') return 2;
	if (buf[0] == '3') return 3;
	if (buf[0] == '4') return 4;
	return 0;
}

int promptCustomPuzzle(int grid[GRID_SIZE][GRID_SIZE]) {
	printMessage("Enter 9 lines of 9 digits (0 for empty). Example: 530070000");
	char line[64];
	for (int r = 0; r < GRID_SIZE; r++) {
		printf("Row %d: ", r + 1);
		if (!fgets(line, sizeof(line), stdin)) return 0;
		int count = 0;
		for (int i = 0; line[i] != '\0' && count < GRID_SIZE; i++) {
			if (line[i] >= '0' && line[i] <= '9') {
				grid[r][count++] = line[i] - '0';
			}
		}
		if (count != GRID_SIZE) {
			printMessage("Invalid row. Please enter exactly 9 digits.");
			r--; // retry same row
		}
	}
	return 1;
}

int promptFilePath(char *outPath, size_t outPathSize, const char *purpose) {
	printf("%s file path (or blank to cancel): ", purpose);
	char line[260];
	if (!fgets(line, sizeof(line), stdin)) return 0;
	// trim newline
	char *nl = strchr(line, '\n');
	if (nl) *nl = '\0';
	if (line[0] == '\0') return 0;
	strncpy(outPath, line, outPathSize - 1);
	outPath[outPathSize - 1] = '\0';
	return 1;
}

