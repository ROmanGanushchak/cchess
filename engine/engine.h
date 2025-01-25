#ifndef ENGINE_H
#define ENGINE_H

#include <stdint.h>
#include <stdbool.h>
#include "types.h"

typedef struct {
    u8 from;
    u8 to;
} MOVE;

typedef struct {
    u8 *a;
    u32 len, size;
} array_pos;

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

typedef struct {
    u64 figures[8];
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

bool checkIsCheck(GameState* state, bool side);
bool move(GameState* state, uint8_t pos1, uint8_t pos2);
array_pos getPossibleMoves(GameState* state, uint8_t x, uint8_t y);
void startEngine();
void printBoard(GameState* state);
int getFigure(GameState* state, u8 pos);

#endif