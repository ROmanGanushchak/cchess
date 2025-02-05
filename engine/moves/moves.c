#include <stdio.h>
#include <assert.h>
#include "../pgn.h"
#include "../engine.h"
#include "../types.h"

#include "moves.h"
#include "slidingMove.h"
#include "attacks.h"
#include "unsafeMoves.h"

void applyMove(BoardState* board, PGN_MOVE move) {
    switch (move.moveType) {
    case MOVE_CAPTURED:
        board->figures[move.captured] &= ~(ULL1 << move.to);
    case MOVE_SAME_FIGURE_CAPTURED:
    case MOVE_REGULAR:
        __movePiece(board, move.from, move.to, move.piece);
        break;
    
    case MOVE_EN_PASSAGE:
        assert(move.piece == BPAWN);
        __movePiece(board, move.from, move.to, BPAWN);
        __removePiece(board, board->lastMove.to, BPAWN);
        break;
    
    case MOVE_CASTLING:
        assert(move.piece == BKING);
        __movePiece(board, move.from, move.to, BKING);
        if (move.to == 2)
            __movePiece(board, 0, 3, BROOK);
        else if (move.to == 6)
            __movePiece(board, 7, 5, BROOK);
        else if (move.to == 58)
            __movePiece(board, 56, 59, BROOK);
        else if (move.to == 62)
            __movePiece(board, 63, 61, BROOK);
        break;
    
    case MOVE_PROMOTION:
        assert(move.piece == BPAWN);
        if (move.captured != BOCCUPIED)
            __removePiece(board, move.to, move.captured);
        __movePiece(board, move.from, move.to, move.piece);
        board->figures[BPAWN] &= ~(ULL1 << move.to);
        board->figures[getBoardFromPromotion(move.promotion)] |= ULL1 << move.to;
        break;
    }

    if (move.isLongCastleGone) {
        board->longCastleBlack &= board->turn;
        board->longCastleWhite &= !board->turn;
    }
    if (move.isShortCastleGone) {
        board->shortCastleBlack &= board->turn;
        board->shortCastleWhite &= !board->turn;
    }

    board->turn = !board->turn;
    board->lastMoveBefore = board->lastMove;
    board->lastMove.from = move.from;
    board->lastMove.to = move.to;
    board->halfMoves = move.halfMoves;
    board->repeatCount = move.repeatCount;
}

void invertMove(BoardState* board, PGN* pgn, u32 idx) {
    PGN_MOVE move = pgn->moves[idx];
    board->turn = !board->turn;
    board->lastMove = board->lastMoveBefore;
    if (idx >= 2)
        board->lastMoveBefore = (MOVE){.from=pgn->moves[idx-2].from, .to=pgn->moves[idx-2].to};
    else board->lastMoveBefore = (MOVE){.from=0, .to=0};
    board->halfMoves = move.halfMoves;
    board->repeatCount = move.repeatCount;

    if (board->turn) {
        board->longCastleWhite |= move.isLongCastleGone;
        board->shortCastleWhite |= move.isShortCastleGone;
    } else {
        board->longCastleBlack |= move.isLongCastleGone;
        board->shortCastleBlack |= move.isShortCastleGone;
    }

    switch (move.moveType) {
    case MOVE_REGULAR:
        __movePiece(board, move.to, move.from, move.piece);
        break;
    
    case MOVE_SAME_FIGURE_CAPTURED:
        __movePiece(board, move.to, move.from, move.piece);
        __spawnPiece(board, move.to, move.piece, !board->turn);
        break;
    
    case MOVE_CAPTURED:
        __movePiece(board, move.to, move.from, move.piece);
        __spawnPiece(board, move.to, move.captured, !board->turn);
        break;
    
    case MOVE_CASTLING:
        assert(move.piece == BKING);
        __movePiece(board, move.to, move.from, BKING);
        if (move.to == 2)
            __movePiece(board, 3, 0, BROOK);
        else if (move.to == 6)
            __movePiece(board, 5, 7, BROOK);
        else if (move.to == 58)
            __movePiece(board, 59, 56, BROOK);
        else if (move.to == 62)
            __movePiece(board, 61, 63, BROOK);
        break;
    
    case MOVE_PROMOTION:
        assert(move.piece == BPAWN);
        board->figures[getBoardFromPromotion(move.promotion)] &= ~(ULL1 << move.to);
        board->figures[BPAWN] |= ULL1 << move.to;
        __movePiece(board, move.to, move.from, BPAWN);
        if (move.captured != BOCCUPIED)
            __spawnPiece(board, move.to, move.captured, !board->turn);
        break;
    
    case MOVE_EN_PASSAGE:
        assert(move.piece == BPAWN);
        __movePiece(board, move.to, move.from, BPAWN);
        __spawnPiece(board, board->lastMove.to, BPAWN, !board->turn);
        break;
    }
}

