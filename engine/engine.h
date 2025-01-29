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

#define BOARD_COUNT 8
typedef struct {
    u64 figures[BOARD_COUNT];
    MOVE lastMove;
    bool turn : 1; // 1 - white, 0 - black
    bool isCheck : 1;
    bool isMate : 1;
    bool doubleCastleWhite : 1;
    bool doubleCastleBlack : 1;
    bool thribleCastleWhite : 1;
    bool thribleCastleBlack : 1;
} GameState;

extern GameState initialGameState;

void startEngine();
void printBoard(GameState* state);
u8 getFigureBoard(GameState* state, u8 pos);
bool isCkeckmate(GameState* state);
void printU64(u64 a);

#endif