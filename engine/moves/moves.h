#ifndef MOVES_H
#define MOVES_H

#include "../types.h"
#include "../engine.h"

typedef enum {
    PROMOTING_NONE,
    PROMOTING_QUENE,
    PROMOTING_ROOK,
    PROMOTING_BISHOP,
    PROMOTING_KNIGHT
} PROMOTION_TYPE;

typedef enum {
    MOVE_EN_PASSAGE,
    MOVE_REGULAR,
    MOVE_SAME_FIGURE_CAPTURED,
    MOVE_CAPTURED,
    MOVE_CASTLING,
    MOVE_PROMOTION
} MOVE_TYPE;

typedef struct {
    bool isValid;
    GAME_STATES state;
} MOVE_RET;

u64 getPossibleMoves(BoardState* board, u8 pos);
MOVE_RET movePiece(BoardState* board, u8 pos1, u8 pos2, PROMOTION_TYPE promotionType);
u8 getBoardFromPromotion(u8 promotionType);

#endif