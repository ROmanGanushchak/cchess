#ifndef PGN_H
#define PGN_H
#include "types.h"
#include "engine.h"

typedef struct {
    u8 from;
    u8 to;
    u8 halfMoves;
    u8 gameState : 5; // GAME_STATES
    bool isShortCastleGone : 1;
    bool isLongCastleGone : 1;
    bool isValidated: 1;
    u8 promotion: 4; // typeOf PROMOTION_TYPE
    u8 captured: 4;  // typeOf Boards
    u8 moveType : 4; // MOVE_TYPE
    u8 piece : 3; // Boards
    u8 repeatCount: 3;
    bool isColumn : 1; // is Column specified in PGN, is needed only if isValidated is false
    bool isRow: 1; // is Row specified in PGN, is needed only if isValidated is false
} PGN_MOVE;

typedef struct {
    BoardState initBoard;
    PGN_MOVE* moves;
    u32 len;
    u32 cap;
    u32 idx;
} PGN;

void addMove(PGN* pgn, PGN_MOVE move);
void removeSince(PGN* pgn, u32 idx);
PGN createPGN(BoardState board);


// typedef struct {
//     PGN_MOVE* savedMoves;
//     u32 len;
//     u32 cap;
//     u16 idx;
// } FORK_INFO;

// typedef struct {
//     FORK_INFO* arr;
//     u32 len;
//     u32 cap;
// } FORK_ARR;

// typedef struct {
//     BoardState board;
//     PGN* pgn;
//     FORK_ARR forkstack;
//     u16 idx;
// } PGN_VIEW;

// void saveMoveToView(PGN_VIEW* view, PGN_MOVE move);
// PGN_MOVE getPGNViewMove(PGN_VIEW* view, u16 idx);
// void createNewFork(PGN_VIEW* view, u16 idx);
// void exitFork(PGN_VIEW* view);
// u32 getPGN_VIEW_len(PGN_VIEW* view);


DEFINE_PAIR(PGN, BoardState, PAIR_PGN_BoardState)

// assumes that starting position is a default one
STR convertPgnToStr(PGN* pgn);
// assumes that string is correct and doesnt valiedates it
PAIR_PGN_BoardState convertStrToPgn(STR str);

#endif