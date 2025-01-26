#include <stdio.h>
#include "types.h"
#include "moves.h"
#include "slidingMove.h"
#include "figures.h"

int getFigureBoard(GameState* state, u8 pos) {
    if (state->figures[BPAWN] & (ULL1 << pos)) return BPAWN;
    if (state->figures[BBISHOP] & (ULL1 << pos)) return BBISHOP;
    if (state->figures[BQUEEN] & (ULL1 << pos)) return BQUEEN;
    if (state->figures[BROOK] & (ULL1 << pos)) return BROOK;
    if (state->figures[BKNIGHT] & (ULL1 << pos)) return BKNIGHT;
    if (state->figures[BKING] & (ULL1 << pos)) return BKING;
    return BOCCUPIED;
}

void __movePiece(GameState* state, u8 pos1, u8 pos2, u8 boardType) {
    printf("MovedFigureType: %d\n", boardType);
    bool side = (state->figures[BCOLOR] >> pos1) & 1;
    state->figures[BCOLOR] &= ~(ULL1 << pos1 | ULL1 << pos2);
    state->figures[BCOLOR] |= (u64)side << pos2;
    state->figures[BOCCUPIED] = ( state->figures[BOCCUPIED] & ~(ULL1 << pos1) ) | ( ULL1 << pos2 );
    state->figures[boardType] = ( state->figures[boardType] & ~(ULL1 << pos1) ) | ( ULL1 << pos2 );
}

u64 removeInvalidMoves(GameState* state, u64 moves, u8 pos, u8 boardType) {
    bool side = (state->figures[BCOLOR] >> pos) & 1;
    u64 king = state->figures[BKING] & (side ? state->figures[BCOLOR] : ~state->figures[BCOLOR]);
    for (u8 i=0; i<64; i++) {
        if (!( (moves >> i) & 1 )) continue;
        /* when figure captures other figure both figures are presented on the board, 
        when we move back and both pieces have same type we need to restore the captured figure */
        bool isFigureRestoreNeeded = (state->figures[boardType] >> i) & 1;
        bool wasFigureBefore = (state->figures[BOCCUPIED] >> i) & 1;
        __movePiece(state, pos, i, boardType);

        if (getAttacks(state, !side) & king) 
            moves ^= ULL1 << i;
        
        __movePiece(state, i, pos, boardType);
        if (wasFigureBefore) {
            state->figures[BOCCUPIED] |= ULL1 << i;
            state->figures[BCOLOR] &= ~(ULL1 << i);
            state->figures[BCOLOR] |= (u64)(!side) << i;
            if (isFigureRestoreNeeded)
                state->figures[boardType] |= ULL1 << i;
        }
    }
    return moves;
}

u64 getPossibleMoves(GameState* state, u8 pos) {
    u64 res = 0;
    u64 occupied = state->figures[BOCCUPIED];
    bool side = (state->figures[BCOLOR] >> pos) & 1;
    u64 allies = occupied & ( side ? state->figures[BCOLOR] : ~state->figures[BCOLOR] );
    int boardType = getFigureBoard(state, pos);

    if (boardType == BOCCUPIED) return 0;
    switch (boardType) {
    case BKING:
        return possibleKingAttacks(state, side) & ~getAttacks(state, !side) & ~allies;
    case BPAWN:
        u64 pawn = ULL1 << pos;
        u64 enemies = occupied & ~allies;
        u64 forward;
        if (side) {
            res = ((pawn * !( (LEFT_EDGE >> pos) & 1 )) << 7 | (pawn * !( (RIGHT_EDGE >> pos) & 1 )) << 9) & enemies;
            forward = (pawn << 8) & ~occupied;
            forward |= ((forward << 8) & ((u64)0xFF << 24)) & ~occupied;
        } else {
            res = (( (pawn * !( (LEFT_EDGE >> pos) & 1 ) ) >> 9) | ( (pawn * !( (RIGHT_EDGE >> pos) & 1 )) >> 7 )) & enemies;
            forward = (pawn >> 8) & ~occupied;
            forward |= ((forward >> 8) & ((u64)0xFF << 32)) & ~occupied;
        }
        res |= forward;
        break;
    case BQUEEN:
        res = (getLinearMovement(occupied, pos) | getDiagonalMovement(occupied, reverseRaws(occupied), pos)) & ~allies;
        break;
    case BROOK:
        res = getLinearMovement(occupied, pos) & ~allies;
        break;
    case BBISHOP:
        res = getDiagonalMovement(occupied, reverseRaws(occupied), pos) & ~allies;
        break;
    case BKNIGHT:
        res = possibleKnightAttacks(state, side) & ~allies;
        break;
    }
    return removeInvalidMoves(state, res, pos, boardType);
}

bool movePiece(GameState* state, u8 pos1, u8 pos2, u8 promotionType) {
    if (!( (state->figures[BOCCUPIED] >> pos1) & 1 ) || ((state->figures[BCOLOR] >> pos1) & 1) != state->turn)
        return false;
    
    u64 possibleMoves = getPossibleMoves(state, pos1);
    printf("Possible:\n");
    printU64(possibleMoves);
    if (!( (possibleMoves >> pos2) & 1)) return false;

    if ((state->figures[BOCCUPIED] >> pos2) & 1) {
        printf("Negating %d\n", pos2);
        for (int i=0; i<BOARD_COUNT; i++) 
            state->figures[i] &= ~(ULL1 << pos2);
    }
    
    printf("FigureType: %d\n", getFigureBoard(state, pos1));

    __movePiece(state, pos1, pos2, getFigureBoard(state, pos1));

    printf("Type: %d\n", getFigureBoard(state, pos2));

    state->turn = !state->turn;
    return true;
}