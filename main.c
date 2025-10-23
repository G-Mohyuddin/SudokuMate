#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "structs.h"
#include "cli.h"
#include "game_logic.h"
#include "solver.h"
#include "file_io.h"

// Very simple safe string copy (beginner-friendly)
static void copyString(char *dst, const char *src, size_t dstSize) {
	if (!dst || !src || dstSize == 0) return;
	size_t i = 0;
	while (i + 1 < dstSize && src[i] != '\0') {
		dst[i] = src[i];
		i++;
	}
	dst[i] = '\0';
}

static void loadTestPuzzle(Puzzle *puzzle) {
	// Clear the puzzle structure and set a friendly name/difficulty
	memset(puzzle, 0, sizeof(*puzzle));
	copyString(puzzle->name, "Test Puzzle", sizeof(puzzle->name));
	copyString(puzzle->difficulty, "Easy", sizeof(puzzle->difficulty));

	int initial[GRID_SIZE][GRID_SIZE] = {
		{5,3,0, 0,7,0, 0,0,0},
		{6,0,0, 1,9,5, 0,0,0},
		{0,9,8, 0,0,0, 0,6,0},

		{8,0,0, 0,6,0, 0,0,3},
		{4,0,0, 8,0,3, 0,0,1},
		{7,0,0, 0,2,0, 0,0,6},

		{0,6,0, 0,0,0, 2,8,0},
		{0,0,0, 4,1,9, 0,0,5},
		{0,0,0, 0,8,0, 0,7,9}
	};

	for (int r = 0; r < GRID_SIZE; r++) {
		for (int c = 0; c < GRID_SIZE; c++) {
			puzzle->grid[r][c] = initial[r][c];
		}
	}

	// sample tutorial hints (trigger on remaining empty counts)
	puzzle->hints[0].triggerCellCount = 60;
    snprintf(puzzle->hints[0].message, sizeof(puzzle->hints[0].message), "%s", "Look for a Naked Single in row 1.");
	puzzle->hints[0].hintRow = 0; puzzle->hints[0].hintCol = 2; puzzle->hints[0].hintValue = 4;
	puzzle->hints[1].triggerCellCount = 50;
    snprintf(puzzle->hints[1].message, sizeof(puzzle->hints[1].message), "%s", "Check column 5 for a Hidden Single.");
	puzzle->hints[1].hintRow = -1; puzzle->hints[1].hintCol = -1; puzzle->hints[1].hintValue = 0;
	puzzle->numHints = 2;
}

static void loadEasyGuaranteedNakedSingle(Puzzle *puzzle) {
	// Beginner-friendly easy puzzle with at least one Naked Single
	memset(puzzle, 0, sizeof(*puzzle));
	copyString(puzzle->name, "Easy: The First Step", sizeof(puzzle->name));
	copyString(puzzle->difficulty, "Easy", sizeof(puzzle->difficulty));
	int g[GRID_SIZE][GRID_SIZE] = {
		{0,0,0, 2,6,0, 7,0,0},
		{6,8,0, 0,7,0, 0,9,0},
		{1,9,0, 0,0,4, 5,0,0},
		{8,2,0, 1,0,0, 0,4,0},
		{0,0,4, 6,0,2, 9,0,0},
		{0,5,0, 0,0,3, 0,2,8},
		{0,0,9, 3,0,0, 0,7,4},
		{0,4,0, 0,5,0, 0,3,6},
		{0,0,3, 0,1,8, 0,0,0}
	};
	for (int r = 0; r < GRID_SIZE; r++) {
		for (int c = 0; c < GRID_SIZE; c++) {
			puzzle->grid[r][c] = g[r][c];
		}
	}
	// Smart hint: trigger when 38 empties AND R4C5 still empty; suggests 9
	puzzle->hints[0].triggerCellCount = 38;
    snprintf(puzzle->hints[0].message, sizeof(puzzle->hints[0].message),
        "HINT: Naked Single! Look closely at R4C5."
        " Check its row, column, and 3x3 box; only '9' fits.");
	puzzle->hints[0].hintRow = 3; puzzle->hints[0].hintCol = 4; puzzle->hints[0].hintValue = 9;
	puzzle->numHints = 1;
}

