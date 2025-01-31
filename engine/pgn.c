#include "pgn.h"
#include "moves.h"
#include "unsafeMoves.h"

/*typedef enum {
    MOVE_REGULAR,
    MOVE_CAPTURED,
    MOVE_SAME_FIGURE_CAPTURED,
    MOVE_EN_PASSAGE,
    MOVE_CASTLING,
    MOVE_PROMOTION
} MOVE_TYPE;*/

void applyMove(BoardState* board, PGN_MOVE move) {
    bool isCaptureOrAdvance = move.moveType == MOVE_CAPTURED || 
        move.moveType == MOVE_SAME_FIGURE_CAPTURED || 
        move.moveType == MOVE_PROMOTION;
    
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
        break;
    
    case MOVE_PROMOTION:
        __movePiece(board, move.from, move.to, move.piece);
        board->figures[BPAWN] &= ULL1 << move.to;
        board->figures[getBoardFromPromotion(move.promotion)] |= ULL1 << move.to;
        break;
    }

    board->turn = !board->turn;
    board->lastMoveBefore = board->lastMove;
    board->lastMove.from = move.from;
    board->lastMove.to = move.to;
}