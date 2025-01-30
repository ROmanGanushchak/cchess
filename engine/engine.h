#ifndef ENGINE_H
#define ENGINE_H

#include <stdint.h>
#include <stdbool.h>
#include "types.h"

typedef struct {
    u8 from;
    u8 to;
} MOVE;

typedef enum {
    BOCCUPIED,
    BCOLOR,
    BKING,
    BKNIGHT,
    BPAWN,
    BBISHOP,
    BROOK,
    BQUEEN
} Boards;

typedef enum {
    GAME_WHITE_WON,
    GAME_BLACK_WON,
    GAME_DRAW,
    GAME_WHITE_CHECK,
    GAME_BLACK_CHECK,
    GAME_REGULAR,
} GAME_STATES;

#define BOARD_COUNT 8
typedef struct {
    u64 figures[BOARD_COUNT];
    MOVE lastMove;
    MOVE lastMoveBefore;
    u8 halfMoves;
    u8 repeatCount : 3;
    bool turn : 1; // 1 - white, 0 - black
    bool isEnd: 1;
    bool doubleCastleWhite : 1;
    bool doubleCastleBlack : 1;
    bool thribleCastleWhite : 1;
    bool thribleCastleBlack : 1;
} BoardState;

extern BoardState initialBoardState;

void startEngine();
void printBoard(BoardState* board);
u8 getFigureBoard(BoardState* board, u8 pos);
GAME_STATES updateGameState(BoardState* board, u8 pos1, u8 pos2, bool isCaptureOrAdvance);
bool isGameEnd(GAME_STATES state);
void printU64(u64 a);

#endif