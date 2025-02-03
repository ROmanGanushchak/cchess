#include <malloc.h>
#include <assert.h>
#include <stdio.h>
#include "pgn.h"
#include "moves.h"
#include "unsafeMoves.h"

void addMove(PGN* pgn, PGN_MOVE move) {
    if (pgn->len + 1 > pgn->cap) {
        pgn->cap <<= 1;
        pgn->moves = realloc(pgn->moves, pgn->cap * sizeof(PGN_MOVE));
    }
    pgn->moves[pgn->len++] = move;
}

void removeSince(PGN* pgn, u32 idx) {
    pgn->len = idx;
}

PGN createPGN(BoardState board) {
    PGN pgn = (PGN){.cap=16, .len=0, .initBoard=board, .idx=-1};
    pgn.moves = malloc(pgn.cap * sizeof(PGN_MOVE));
    return pgn;
}

// PGN_MOVE getPGNViewMove(PGN_VIEW* view, u16 idx) {
//     printf("Got len: %d, idx: %d\n", getPGN_VIEW_len(view), idx);
//     i32 forkIdx = view->forkstack.len-1;
//     for (; forkIdx >= 0 && view->forkstack.arr[forkIdx].idx < idx; forkIdx--);
//     if (forkIdx != -1) {
//         i32 _idx = idx - view->forkstack.arr[forkIdx].idx;
//         assert(_idx < view->forkstack.arr[forkIdx].len);
//         return view->forkstack.arr[forkIdx].savedMoves[_idx];
//     }
//     assert(idx < view->pgn->len);
//     return view->pgn->moves[idx];
// }

// void saveMoveToView(PGN_VIEW* view, PGN_MOVE move) {
//     if (view->forkstack.len == 0) {
//         addMove(view->pgn, move);
//         return;
//     }
//     FORK_INFO *fork = &view->forkstack.arr[view->forkstack.len-1];
//     if (fork->len == fork->cap) {
//         if (fork->cap == 0) {
//             fork->cap = 8;
//             fork->savedMoves = malloc(fork->cap * sizeof(fork->savedMoves[0]));
//         } else {
//             fork->cap *= 2;
//             fork->savedMoves = realloc(fork->savedMoves, fork->cap * sizeof(fork->savedMoves[0]));
//         }
//     }
//     fork->savedMoves[fork->len++] = move;
// }

// void createNewFork(PGN_VIEW* view, u16 idx) {
//     FORK_ARR *forks = &view->forkstack;
//     assert(forks->arr[forks->len-1].idx < idx && "Forks have to be consecutive");
//     if (forks->len == forks->cap) {
//         if (forks->cap == 0) {
//             forks->cap = 8;
//             forks->arr = malloc(forks->cap * sizeof(forks->arr[0]));
//         } else {
//             forks->cap *= 2;
//             forks->arr = realloc(forks->arr, forks->cap * sizeof(forks->arr[0]));
//         }
//     }
//     forks->arr[forks->len++] = (FORK_INFO){.cap=0, .len=0, .idx=idx, .savedMoves=NULL};
// }

// void exitFork(PGN_VIEW* view) {
//     if (view->forkstack.len == 0) {
//         fprintf(stderr, "Trying to delete non-existing fork for PGN_VIEW, %s:%d\n", __FILE__, __LINE__);
//         return;
//     }
//     free(view->forkstack.arr[view->forkstack.len-1].savedMoves);
//     view->forkstack.len--;
// }

// u32 getPGN_VIEW_len(PGN_VIEW* view) {
//     if (view->forkstack.len == 0)
//         return view->pgn->len;
//     FORK_INFO lastFork = view->forkstack.arr[view->forkstack.len-1];
//     return lastFork.idx + lastFork.len;
// }