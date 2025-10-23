#include <stdbool.h>
#include "solver.h"
#include "game_logic.h"

static bool findEmptyCell(int grid[GRID_SIZE][GRID_SIZE], int *outRow, int *outCol) {
	for (int r = 0; r < GRID_SIZE; r++) {
		for (int c = 0; c < GRID_SIZE; c++) {
			if (grid[r][c] == 0) {
				*outRow = r;
				*outCol = c;
				return true;
			}
		}
	}
	return false;
}

bool solveSudoku(int grid[GRID_SIZE][GRID_SIZE]) {
	int row, col;
	if (!findEmptyCell(grid, &row, &col)) {
		return true; // solved
	}
	for (int value = 1; value <= 9; value++) {
		if (isMoveValid((const int (*)[GRID_SIZE])grid, row, col, value)) {
			grid[row][col] = value;
			if (solveSudoku(grid)) return true;
			grid[row][col] = 0;
		}
	}
	return false; // backtrack
}


