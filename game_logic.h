#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <stdbool.h>
#include "structs.h"

bool isMoveValid(const int grid[GRID_SIZE][GRID_SIZE], int row, int col, int value);

int computePossibilities(const int grid[GRID_SIZE][GRID_SIZE], int row, int col, int poss[10]);

bool isBoardComplete(const int grid[GRID_SIZE][GRID_SIZE]);
bool isBoardValid(const int grid[GRID_SIZE][GRID_SIZE]);

#endif 


