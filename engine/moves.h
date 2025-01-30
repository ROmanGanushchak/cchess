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

typedef struct {
    bool isValid;
    GAME_STATES state;
} MOVE_RET;

u64 getPossibleMoves(BoardState* board, u8 pos);
MOVE_RET movePiece(BoardState* board, u8 pos1, u8 pos2, u8 promotionType);

#endif