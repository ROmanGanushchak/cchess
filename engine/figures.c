#include <stdio.h>
#include "figures.h"
#include "types.h"
#include "slidingMove.h"

const u64 SetNoRightColumn = ~(LEFT_EDGE << 7);
const u64 SetNoLeftColumn =  ~LEFT_EDGE;
const u64 SetNoTwoRightColumns = SetNoRightColumn & (SetNoRightColumn << 1);
const u64 SetNoTwoLeftColumns = SetNoLeftColumn & (SetNoLeftColumn >> 1);

u64 possiblePawnAttacks(GameState* state, bool side) {
    if (side) {
        u64 pawns = state->figures[BPAWN] & state->figures[BCOLOR];
        return ( (pawns & SetNoRightColumn) >> 7) | ( (pawns & SetNoLeftColumn) >> 9);
    } else {
        u64 pawns = state->figures[BPAWN] & ~state->figures[BCOLOR];
        return ( (pawns & SetNoRightColumn) << 7) | ( (pawns & SetNoLeftColumn) << 9);
    }
}

u64 possibleKnightAttacks(GameState* state, bool side) {
    u64 knights = state->figures[BKNIGHT] & ((side) ? state->figures[BCOLOR] : ~state->figures[BCOLOR]);
    return (knights & SetNoTwoLeftColumns) >> 6 | (knights & SetNoTwoLeftColumns) << 10 |
            (knights & SetNoTwoRightColumns) >> 10 | (knights & SetNoTwoRightColumns) << 6 |
            (knights & SetNoLeftColumn) >> 15 | (knights & SetNoTwoLeftColumns) << 17 |
            (knights & SetNoRightColumn) >> 17 | (knights & SetNoTwoRightColumns) << 15;
}

u64 possibleKingAttacks(GameState* state, bool side) {
    u64 kings = state->figures[BKING] & ((side) ? state->figures[BCOLOR] : ~state->figures[BCOLOR]);
    return kings >> 8 | kings << 8 | 
        (kings & SetNoLeftColumn) << 1 | (kings & SetNoLeftColumn) << 9 | (kings & SetNoLeftColumn) >> 7 | 
        (kings & SetNoRightColumn) << 1 | (kings & SetNoRightColumn) << 7 | (kings & SetNoRightColumn) >> 9; 
}

u64 possibleRookAttacks(GameState* state, bool side) {
    u32 rooks = state->rooks;
    u64 res = 0;
    for (int i=(side) ? 0 : 2, n=(side) ? 2 : 4; i<n; i++) {
        u8 rook = (rooks >> (i << 3)) & 0xFF;
        if (rook & 0x80) break;
        res |= getLinearMovement(state->figures[BOCCUPIED], rook & 0x3F);

        printf("For ROOK %d:\n", rook);
        printU64(getLinearMovement(state->figures[BOCCUPIED], rook & 0x3F));
        printf("\n");
        res |= getLinearMovement(state->figures[BOCCUPIED], rook & 0x3F);
    }
    return res;
}

u64 possibleBishopAttacks(GameState* state, bool side) {
    u32 bishops = state->bishops;
    u64 res = 0;
    for (int i=(side) ? 0 : 2, n=(side) ? 2 : 4; i<n; i++) {
        u8 bishop = ( bishops >> (i >> 3) ) & 0xFF;
        if (bishop & 0x80) break;
        res |= getDiagonalMovement(state->figures[BOCCUPIED], bishop & 0x3F);
    }
    return res;
}

u64 possibleQueneAttacks(GameState* state, bool side) {
    u8 queen = ((side) ? (state->queens & 0xFF) : (state->queens)) >> 8;
    if (queen & 0x80) return 0;
    printf("Queen pos: %d\n", queen & 0x3F);
    return getDiagonalMovement(state->figures[BOCCUPIED], queen & 0x3F) | getLinearMovement(state->figures[BOCCUPIED], queen & 0x3F);
}



// u64 possibleMovesKing(GameState* state);
// u64 possibleMovesQuene(GameState* state);
// u64 possibleMovesRook(GameState* state);
// u64 possibleMovesBishop(GameState* state);
// u64 possibleMovesKnight(GameState* state);
// u64 possibleMovesPawns(GameState* state);

