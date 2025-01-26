#include <stdio.h>
#include "figures.h"
#include "types.h"
#include "slidingMove.h"

const u64 SetNoRightColumn = ~(LEFT_EDGE << 7);
const u64 SetNoLeftColumn =  ~LEFT_EDGE;
const u64 SetNoTwoRightColumns = ~((LEFT_EDGE << 7) | (LEFT_EDGE << 6));
const u64 SetNoTwoLeftColumns = ~(LEFT_EDGE | (LEFT_EDGE << 1));

u64 possiblePawnAttacks(GameState* state, bool side) {
    if (side) {
        u64 pawns = state->figures[BPAWN] & state->figures[BCOLOR];
        return ( (pawns & SetNoLeftColumn) << 7) | ( (pawns & SetNoRightColumn) << 9);
    } else {
        u64 pawns = state->figures[BPAWN] & ~state->figures[BCOLOR];
        return ( (pawns & SetNoRightColumn) >> 7) | ( (pawns & SetNoLeftColumn) >> 9);
    }
}

u64 possibleKnightAttacks(GameState* state, bool side) {
    u64 knights = state->figures[BKNIGHT] & ((side) ? state->figures[BCOLOR] : ~state->figures[BCOLOR]);
    return 
            (knights & SetNoTwoLeftColumns) >> 10 | (knights & SetNoTwoLeftColumns) << 6 |
            (knights & SetNoTwoRightColumns) >> 6 | (knights & SetNoTwoRightColumns) << 10 |
            (knights & SetNoRightColumn) >> 15 | (knights & SetNoRightColumn) << 17 |
            (knights & SetNoLeftColumn) >> 17 | (knights & SetNoLeftColumn) << 15;
}

u64 possibleKingAttacks(GameState* state, bool side) {
    u64 kings = state->figures[BKING] & ((side) ? state->figures[BCOLOR] : ~state->figures[BCOLOR]);
    return kings >> 8 | kings << 8 | 
        (kings & SetNoLeftColumn) >> 1 | (kings & SetNoLeftColumn) << 7 | (kings & SetNoLeftColumn) >> 9 | 
        (kings & SetNoRightColumn) << 1 | (kings & SetNoRightColumn) << 9 | (kings & SetNoRightColumn) >> 7; 
}

u64 getAttacks(GameState* state, bool side) {
    u64 rooks = state->figures[BROOK] & ( (side) ? state->figures[BCOLOR] : ~state->figures[BCOLOR] );
    u64 bishops = state->figures[BBISHOP] & ( (side) ? state->figures[BCOLOR] : ~state->figures[BCOLOR] );
    u64 queens = state->figures[BQUEEN] & ( (side) ? state->figures[BCOLOR] : ~state->figures[BCOLOR] );
    u64 occupied = state->figures[BOCCUPIED];
    u64 occupiedReversed = reverseRaws(occupied);

    u64 res = possibleKingAttacks(state, side) | possibleKnightAttacks(state, side) | possiblePawnAttacks(state, side);
    for (u8 i=0; i<64; i++) {
        if (((rooks >> i) & 1) || ((queens >> i) & 1))
            res |= getLinearMovement(occupied, i);
        if (((bishops >> i) & 1) || ((queens >> i) & 1))
            res |= getDiagonalMovement(occupied, occupiedReversed, i);
    }

    return res;
}



// u64 possibleMovesKing(GameState* state);
// u64 possibleMovesQuene(GameState* state);
// u64 possibleMovesRook(GameState* state);
// u64 possibleMovesBishop(GameState* state);
// u64 possibleMovesKnight(GameState* state);
// u64 possibleMovesPawns(GameState* state);