static void loadMediumHiddenPair(Puzzle *puzzle) {
	// Medium puzzle showcasing a Hidden Pair opportunity
	memset(puzzle, 0, sizeof(*puzzle));
	copyString(puzzle->name, "Medium: The Unseen Duo", sizeof(puzzle->name));
	copyString(puzzle->difficulty, "Medium", sizeof(puzzle->difficulty));
	int g[GRID_SIZE][GRID_SIZE] = {
		{0,0,2, 0,0,0, 0,9,0},
		{0,1,0, 5,0,7, 0,3,0},
		{7,0,0, 0,8,0, 0,0,1},
		{0,0,0, 0,0,0, 8,0,0},
		{5,0,1, 0,6,0, 4,0,2},
		{0,0,6, 0,0,0, 0,0,0},
		{4,0,0, 0,1,0, 0,0,6},
		{0,2,0, 6,0,5, 0,4,0},
		{0,3,0, 0,0,0, 1,0,0}
	};
	for (int r = 0; r < GRID_SIZE; r++) {
		for (int c = 0; c < GRID_SIZE; c++) {
			puzzle->grid[r][c] = g[r][c];
		}
	}
	// Smart hint: trigger when 50 empties AND both target cells still empty
	puzzle->hints[0].triggerCellCount = 50;
    snprintf(puzzle->hints[0].message, sizeof(puzzle->hints[0].message),
		"HINT: Hidden Pair! Look at the top-left 3x3 box. Where can the number '3' go?"
		" You'll see it can ONLY go in two cells: Row 1, Col 1 and Row 3, Col 3."
        " That's a Hidden Pair! You can safely remove any other candidates from those two spots.");
	puzzle->hints[0].hintRow = -1; puzzle->hints[0].hintCol = -1; puzzle->hints[0].hintValue = 0;
	puzzle->numHints = 1;
}

static void loadHardXWing(Puzzle *puzzle) {
	// Hard puzzle with an X-Wing pattern hint
	memset(puzzle, 0, sizeof(*puzzle));
	copyString(puzzle->name, "Hard: The X-Factor", sizeof(puzzle->name));
	copyString(puzzle->difficulty, "Hard", sizeof(puzzle->difficulty));
	int g[GRID_SIZE][GRID_SIZE] = {
		{0,0,0, 0,0,5, 0,1,8},
		{0,0,0, 0,0,0, 9,0,0},
		{1,0,8, 0,9,0, 0,6,0},
		{0,6,0, 1,0,0, 0,2,0},
		{0,0,2, 0,0,0, 1,0,0},
		{0,4,0, 0,0,3, 0,7,0},
		{0,2,0, 0,1,0, 7,0,4},
		{0,0,6, 0,0,0, 0,0,0},
		{4,1,0, 9,0,0, 0,0,0}
	};
	for (int r = 0; r < GRID_SIZE; r++) {
		for (int c = 0; c < GRID_SIZE; c++) {
			puzzle->grid[r][c] = g[r][c];
		}
	}
	// Smart hint: trigger when 35 empties AND X-Wing pattern cells still empty
	// X-Wing on candidate '4': Rows 2,8 (indices 1,7) and Columns 4,8 (indices 3,7)
	puzzle->hints[0].triggerCellCount = 35;
    snprintf(puzzle->hints[0].message, sizeof(puzzle->hints[0].message),
        "HINT: X-Wing! In Row 2, '4' is limited to Col 4 and Col 8."
        " In Row 8, it's the same two columns. This forms an X-Wing!"
        " Eliminate all other '4' candidates from those two columns.");
	puzzle->hints[0].hintRow = -1; puzzle->hints[0].hintCol = -1; puzzle->hints[0].hintValue = 0;
	puzzle->numHints = 1;
}

