#ifndef FILE_IO_H
#define FILE_IO_H

#include <stdbool.h>
#include "structs.h"

bool saveGameState(const GameState *state, const char *path);
bool loadGameState(GameState *state, const char *path);

bool appendHighScore(const char *difficulty, const char *puzzleName, unsigned int seconds, const char *path);
void printHighScores(const char *path);
void printTopHighScores(const char *path, int topN);

bool loadPuzzleByDifficulty(Puzzle *out, const char *path, const char *difficulty);

bool saveGameStateAuto(const GameState *state, char *outPath, size_t outPathSize);

int listSaveFiles(char names[][260], int maxCount);

bool deleteSaveFile(const char *baseName);

#endif 



