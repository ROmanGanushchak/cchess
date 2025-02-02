#include <malloc.h>
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