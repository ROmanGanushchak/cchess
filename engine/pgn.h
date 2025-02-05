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
PGN createPGN(BoardState board);


typedef struct {
    PGN_MOVE* savedMoves;
    u32 len;
    u32 cap;
    u16 idx;
} FORK_INFO;

typedef struct {
    FORK_INFO* arr;
    u32 len;
    u32 cap;
} FORK_ARR;

typedef struct {
    BoardState board;
    PGN* pgn;
    PGN_MOVE* moves;
    u16 idx;
} PGN_VIEW;

// As index uses the idx for view
bool pgnViewMove(PGN_VIEW* view, i32 offset);
bool pgnViewMakeMove(PGN_VIEW* view, PGN_MOVE move);
void pgnViewCreateFork(PGN_VIEW* view);
bool pgnViewLeaveFork(PGN_VIEW* view);
// destroyes all elements of the view with an exception of pgn, doesnt free the struct itself
void releasePgnView(PGN_VIEW* view);

typedef struct {
    PGN_VIEW view;
    bool isValid;
} OPTIONAL_PGN_VIEW;

// assumes that starting position is a default one
STR convertPgnToStr(PGN* pgn);
// assumes that string is correct and doesnt valiedates it
OPTIONAL_PGN_VIEW convertStrToPgn(STR str);

#endif