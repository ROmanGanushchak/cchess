#include "unsafeMoves.h"
#include "../engine.h"
#include "moves.h"

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
MOVE_TYPE _movePiece(BoardState* board, u8 pos1, u8 pos2, u8 boardType) {
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


void _undoMovePiece(BoardState* board, u8 pos1, u8 pos2, u8 boardType, MOVE_TYPE type) {
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