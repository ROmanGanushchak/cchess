#include "moves.h"
#include "attacks.h"
#include "slidingMove.h"
#include "unsafeMoves.h"

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

u64 removeInvalidMoves(BoardState* board, u64 moves, u8 pos, u8 boardType) {
    bool side = (board->figures[BCOLOR] >> pos) & 1;
    u64 king = board->figures[BKING] & (side ? board->figures[BCOLOR] : ~board->figures[BCOLOR]);
    for (u8 i=0; i<64; i++) {
        if (!( (moves >> i) & 1 )) continue;
        MOVE_TYPE type = _movePiece(board, pos, i, boardType);

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
        if (board->shortCastleWhite && horizonal & (1 << 7))
            res |= 1 << 6;
        if (board->longCastleWhite && horizonal & 1)
            res |= 1 << 2;
    } else {
        if (board->shortCastleBlack && horizonal & (ULL1 << 63))
            res |= ULL1 << 62;
        if (board->longCastleBlack && horizonal & (ULL1 << 56))
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