#ifndef MOVES_H
#define MOVES_H

#include "../types.h"
#include "../pgn.h"
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

bool makeMove(BoardState* board, PGN* pgn, u8 from, u8 to, PROMOTION_TYPE type);
bool makeMoveBack(BoardState* board, PGN* pgn);
bool makeMoveForward(BoardState* board, PGN* pgn);
PGN_MOVE applyMoveWithCheck(BoardState* board, u8 pos1, u8 pos2, PROMOTION_TYPE promotionType);
void applyMove(BoardState* board, PGN_MOVE move);
void invertMove(BoardState* board, PGN* pgn, u32 idx);
u64 getPossibleMoves(BoardState* board, u8 pos);
u8 getBoardFromPromotion(u8 promotionType);

#endif