static void loadExpertSwordfish(Puzzle *puzzle) {
	// Expert puzzle highlighting a Swordfish pattern
	memset(puzzle, 0, sizeof(*puzzle));
	copyString(puzzle->name, "Expert: The Three Blades", sizeof(puzzle->name));
	copyString(puzzle->difficulty, "Expert", sizeof(puzzle->difficulty));
	int g[GRID_SIZE][GRID_SIZE] = {
		{0,0,0, 3,0,8, 0,0,2},
		{0,0,0, 0,4,0, 7,0,0},
		{0,0,1, 9,7,0, 0,8,0},
		{9,0,5, 0,0,3, 0,0,6},
		{0,3,7, 0,0,0, 5,2,0},
		{8,0,0, 5,0,0, 9,0,3},
		{0,7,0, 0,9,6, 1,0,0},
		{0,0,6, 0,3,0, 0,0,0},
		{4,0,0, 8,0,7, 0,0,0}
	};
	for (int r = 0; r < GRID_SIZE; r++) {
		for (int c = 0; c < GRID_SIZE; c++) {
			puzzle->grid[r][c] = g[r][c];
		}
	}
	// Smart hint: trigger when 45 empties AND Swordfish pattern cells still empty
	// Swordfish on candidate '9': Rows 3,7,8 (indices 2,6,7) and Columns 1,5,8 (indices 0,4,7)
	puzzle->hints[0].triggerCellCount = 45;
    snprintf(puzzle->hints[0].message, sizeof(puzzle->hints[0].message),
        "HINT: Swordfish! In rows 3, 7, and 8, the candidate '9'"
        " appears only in columns 1, 5, and 8."
        " This forms a Swordfish; remove other '9' candidates from those columns.");
	puzzle->hints[0].hintRow = -1; puzzle->hints[0].hintCol = -1; puzzle->hints[0].hintValue = 0;
	puzzle->numHints = 1;
}

