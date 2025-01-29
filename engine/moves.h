#ifndef MOVES_H
#define MOVES_H

#include "types.h"
#include "engine.h"

enum {
    PROMOTING_NONE,
    PROMOTING_QUENE,
    PROMOTING_ROOK,
    PROMOTING_BISHOP,
    PROMOTING_KNIGHT
};

typedef enum {
    MOVE_EN_PASSAGE,
    MOVE_REGULAR,
    MOVE_SAME_FIGURE_CAPTURED,
    MOVE_CAPTURED,
} MoveType;

u64 getPossibleMoves(GameState* state, u8 pos);
bool movePiece(GameState* state, u8 pos1, u8 pos2, u8 promotionType);

#endif