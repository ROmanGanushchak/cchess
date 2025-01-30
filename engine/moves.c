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

void __removePiece(BoardState* board, u8 pos, u8 boardType) {
    board->figures[boardType] &= ~(ULL1 << pos);
    board->figures[BOCCUPIED] &= ~(ULL1 << pos);
    board->figures[BCOLOR] &= ~(ULL1 << pos);
}

void __movePiece(BoardState* board, u8 pos1, u8 pos2, u8 boardType) {
    bool side = (board->figures[BCOLOR] >> pos1) & 1;
    board->figures[BCOLOR] &= ~(ULL1 << pos1 | ULL1 << pos2);
    board->figures[BCOLOR] |= (u64)side << pos2;
    board->figures[BOCCUPIED] = ( board->figures[BOCCUPIED] & ~(ULL1 << pos1) ) | ( ULL1 << pos2 );
    board->figures[boardType] = ( board->figures[boardType] & ~(ULL1 << pos1) ) | ( ULL1 << pos2 );
}


/* Is not a complete move, is applicable only for futher check/checkmate checks, if figure captures other figure, both figures will be presented at same cell after that function */
MoveType _movePiece(BoardState* board, u8 pos1, u8 pos2, u8 boardType) {
    bool side = (board->figures[BCOLOR] >> pos1) & 1;
    bool isCapture = (board->figures[BOCCUPIED] >> pos2) & 1;
    bool isSameFigureCaptured = (board->figures[boardType] >> pos2) & 1;
    bool isEnPassage = boardType == BPAWN && (pos1 & 7) != (pos2 & 7) && !( (board->figures[BOCCUPIED] >> pos2) & 1 );

    __movePiece(board, pos1, pos2, boardType);

    if (isSameFigureCaptured) return MOVE_SAME_FIGURE_CAPTURED;
    if (isCapture) return MOVE_CAPTURED;
    if (isEnPassage) {
        __removePiece(board, pos2-8*(side*2-1), BPAWN);
        return MOVE_EN_PASSAGE;
    }
    return MOVE_REGULAR;
}


void _undoMovePiece(BoardState* board, u8 pos1, u8 pos2, u8 boardType, MoveType type) {
    __movePiece(board, pos2, pos1, boardType);
    bool side = (board->figures[BCOLOR] >> pos1) & 1;

    if (type == MOVE_SAME_FIGURE_CAPTURED || type == MOVE_CAPTURED) {
        board->figures[BCOLOR] &= ~(ULL1 << pos2);
        board->figures[BCOLOR] |= (u64)(!side) << pos2;
        board->figures[BOCCUPIED] |= ULL1 << pos2;
        if (type == MOVE_SAME_FIGURE_CAPTURED)
            board->figures[boardType] |= ULL1 << pos2;
    } else if (type == MOVE_EN_PASSAGE) {
        u8 idx = pos2-8*(side*2-1);
        board->figures[BOCCUPIED] |= ULL1 << idx;
        board->figures[BPAWN] |= ULL1 << idx;
        board->figures[BCOLOR] &= ~(ULL1 << idx);
        board->figures[BCOLOR] |= (u64)(!side) << idx;
    }
}


u64 removeInvalidMoves(BoardState* board, u64 moves, u8 pos, u8 boardType) {
    bool side = (board->figures[BCOLOR] >> pos) & 1;
    u64 king = board->figures[BKING] & (side ? board->figures[BCOLOR] : ~board->figures[BCOLOR]);
    for (u8 i=0; i<64; i++) {
        if (!( (moves >> i) & 1 )) continue;
        MoveType type = _movePiece(board, pos, i, boardType);

        if (getAttacks(board, !side) & king) 
            moves ^= ULL1 << i;
        
        _undoMovePiece(board, pos, i, boardType, type);
    }
    return moves;
}


u64 getPossibleCastling(BoardState* board, u64 attacked, u8 pos, bool side) {
    if (attacked & (ULL1 << pos)) return 0;
    u64 horizonal = horizontalMovement(board->figures[BOCCUPIED] | attacked, pos);
    u64 res = 0;
    if (side) {
        if (board->doubleCastleWhite && horizonal & (1 << 7))
            res |= 1 << 6;
        if (board->thribleCastleWhite && horizonal & 1)
            res |= 1 << 2;
    } else {
        if (board->doubleCastleBlack && horizonal & (ULL1 << 63))
            res |= ULL1 << 62;
        if (board->thribleCastleBlack && horizonal & (ULL1 << 56))
            res |= ULL1 << 58;
    }
    return res;
}