int main(void) {
    (void)&loadTestPuzzle; // prevent unused-function warning for debug helper
	MENU_START:
	// Main data structures used during the game session
	Puzzle puzzle;
	GameState state; // declare state before any use
	memset(&state, 0, sizeof(state));
	while (1) {
		int menu = showStartMenu();
		if (menu == 0) {
			printMessage("Goodbye!");
			return 0;
		} else if (menu == 1) {
			// Let the player pick a difficulty, or go back
			while (1) {
				int d = showDifficultyMenu();
				if (d == 0) break;
				if (d == 1) { // Easy
					loadEasyGuaranteedNakedSingle(&puzzle);
					goto INIT_STATE;
				} else if (d == 2) { // Medium
					loadMediumHiddenPair(&puzzle);
					goto INIT_STATE;
				} else if (d == 3) { // Hard
					loadHardXWing(&puzzle);
					goto INIT_STATE;
				} else if (d == 4) { // Expert
					loadExpertSwordfish(&puzzle);
					goto INIT_STATE;
				} else {
					printMessage("This difficulty will be added next.");
				}
			}
		} else if (menu == 2) {
			memset(&puzzle, 0, sizeof(puzzle));
		copyString(puzzle.name, "Custom", sizeof(puzzle.name));
		copyString(puzzle.difficulty, "Custom", sizeof(puzzle.difficulty));
			if (!promptCustomPuzzle(puzzle.grid)) {
				printMessage("Cancelled.");
				continue;
			}
			break;
		} else if (menu == 3) {
			// Load a saved game from the saves/ folder
			char names[64][260];
			int n = listSaveFiles(names, 64);
			if (n <= 0) { printMessage("No saves found in saves/."); continue; }
			printf("Saved games:\n");
			for (int i = 0; i < n; i++) printf("%d) %s\n", i + 1, names[i]);
			printf("Select number to load, or 'dN' to delete (e.g., d2): ");
			char line[32]; if (!fgets(line, sizeof(line), stdin)) continue; int idx = atoi(line) - 1;
			if (line[0] == 'd' || line[0] == 'D') {
				int del = atoi(line + 1) - 1;
				if (del >= 0 && del < n) {
					if (deleteSaveFile(names[del])) printMessage("Save deleted."); else printMessage("Delete failed.");
				} else printMessage("Invalid selection.");
				continue;
			}
			if (idx < 0 || idx >= n) { printMessage("Invalid selection."); continue; }
            char path[320]; snprintf(path, sizeof(path), "saves/%s", names[idx]);
			GameState loaded;
			if (loadGameState(&loaded, path)) {
				memset(&puzzle, 0, sizeof(puzzle));
				copyString(puzzle.name, loaded.activePuzzleName, sizeof(puzzle.name));
				copyString(puzzle.difficulty, loaded.activeDifficulty, sizeof(puzzle.difficulty));
				for (int r = 0; r < GRID_SIZE; r++) for (int c = 0; c < GRID_SIZE; c++) puzzle.grid[r][c] = loaded.currentGrid[r][c];
				memcpy(&state, &loaded, sizeof(GameState));
				break;
			} else { printMessage("Failed to load save file."); }
		} else if (menu == 4) {
			printTopHighScores("highscores.txt", 10);
		}
	}

INIT_STATE:
	if (state.activePuzzleName[0] == '\0') {
		memset(&state, 0, sizeof(state));
		copyString(state.activeDifficulty, puzzle.difficulty, sizeof(state.activeDifficulty));
		copyString(state.activePuzzleName, puzzle.name, sizeof(state.activePuzzleName));
		for (int r = 0; r < GRID_SIZE; r++) {
			for (int c = 0; c < GRID_SIZE; c++) {
				state.currentGrid[r][c] = puzzle.grid[r][c];
				state.isFixed[r][c] = (puzzle.grid[r][c] != 0);
			}
		}
	}

	printf("\nSudokuMate - Interactive\n\n");
	state.timerStart = time(NULL);

	while (1) {
		printBoard((const int (*)[GRID_SIZE])state.currentGrid);
		// Show elapsed time in minutes:seconds
		unsigned int elapsed = state.elapsedSeconds;
		if (state.timerStart != 0) {
			unsigned int now = (unsigned int)time(NULL);
			if (now >= (unsigned int)state.timerStart) elapsed += (now - (unsigned int)state.timerStart);
		}
		printf("Time %02u:%02u | Puzzle: %s | Difficulty: %s\n",
			(elapsed / 60) % 60, elapsed % 60, state.activePuzzleName, state.activeDifficulty);
		int row, col, value;
		int rc = promptMove(&row, &col, &value);
		if (rc == 0) {
			// Player chose to quit the current game; return to the main menu
			printMessage("Returning to main menu...");
			memset(&state, 0, sizeof(state));
			memset(&puzzle, 0, sizeof(puzzle));
			goto MENU_START;
		}
		if (rc == 2) {
			int working[GRID_SIZE][GRID_SIZE];
			for (int r = 0; r < GRID_SIZE; r++) {
				for (int c = 0; c < GRID_SIZE; c++) {
					working[r][c] = state.currentGrid[r][c];
				}
			}
			if (solveSudoku(working)) {
				for (int r = 0; r < GRID_SIZE; r++) {
					for (int c = 0; c < GRID_SIZE; c++) {
						state.currentGrid[r][c] = working[r][c];
					}
				}
				// Recompute total elapsed time before recording high score
				unsigned int now = (unsigned int)time(NULL);
				unsigned int total = state.elapsedSeconds + (state.timerStart ? (now - (unsigned int)state.timerStart) : 0);
				printMessage("Solved.");
				appendHighScore(state.activeDifficulty, state.activePuzzleName, total, "highscores.txt");
			} else {
				printMessage("No solution found (board may be invalid).");
			}
			continue;
		}
		if (rc == 4) {
			int poss[10];
			int count = computePossibilities((const int (*)[GRID_SIZE])state.currentGrid, row, col, poss);
			if (count == 0) {
				printMessage("No possibilities (cell filled or blocked).");
			} else {
				printf("Possible values at (%d,%d): ", row + 1, col + 1);
				for (int v = 1; v <= 9; v++) if (poss[v]) printf("%d ", v);
				printf("\n");
			}
			continue;
		}
		if (rc == 3) {
			char used[320];
			// Update accumulated time before saving so it persists
			unsigned int now = (unsigned int)time(NULL);
			if (state.timerStart != 0 && now >= (unsigned int)state.timerStart) {
				state.elapsedSeconds += (now - (unsigned int)state.timerStart);
			}
			state.timerStart = time(NULL);
			if (saveGameStateAuto(&state, used, sizeof(used))) {
				printf("Saved to %s\n", used);
			} else {
				printMessage("Save failed.");
			}
			continue;
		}
		if (rc == 5) { // undo
			if (state.moveCount > 0) {
				Move m = state.moves[state.moveCount - 1];
				state.currentGrid[m.row][m.col] = m.previousValue;
				state.moveCount--;
				state.redoCount++;
				printMessage("Undone last move.");
			} else {
				printMessage("Nothing to undo.");
			}
			continue;
		}
		if (rc == 6) { // redo
			if (state.redoCount > 0) {
				Move m = state.moves[state.moveCount];
				state.currentGrid[m.row][m.col] = m.newValue;
				state.moveCount++;
				state.redoCount--;
				printMessage("Redone move.");
			} else {
				printMessage("Nothing to redo.");
			}
			continue;
		}
		if (row < 0 || row >= GRID_SIZE || col < 0 || col >= GRID_SIZE) {
			printMessage("Row/col must be between 1 and 9.");
			continue;
		}
		if (state.isFixed[row][col]) {
			printMessage("Cannot change a fixed (given) cell.");
			continue;
		}
		if (!isMoveValid((const int (*)[GRID_SIZE])state.currentGrid, row, col, value)) {
			printMessage("Invalid move by Sudoku rules.");
			continue;
		}
		// push move for undo
		if (state.moveCount < (sizeof(state.moves) / sizeof(state.moves[0]))) {
			state.moves[state.moveCount].row = row;
			state.moves[state.moveCount].col = col;
			state.moves[state.moveCount].previousValue = state.currentGrid[row][col];
			state.moves[state.moveCount].newValue = value;
			state.moveCount++;
			state.redoCount = 0; // clear redo stack
		}
		state.currentGrid[row][col] = value;

		// hint triggers: count empties and compare with next hint
		int empty = 0;
		for (int rr = 0; rr < GRID_SIZE; rr++) {
			for (int cc = 0; cc < GRID_SIZE; cc++) {
				if (state.currentGrid[rr][cc] == 0) empty++;
			}
		}
		if (state.nextHintIndex < puzzle.numHints) {
			Hint *h = &puzzle.hints[state.nextHintIndex];
			bool targetEmptyOk = true;
			if (h->hintRow >= 0 && h->hintCol >= 0) {
				targetEmptyOk = (state.currentGrid[h->hintRow][h->hintCol] == 0);
			}
			if (empty == h->triggerCellCount && targetEmptyOk) {
				// Print hint message as-is to avoid duplicate prefixes
				printf("%s\n", h->message);
				if (h->hintRow >= 0 && h->hintCol >= 0 && h->hintValue >= 1) {
					printf("Try (%d,%d) = %d\n", h->hintRow + 1, h->hintCol + 1, h->hintValue);
				}
				state.nextHintIndex++;
			}
		}

		// completion detection
		if (isBoardComplete((const int (*)[GRID_SIZE])state.currentGrid)) {
			if (isBoardValid((const int (*)[GRID_SIZE])state.currentGrid)) {
				unsigned int now = (unsigned int)time(NULL);
				unsigned int total = state.elapsedSeconds + (state.timerStart ? (now - (unsigned int)state.timerStart) : 0);
				printf("\nCompleted! Time: %u seconds\n", total);
				appendHighScore(state.activeDifficulty, state.activePuzzleName, total, "highscores.txt");
				// After completion, return to main menu
				memset(&state, 0, sizeof(state));
				memset(&puzzle, 0, sizeof(puzzle));
				printMessage("Returning to main menu...");
				goto MENU_START;
			} else {
				printMessage("Board is full but invalid. Keep trying or undo.");
			}
		}
	}

	return 0;
}
