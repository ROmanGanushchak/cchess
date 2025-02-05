#include <stdio.h>
#include "attacks.h"
#include "../types.h"
#include "slidingMove.h"

const u64 SetNoRightColumn = ~(LEFT_EDGE << 7);
const u64 SetNoLeftColumn =  ~LEFT_EDGE;
const u64 SetNoTwoRightColumns = ~((LEFT_EDGE << 7) | (LEFT_EDGE << 6));
const u64 SetNoTwoLeftColumns = ~(LEFT_EDGE | (LEFT_EDGE << 1));

u64 possiblePawnAttacks(BoardState* board, bool side) {
    if (side) {
        u64 pawns = board->figures[BPAWN] & board->figures[BCOLOR];
        return ( (pawns & SetNoLeftColumn) << 7) | ( (pawns & SetNoRightColumn) << 9);
    } else {
        u64 pawns = board->figures[BPAWN] & ~board->figures[BCOLOR];
        return ( (pawns & SetNoRightColumn) >> 7) | ( (pawns & SetNoLeftColumn) >> 9);
    }
}

u64 possibleKnightAttacks(u64 knights, u64 color, bool side) {
    knights &= (side) ? color : ~color;
    return 
            (knights & SetNoTwoLeftColumns) >> 10 | (knights & SetNoTwoLeftColumns) << 6 |
            (knights & SetNoTwoRightColumns) >> 6 | (knights & SetNoTwoRightColumns) << 10 |
            (knights & SetNoRightColumn) >> 15 | (knights & SetNoRightColumn) << 17 |
            (knights & SetNoLeftColumn) >> 17 | (knights & SetNoLeftColumn) << 15;
}

u64 _possibleKingAttacks(u64 kings) {
    return kings >> 8 | kings << 8 | 
        (kings & SetNoLeftColumn) >> 1 | (kings & SetNoLeftColumn) << 7 | (kings & SetNoLeftColumn) >> 9 | 
        (kings & SetNoRightColumn) << 1 | (kings & SetNoRightColumn) << 9 | (kings & SetNoRightColumn) >> 7; 
}

u64 possibleKingAttacks(BoardState* board, bool side) {
    return _possibleKingAttacks(board->figures[BKING] & (side ? board->figures[BCOLOR] : ~board->figures[BCOLOR]));
}

u64 getAttacks(BoardState* board, bool side) {
    u64 rooks = board->figures[BROOK] & ( (side) ? board->figures[BCOLOR] : ~board->figures[BCOLOR] );
    u64 bishops = board->figures[BBISHOP] & ( (side) ? board->figures[BCOLOR] : ~board->figures[BCOLOR] );
    u64 queens = board->figures[BQUEEN] & ( (side) ? board->figures[BCOLOR] : ~board->figures[BCOLOR] );
    u64 occupied = board->figures[BOCCUPIED];
    u64 occupiedReversed = reverseRaws(occupied);

    u64 res = possibleKingAttacks(board, side) | possibleKnightAttacks(board->figures[BKNIGHT], board->figures[BCOLOR], side) | possiblePawnAttacks(board, side);
    for (u8 i=0; i<64; i++) {
        if (((rooks >> i) & 1) || ((queens >> i) & 1))
            res |= getLinearMovement(occupied, i);
        if (((bishops >> i) & 1) || ((queens >> i) & 1))
            res |= getDiagonalMovement(occupied, occupiedReversed, i);
    }

    return res;
}
