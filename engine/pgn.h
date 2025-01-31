#ifndef PGN_H
#define PGN_H
#include "types.h"
#include "engine.h"
#include "moves.h"

typedef struct {
    u8 from;
    u8 to;
    u8 gameState : 5; // GAME_STATES
    u8 isShortCastleLost : 1;
    u8 isLongCastleLost : 1;
    u8 turn : 1;
    u8 moveType : 4; // MOVE_TYPE
    u8 promotion: 4; // PROMOTION_TYPE
    u8 piece : 4; // Boards
} PGN_MOVE;

typedef struct {
    BoardState initBoard;
    PGN_MOVE* moves;
    u32 len;
    u32 cap;
} PGN;

DEFINE_PAIR(PGN, BoardState, PAIR_PGN_BoardState)

void addMove(PGN* pgn, PGN_MOVE move);
void removeSince(PGN* pgn, u32 idx);
void applyMove(BoardState* state, PGN_MOVE move);
void invertMove(BoardState* state, PGN_MOVE move);

// assumes that starting position is a default one
STR convertPgnToStr(PGN* pgn);
// assumes that string is correct and doesnt valiedates it
PAIR_PGN_BoardState convertStrToPgn(STR str);

#endif