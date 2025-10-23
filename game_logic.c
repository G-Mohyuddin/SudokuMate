#include "game_logic.h"

static bool isInRow(const int grid[GRID_SIZE][GRID_SIZE], int row, int value) {
	for (int c = 0; c < GRID_SIZE; c++) {
		if (grid[row][c] == value) return true;
	}
	return false;
}

static bool isInCol(const int grid[GRID_SIZE][GRID_SIZE], int col, int value) {
	for (int r = 0; r < GRID_SIZE; r++) {
		if (grid[r][col] == value) return true;
	}
	return false;
}

static bool isInBox(const int grid[GRID_SIZE][GRID_SIZE], int startRow, int startCol, int value) {
	for (int r = 0; r < 3; r++) {
		for (int c = 0; c < 3; c++) {
			if (grid[startRow + r][startCol + c] == value) return true;
		}
	}
	return false;
}

bool isMoveValid(const int grid[GRID_SIZE][GRID_SIZE], int row, int col, int value) {
	if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE) return false;
	if (value < 1 || value > 9) return false;
	if (grid[row][col] != 0) return false; // can't place over existing number

	if (isInRow(grid, row, value)) return false;
	if (isInCol(grid, col, value)) return false;
	int boxRow = (row / 3) * 3;
	int boxCol = (col / 3) * 3;
	if (isInBox(grid, boxRow, boxCol, value)) return false;

	return true;
}

int computePossibilities(const int grid[GRID_SIZE][GRID_SIZE], int row, int col, int poss[10]) {
	if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE) return 0;
	for (int i = 0; i <= 9; i++) poss[i] = 0;
	if (grid[row][col] != 0) return 0;
	int count = 0;
	for (int v = 1; v <= 9; v++) {
		if (isMoveValid(grid, row, col, v)) {
			poss[v] = 1;
			count++;
		}
	}
	return count;
}

static bool allOneToNineUnique(const int values[GRID_SIZE]) {
	int seen[10] = {0};
	for (int i = 0; i < GRID_SIZE; i++) {
		int v = values[i];
		if (v < 1 || v > 9) return false;
		if (seen[v]) return false;
		seen[v] = 1;
	}
	return true;
}

bool isBoardComplete(const int grid[GRID_SIZE][GRID_SIZE]) {
	for (int r = 0; r < GRID_SIZE; r++) {
		for (int c = 0; c < GRID_SIZE; c++) {
			if (grid[r][c] == 0) return false;
		}
	}
	return true;
}

bool isBoardValid(const int grid[GRID_SIZE][GRID_SIZE]) {
	int buf[GRID_SIZE];
	// rows
	for (int r = 0; r < GRID_SIZE; r++) {
		for (int c = 0; c < GRID_SIZE; c++) buf[c] = grid[r][c];
		if (!allOneToNineUnique(buf)) return false;
	}
	// cols
	for (int c = 0; c < GRID_SIZE; c++) {
		for (int r = 0; r < GRID_SIZE; r++) buf[r] = grid[r][c];
		if (!allOneToNineUnique(buf)) return false;
	}
	// boxes
	for (int br = 0; br < 3; br++) {
		for (int bc = 0; bc < 3; bc++) {
			int k = 0;
			for (int r = 0; r < 3; r++) {
				for (int c = 0; c < 3; c++) {
					buf[k++] = grid[br * 3 + r][bc * 3 + c];
				}
			}
			if (!allOneToNineUnique(buf)) return false;
		}
	}
	return true;
}


