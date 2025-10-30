#ifndef CLI_H
#define CLI_H

#include "structs.h"

void printBoard(const int grid[GRID_SIZE][GRID_SIZE]);

// Returns: 1=move; 0=quit; 2=solve; 3=save; 4=check possibilities; 5=undo; 6=redo
int promptMove(int *row, int *col, int *value);

void printMessage(const char *msg);

// Menu: returns 1=play, 2=custom, 3=saved games, 4=high scores, 0=quit
int showStartMenu(void);

// Difficulty submenu: returns 1=Easy, 2=Medium, 3=Hard, 4=Expert, 0=back
int showDifficultyMenu(void);

// Prompt user to fill a custom puzzle grid. Returns 1 on success, 0 on cancel.
int promptCustomPuzzle(int grid[GRID_SIZE][GRID_SIZE]);

// Ask for a file path; returns 1 if path captured, 0 on cancel
int promptFilePath(char *outPath, size_t outPathSize, const char *purpose);

// Prompt for a difficulty string; returns 1 on success
int promptDifficulty(char *out, size_t outSize);

#endif // CLI_H


