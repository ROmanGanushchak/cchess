#include <malloc.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "pgn.h"
#include "moves.h"
#include "unsafeMoves.h"
#include "attacks.h"

void addMove(PGN* pgn, PGN_MOVE move) {
    if (pgn->len + 1 > pgn->cap) {
        pgn->cap <<= 1;
        pgn->moves = realloc(pgn->moves, pgn->cap * sizeof(PGN_MOVE));
    }
    pgn->moves[pgn->len++] = move;
}

PGN createPGN(BoardState board) {
    PGN pgn = (PGN){.cap=16, .len=0, .initBoard=board, .idx=-1};
    pgn.moves = malloc(pgn.cap * sizeof(PGN_MOVE));
    return pgn;
}

// bool pgnViewMove(PGN_VIEW* view, i32 offset) {
//     if (offset == 0) return true;
//     i32 idx = view->idx;
//     if (offset > 0) {
//         for (int i=idx, n=min(); i<offset; i++) {

//         }
//     }
// }

// void releasePgnView(PGN_VIEW* view) {
//     FORK_INFO* forks = view->forkstack.arr;
//     if (forks) {
//         for (int i=0; i<view->forkstack.len; i++)
//             free(forks[i]);
//         free(forks);
//     }
// }