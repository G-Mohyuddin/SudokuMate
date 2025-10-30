#ifndef FILE_IO_H
#define FILE_IO_H

#include <stdbool.h>
#include "structs.h"

bool saveGameState(const GameState *state, const char *path);
bool loadGameState(GameState *state, const char *path);

bool appendHighScore(const char *difficulty, const char *puzzleName, unsigned int seconds, const char *path);
void printHighScores(const char *path);
void printTopHighScores(const char *path, int topN);

// Load first puzzle matching difficulty from a text file.
// Format per line: name|difficulty|81digits (0 for empty)
bool loadPuzzleByDifficulty(Puzzle *out, const char *path, const char *difficulty);

// Build a default save path (saves/<sanitized-name>.sav) and save without prompting.
// If outPath is non-NULL, the final path will be written there (null-terminated).
bool saveGameStateAuto(const GameState *state, char *outPath, size_t outPathSize);

// Enumerate saves in the saves/ directory. Returns count, fills names with base filenames.
int listSaveFiles(char names[][260], int maxCount);

// Delete a save by base filename (in saves/). Returns true on success.
bool deleteSaveFile(const char *baseName);

#endif // FILE_IO_H


