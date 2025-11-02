#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>
#include <stddef.h>
#include <time.h>

#define GRID_SIZE 9

typedef struct Hint {
	int triggerCellCount;
	char message[256];
	int hintRow; 
	int hintCol; 
	int hintValue;
} Hint;

typedef struct Puzzle {
	char name[64];
	char difficulty[32];
	int grid[GRID_SIZE][GRID_SIZE]; 
	int solution[GRID_SIZE][GRID_SIZE]; 
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
	bool isFixed[GRID_SIZE][GRID_SIZE]; 
	char activeDifficulty[32];
	char activePuzzleName[64];

	Move moves[128];
	size_t moveCount;
	size_t redoCount; 

	time_t timerStart;
	unsigned int elapsedSeconds; 

	
	size_t nextHintIndex;
} GameState;

#endif 



