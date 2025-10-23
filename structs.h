#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>
#include <stddef.h>
#include <time.h>

#define GRID_SIZE 9

typedef struct Hint {
	int triggerCellCount; // when remaining empty cells equals this, show this hint
	char message[256];
	int hintRow; // optional: row for a suggested move (-1 if N/A)
	int hintCol; // optional: col for a suggested move (-1 if N/A)
	int hintValue; // optional: value for a suggested move (0 if N/A)
} Hint;

typedef struct Puzzle {
	char name[64];
	char difficulty[32];
	int grid[GRID_SIZE][GRID_SIZE]; // initial givens (0 means empty)
	int solution[GRID_SIZE][GRID_SIZE]; // optional: filled by loader/solver
	Hint hints[16];
	size_t numHints;
} Puzzle;

typedef struct Move {
	int row;
	int col;
	int previousValue;
	int newValue;
} Move;

typedef struct GameState {
	int currentGrid[GRID_SIZE][GRID_SIZE];
	bool isFixed[GRID_SIZE][GRID_SIZE]; // true if original given, not editable
	char activeDifficulty[32];
	char activePuzzleName[64];

	Move moves[128];
	size_t moveCount;
	size_t redoCount; // number of redoable moves stored after undos

	time_t timerStart; // 0 when not started
	unsigned int elapsedSeconds; // accumulated when paused/saved

	// index of next hint to trigger from the loaded puzzle
	size_t nextHintIndex;
} GameState;

#endif // STRUCTS_H