PGN_MOVE applyMoveWithCheck(BoardState* board, u8 from, u8 to, PROMOTION_TYPE promotionType) {
    if (!( (board->figures[BOCCUPIED] >> from) & 1 ) || ((board->figures[BCOLOR] >> from) & 1) != board->turn || board->isEnd || from == to)
        return (PGN_MOVE){.isValidated=false};
    
    Boards boardType = getFigureBoard(board, from);
    bool side = (board->figures[BCOLOR] >> from) & 1;
    u64 candidateMoves = getCandidateMoves(board, boardType, from, side, 0);
    if (!( (candidateMoves >> to) & 1)) 
        return (PGN_MOVE){.isValidated=false};
    
    if (boardType != BKING) {
        u64 king = board->figures[BKING] & (side ? board->figures[BCOLOR] : ~board->figures[BCOLOR]);
        MOVE_TYPE _type = _movePiece(board, from, to, boardType);
        bool isCheckAfterMove = ( getAttacks(board, !side) & king ) != 0;
        _undoMovePiece(board, from, to, boardType, _type);

        if (isCheckAfterMove) return (PGN_MOVE){.isValidated=false};
    }

    PGN_MOVE move = (PGN_MOVE){.from=from, .to=to, 
        .piece=boardType, .promotion=promotionType, 
        .moveType=MOVE_REGULAR, .isValidated=false,
        .captured=BOCCUPIED};

    if ((board->figures[boardType] >> to) & 1) {
        move.moveType = MOVE_SAME_FIGURE_CAPTURED;
        move.captured = boardType;
    } else if ((board->figures[BOCCUPIED] >> to) & 1) {
        move.moveType = MOVE_CAPTURED;
        move.captured = getFigureBoard(board, to);
        assert(move.captured != BOCCUPIED);
    }

    if (boardType == BKING) {
        if (abs((from & 7) - (to & 7)) > 1) 
            move.moveType = MOVE_CASTLING;
        move.isLongCastleGone = (board->turn && board->longCastleWhite) || (!board->turn && board->longCastleBlack);
        move.isShortCastleGone = (board->turn && board->shortCastleWhite) || (!board->turn && board->shortCastleBlack);
    } else if (boardType == BROOK) {
        if (side) {
            move.isLongCastleGone = from == 0 && board->longCastleWhite;
            move.isShortCastleGone= from == 7 && board->shortCastleWhite;
        } else {
            move.isLongCastleGone = from == 56 && board->longCastleBlack;
            move.isShortCastleGone= from == 63 && board->shortCastleBlack;
        }
    } else if (boardType == BPAWN) {
        if ((side && (to >> 3) == 7) || (!side && (to >> 3) == 0)) {
            if (promotionType == PROMOTING_NONE)
                return move;
            move.moveType = MOVE_PROMOTION;
        }
        else if ((from & 7) != (to & 7) && !((board->figures[BOCCUPIED] >> to) & 1))
            move.moveType = MOVE_EN_PASSAGE;
    }
    
    bool isCaptureOrAdvance = move.moveType == MOVE_CAPTURED || 
        move.moveType == MOVE_SAME_FIGURE_CAPTURED || 
        move.moveType == MOVE_PROMOTION;
    if (isCaptureOrAdvance)
        move.halfMoves = board->halfMoves + 1;
    else move.halfMoves = 0;

    if (board->lastMoveBefore.from == move.to && board->lastMoveBefore.to == move.from) 
        move.repeatCount = board->repeatCount + 1;
    else move.repeatCount = 0;

    applyMove(board, move);
    
    move.gameState = getGameState(board);
    move.isValidated = true;
    return move;
}

bool makeMove(BoardState* board, PGN* pgn, u8 from, u8 to, PROMOTION_TYPE type) {
    PGN_MOVE move = applyMoveWithCheck(board, from, to, type);
    if (!move.isValidated) return false;

    if (pgn->idx + 1 != pgn->len)
        pgn->len = pgn->idx + 1;
    addMove(pgn, move);
    pgn->idx = pgn->len-1;
    return true;
}

bool makeMoveBack(BoardState* board, PGN* pgn) {
    if (pgn->idx < 0) return false;
    invertMove(board, pgn, pgn->idx--);
    return true;
}
bool makeMoveForward(BoardState* board, PGN* pgn) {
    if (pgn->idx + 1 >= pgn->len) return false;
    printf("IDX: %d\n", pgn->idx);
    applyMove(board, pgn->moves[++pgn->idx]);
    return true;
}