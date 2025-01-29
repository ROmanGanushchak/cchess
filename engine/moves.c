#include <stdio.h>
#include "types.h"
#include "moves.h"
#include "slidingMove.h"
#include "figures.h"
#include "engine.h"

u8 getBoardFromPromotion(u8 promotionType) {
    switch(promotionType) {
    case PROMOTING_BISHOP:
        return BBISHOP;
    case PROMOTING_KNIGHT:
        return BKNIGHT;
    case PROMOTING_QUENE:
        return BQUEEN;
    case PROMOTING_ROOK:
        return BROOK;
    }
    return 255;
}

void __removePiece(GameState* state, u8 pos, u8 boardType) {
    state->figures[boardType] &= ~(ULL1 << pos);
    state->figures[BOCCUPIED] &= ~(ULL1 << pos);
    state->figures[BCOLOR] &= ~(ULL1 << pos);
}

void __movePiece(GameState* state, u8 pos1, u8 pos2, u8 boardType) {
    bool side = (state->figures[BCOLOR] >> pos1) & 1;
    state->figures[BCOLOR] &= ~(ULL1 << pos1 | ULL1 << pos2);
    state->figures[BCOLOR] |= (u64)side << pos2;
    state->figures[BOCCUPIED] = ( state->figures[BOCCUPIED] & ~(ULL1 << pos1) ) | ( ULL1 << pos2 );
    state->figures[boardType] = ( state->figures[boardType] & ~(ULL1 << pos1) ) | ( ULL1 << pos2 );
}


/* Is not a complete move, is applicable only for futher check/checkmate checks, if figure captures other figure, both figures will be presented at same cell after that function */
MoveType _movePiece(GameState* state, u8 pos1, u8 pos2, u8 boardType) {
    bool side = (state->figures[BCOLOR] >> pos1) & 1;
    bool isCapture = (state->figures[BOCCUPIED] >> pos2) & 1;
    bool isSameFigureCaptured = (state->figures[boardType] >> pos2) & 1;
    bool isEnPassage = boardType == BPAWN && (pos1 & 7) != (pos2 & 7) && !( (state->figures[BOCCUPIED] >> pos2) & 1 );

    __movePiece(state, pos1, pos2, boardType);

    if (isSameFigureCaptured) return MOVE_SAME_FIGURE_CAPTURED;
    if (isCapture) return MOVE_CAPTURED;
    if (isEnPassage) {
        __removePiece(state, pos2-8*(side*2-1), BPAWN);
        return MOVE_EN_PASSAGE;
    }
    return MOVE_REGULAR;
}


void _undoMovePiece(GameState* state, u8 pos1, u8 pos2, u8 boardType, MoveType type) {
    __movePiece(state, pos2, pos1, boardType);
    bool side = (state->figures[BCOLOR] >> pos1) & 1;

    if (type == MOVE_SAME_FIGURE_CAPTURED || type == MOVE_CAPTURED) {
        state->figures[BCOLOR] &= ~(ULL1 << pos2);
        state->figures[BCOLOR] |= (u64)(!side) << pos2;
        state->figures[BOCCUPIED] |= ULL1 << pos2;
        if (type == MOVE_SAME_FIGURE_CAPTURED)
            state->figures[boardType] |= ULL1 << pos2;
    } else if (type == MOVE_EN_PASSAGE) {
        u8 idx = pos2-8*(side*2-1);
        state->figures[BOCCUPIED] |= ULL1 << idx;
        state->figures[BPAWN] |= ULL1 << idx;
        state->figures[BCOLOR] &= ~(ULL1 << idx);
        state->figures[BCOLOR] |= (u64)(!side) << idx;
    }
}


u64 removeInvalidMoves(GameState* state, u64 moves, u8 pos, u8 boardType) {
    bool side = (state->figures[BCOLOR] >> pos) & 1;
    u64 king = state->figures[BKING] & (side ? state->figures[BCOLOR] : ~state->figures[BCOLOR]);
    for (u8 i=0; i<64; i++) {
        if (!( (moves >> i) & 1 )) continue;
        MoveType type = _movePiece(state, pos, i, boardType);

        if (getAttacks(state, !side) & king) 
            moves ^= ULL1 << i;
        
        _undoMovePiece(state, pos, i, boardType, type);
    }
    return moves;
}


u64 getPossibleCastling(GameState* state, u64 attacked, u8 pos, bool side) {
    if (attacked & (ULL1 << pos)) return 0;
    u64 horizonal = horizontalMovement(state->figures[BOCCUPIED] | attacked, pos);
    u64 res = 0;
    if (side) {
        if (state->doubleCastleWhite && horizonal & (1 << 7))
            res |= 1 << 6;
        if (state->thribleCastleWhite && horizonal & 1)
            res |= 1 << 2;
    } else {
        if (state->doubleCastleBlack && horizonal & (ULL1 << 63))
            res |= ULL1 << 62;
        if (state->thribleCastleBlack && horizonal & (ULL1 << 56))
            res |= ULL1 << 58;
    }
    return res;
}


