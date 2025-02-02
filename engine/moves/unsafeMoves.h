#ifndef UNSAFE_MOVES_H
#define UNSAFE_MOVES_H

#include "../types.h"
#include "moves.h"

void __removePiece(BoardState* board, u8 pos, u8 boardType);
void __spawnPiece(BoardState*, u8 pos, u8 boardType, bool side);
void __movePiece(BoardState* board, u8 pos1, u8 pos2, u8 boardType);

/* Is not a complete move, is applicable only for futher check/checkmate checks, if figure captures other figure, both figures will be presented at same cell after that function */
MOVE_TYPE _movePiece(BoardState* board, u8 pos1, u8 pos2, u8 boardType);
void _undoMovePiece(BoardState* board, u8 pos1, u8 pos2, u8 boardType, MOVE_TYPE type);

#endif