u64 getPossibleMoves(BoardState* board, u8 pos) {
    u64 res = 0;
    u64 occupied = board->figures[BOCCUPIED];
    bool side = (board->figures[BCOLOR] >> pos) & 1;
    u64 allies = occupied & ( side ? board->figures[BCOLOR] : ~board->figures[BCOLOR] );
    u8 boardType = getFigureBoard(board, pos);

    if (boardType == BOCCUPIED) return 0;
    switch (boardType) {
    case BKING:
        u64 attacked = getAttacks(board, !side);
        return (possibleKingAttacks(board, side) | getPossibleCastling(board, attacked, pos, side)) & ~attacked & ~allies;
    case BPAWN:
        u64 pawn = ULL1 << pos;
        u64 enemies = occupied & ~allies;
        u64 forward;
        if (side) {
            res = ((pawn & ~LEFT_EDGE) << 7 | (pawn & ~(LEFT_EDGE << 7)) << 9) & enemies;
            if ((board->lastMove.from >> 3) == 6 && (board->lastMove.to >> 3) == 4 && board->figures[BPAWN] & (ULL1 << board->lastMove.to)) 
                if (abs((int)pos - board->lastMove.to) == 1 && (pos >> 3) == (board->lastMove.to >> 3)) 
                    res |= ULL1 << (board->lastMove.to+8);
            
            forward = (pawn << 8) & ~occupied;
            forward |= ((forward << 8) & ((u64)0xFF << 24)) & ~occupied;
        } else {
            res = ((pawn & ~LEFT_EDGE) >> 9 | (pawn & ~(LEFT_EDGE << 7)) >> 7) & enemies;
            if (board->lastMove.from >> 3 == 1 && board->lastMove.to >> 3 == 3 && board->figures[BPAWN] & (ULL1 << board->lastMove.to)) 
                if (abs(pos - board->lastMove.to) == 1 && pos >> 3 == board->lastMove.to >> 3)
                    res |= ULL1 << (board->lastMove.to-8);
            
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

    return removeInvalidMoves(board, res, pos, boardType);
}

bool tryCastle(BoardState* board, u8 pos1, u8 pos2, bool side) {
    bool isCastle = false;
    if (side && pos1 == 4) {
        if (pos2 == 6 && board->doubleCastleWhite) {
            __movePiece(board, 4, 6, BKING);
            __movePiece(board, 7, 5, BROOK);
            isCastle = true;
        } else if (pos2 == 2 && board->thribleCastleWhite) {
            __movePiece(board, 4, 2, BKING);
            __movePiece(board, 0, 3, BROOK);
            isCastle = true;
        }
    } else if (!side && pos1 == 60) {
        if (pos2 == 62 && board->doubleCastleBlack) {
            __movePiece(board, 60, 62, BKING);
            __movePiece(board, 63, 61, BROOK);
            isCastle = true;
        } else if (pos2 == 58 && board->thribleCastleBlack) {
            __movePiece(board, 60, 58, BKING);
            __movePiece(board, 56, 59, BROOK);
            isCastle = true;
        }
    }

    // if king moved or castled unable futher castling
    if (side) {
        board->doubleCastleWhite = false;
        board->thribleCastleWhite = false;
    } else {
        board->doubleCastleBlack = false;
        board->thribleCastleBlack = false;
    }
    return isCastle;
}

MOVE_RET movePiece(BoardState* board, u8 pos1, u8 pos2, u8 promotionType) {
    if (!( (board->figures[BOCCUPIED] >> pos1) & 1 ) || ((board->figures[BCOLOR] >> pos1) & 1) != board->turn || board->isEnd || pos1 == pos2)
        return (MOVE_RET){.isValid=false};
    bool isMove = true;
    bool isCaptureOrPawnAdvance = false;
    
    u64 possibleMoves = getPossibleMoves(board, pos1);
    if (!( (possibleMoves >> pos2) & 1)) 
        return (MOVE_RET){.isValid=false};

    u8 boardType = getFigureBoard(board, pos1);
    bool side = (board->figures[BCOLOR] >> pos1) & 1;

    if (boardType == BKING) {
        if (tryCastle(board, pos1, pos2, side))
            isMove = false;
    } else if (boardType == BROOK) {
        board->thribleCastleWhite &= pos1 != 0;
        board->doubleCastleWhite  &= pos1 != 7;
        board->thribleCastleBlack &= pos1 != 56;
        board->doubleCastleBlack  &= pos1 != 63;
    } else if (boardType == BPAWN) {
        if ((side && (pos2 >> 3) == 7) || (!side && (pos2 >> 3) == 0)) {
            u8 newPiece = getBoardFromPromotion(promotionType);
            if (newPiece == 255) return (MOVE_RET){.isValid=false};
            _movePiece(board, pos1, pos2, BPAWN);
            board->figures[BPAWN] &= ~(ULL1 << pos2);
            board->figures[newPiece] |= ULL1 << pos2;
            isMove = false;
            isCaptureOrPawnAdvance = true;
        }
    }
    
    if (isMove) {
        if ((board->figures[BOCCUPIED] >> pos2) & 1) {
            isCaptureOrPawnAdvance = true;
            for (u16 i=0; i<BOARD_COUNT; i++) 
                board->figures[i] &= ~(ULL1 << pos2);
        }
        MoveType type = _movePiece(board, pos1, pos2, boardType);
        isCaptureOrPawnAdvance |= type == MOVE_EN_PASSAGE;
    }

    return (MOVE_RET){
        .isValid=true, 
        .state=updateGameState(board, pos1, pos2, isCaptureOrPawnAdvance)
    };
}