u64 getPossibleMoves(GameState* state, u8 pos) {
    u64 res = 0;
    u64 occupied = state->figures[BOCCUPIED];
    bool side = (state->figures[BCOLOR] >> pos) & 1;
    u64 allies = occupied & ( side ? state->figures[BCOLOR] : ~state->figures[BCOLOR] );
    u8 boardType = getFigureBoard(state, pos);

    if (boardType == BOCCUPIED) return 0;
    switch (boardType) {
    case BKING:
        u64 attacked = getAttacks(state, !side);
        return (possibleKingAttacks(state, side) | getPossibleCastling(state, attacked, pos, side)) & ~attacked & ~allies;
    case BPAWN:
        u64 pawn = ULL1 << pos;
        u64 enemies = occupied & ~allies;
        u64 forward;
        if (side) {
            res = ((pawn & ~LEFT_EDGE) << 7 | (pawn & ~(LEFT_EDGE << 7)) << 9) & enemies;
            if ((state->lastMove.from >> 3) == 6 && (state->lastMove.to >> 3) == 4 && state->figures[BPAWN] & (ULL1 << state->lastMove.to)) 
                if (abs((int)pos - state->lastMove.to) == 1 && (pos >> 3) == (state->lastMove.to >> 3)) 
                    res |= ULL1 << (state->lastMove.to+8);
            
            forward = (pawn << 8) & ~occupied;
            forward |= ((forward << 8) & ((u64)0xFF << 24)) & ~occupied;
        } else {
            res = ((pawn & ~LEFT_EDGE) >> 9 | (pawn & ~(LEFT_EDGE << 7)) >> 7) & enemies;
            if (state->lastMove.from >> 3 == 1 && state->lastMove.to >> 3 == 3 && state->figures[BPAWN] & (ULL1 << state->lastMove.to)) 
                if (abs(pos - state->lastMove.to) == 1 && pos >> 3 == state->lastMove.to >> 3)
                    res |= ULL1 << (state->lastMove.to-8);
            
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
        res = possibleKnightAttacks(ULL1 << pos, 0, false) & ~allies;
        break;
    }

    return removeInvalidMoves(state, res, pos, boardType);
}


bool movePiece(GameState* state, u8 pos1, u8 pos2, u8 promotionType) {
    if (!( (state->figures[BOCCUPIED] >> pos1) & 1 ) || ((state->figures[BCOLOR] >> pos1) & 1) != state->turn || state->isMate || pos1 == pos2)
        return false;
    bool isMove = true;
    
    u64 possibleMoves = getPossibleMoves(state, pos1);
    if (!( (possibleMoves >> pos2) & 1)) return false;

    u8 boardType = getFigureBoard(state, pos1);
    bool side = (state->figures[BCOLOR] >> pos1) & 1;

    if (boardType == BKING) {
        if (side && pos1 == 4) {
            if (pos2 == 6 && state->doubleCastleWhite) {
                __movePiece(state, 4, 6, BKING);
                __movePiece(state, 7, 5, BROOK);
                isMove = false;
            } else if (pos2 == 2 && state->thribleCastleWhite) {
                __movePiece(state, 4, 2, BKING);
                __movePiece(state, 0, 3, BROOK);
                isMove = false;
            }
        } else if (!side && pos1 == 60) {
            if (pos2 == 62 && state->doubleCastleBlack) {
                __movePiece(state, 60, 62, BKING);
                __movePiece(state, 63, 61, BROOK);
                isMove = false;
            } else if (pos2 == 58 && state->thribleCastleBlack) {
                __movePiece(state, 60, 58, BKING);
                __movePiece(state, 56, 59, BROOK);
                isMove = false;
            }
        }
        if (side) {
            state->doubleCastleWhite = false;
            state->thribleCastleWhite = false;
        } else {
            state->doubleCastleBlack = false;
            state->thribleCastleBlack = false;
        }
    }
    else if (boardType == BROOK) {
        state->thribleCastleWhite &= pos1 != 0;
        state->doubleCastleWhite  &= pos1 != 7;
        state->thribleCastleBlack &= pos1 != 56;
        state->doubleCastleBlack  &= pos1 != 63;
    }
    else if (boardType == BPAWN) {
        if ((side && (pos2 >> 3) == 7) || (!side && (pos2 >> 3) == 0)) {
            u8 newPiece = getBoardFromPromotion(promotionType);
            if (newPiece == 255) return false;
            _movePiece(state, pos1, pos2, BPAWN);
            state->figures[BPAWN] &= ~(ULL1 << pos2);
            state->figures[newPiece] |= ULL1 << pos2;
            isMove = false;
        }
    }
    
    if (isMove) {
        if ((state->figures[BOCCUPIED] >> pos2) & 1) 
            for (u16 i=0; i<BOARD_COUNT; i++) 
                state->figures[i] &= ~(ULL1 << pos2);
        _movePiece(state, pos1, pos2, boardType);
    }

    state->turn = !state->turn;
    state->lastMove.from = pos1;
    state->lastMove.to = pos2;

    if (isCkeckmate(state)) {
        state->isMate = true;
        if (state->turn == 1)
            printf("Black won\n");
        else 
            printf("White won\n");
    }

    return true;
}