#include <stdio.h>
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
        for (int i=0; i<BOARD_COUNT; i++)
            board->figures[i] &= 1 << move.to;
    case MOVE_SAME_FIGURE_CAPTURED:
    case MOVE_REGULAR:
        __movePiece(board, move.from, move.to, move.piece);
        break;
    case MOVE_EN_PASSAGE:
        _movePiece(board, move.from, move.to, move.piece);
        break;
    
    case MOVE_CASTLING:
        __movePiece(board, move.from, move.to, BKING);
        if (move.to == 2)
            __movePiece(board, 0, 3, BROOK);
        else if (move.to == 6)
            __movePiece(board, 7, 6, BROOK);
        else if (move.to == 58)
            __movePiece(board, 56, 59, BROOK);
        else if (move.to == 62)
            __movePiece(board, 63, 61, BROOK);
        move.isLongCastleLost = 1;
        move.isShortCastleLost = 1;
        break;
    
    case MOVE_PROMOTION:
        __movePiece(board, move.from, move.to, move.piece);
        board->figures[BPAWN] &= ULL1 << move.to;
        board->figures[getBoardFromPromotion(move.promotion)] |= ULL1 << move.to;
        break;
    }

    bool isCaptureOrAdvance = move.moveType == MOVE_CAPTURED || 
        move.moveType == MOVE_SAME_FIGURE_CAPTURED || 
        move.moveType == MOVE_PROMOTION;
    if (isCaptureOrAdvance)
        board->halfMoves++;
    else board->halfMoves = 0;

    if (board->lastMoveBefore.from == move.to && board->lastMoveBefore.to == move.from) 
        ++board->repeatCount;
    else board->repeatCount = 0;

    if (move.isLongCastleLost) {
        board->thribleCastleBlack &= board->turn;
        board->thribleCastleWhite &= !board->turn;
    }
    if (move.isShortCastleLost) {
        board->doubleCastleBlack &= board->turn;
        board->doubleCastleWhite &= !board->turn;
    }

    board->turn = !board->turn;
    board->lastMoveBefore = board->lastMove;
    board->lastMove.from = move.from;
    board->lastMove.to = move.to;
}

MOVE_RET movePiece(BoardState* board, u8 pos1, u8 pos2, PROMOTION_TYPE promotionType) {
    if (!( (board->figures[BOCCUPIED] >> pos1) & 1 ) || ((board->figures[BCOLOR] >> pos1) & 1) != board->turn || board->isEnd || pos1 == pos2)
        return (MOVE_RET){.isValid=false};
    
    u64 possibleMoves = getPossibleMoves(board, pos1);
    if (!( (possibleMoves >> pos2) & 1)) 
        return (MOVE_RET){.isValid=false};

    Boards boardType = getFigureBoard(board, pos1);
    bool side = (board->figures[BCOLOR] >> pos1) & 1;
    PGN_MOVE move = (PGN_MOVE){.from=pos1, .to=pos2, .piece=boardType, .promotion=promotionType, .moveType=MOVE_REGULAR};

    if (boardType == BKING && abs((pos1 & 7) - (pos2 & 7)) > 1) {
        move.moveType = MOVE_CASTLING;
    } else if (boardType == BROOK) {
        move.isLongCastleLost = (pos1 != 0 && side) || (pos1 != 56 && !side);
        move.isShortCastleLost = (pos1 != 7 && side) || (pos1 != 63 && !side);
    } else if (boardType == BPAWN) {
        if ((side && (pos2 >> 3) == 7) || (!side && (pos2 >> 3) == 0)) 
            move.moveType = MOVE_PROMOTION;
    }

    if ((board->figures[boardType] >> pos2) & 1)
        move.moveType = MOVE_SAME_FIGURE_CAPTURED;
    else if ((board->figures[BOCCUPIED] >> pos2) & 1)
        move.moveType = MOVE_CAPTURED;

    applyMove(board, move);
    
    move.gameState = getGameState(board);
    return (MOVE_RET) {
        .isValid = true,
        .state = move.gameState,
    };
}