#ifndef CLI_H
#define CLI_H

#include "structs.h"

void printBoard(const int grid[GRID_SIZE][GRID_SIZE]);

int promptMove(int *row, int *col, int *value);

void printMessage(const char *msg);

int showStartMenu(void);

int showDifficultyMenu(void);

int promptCustomPuzzle(int grid[GRID_SIZE][GRID_SIZE]);

int promptFilePath(char *outPath, size_t outPathSize, const char *purpose);

int promptDifficulty(char *out, size_t outSize);

#endif 



