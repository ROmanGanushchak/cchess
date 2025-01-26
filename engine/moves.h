#ifndef MOVES_H
#define MOVES_H

#include "types.h"
#include "engine.h"

u64 getPossibleMoves(GameState* state, u8 pos);
bool movePiece(GameState* state, u8 pos1, u8 pos2, u8 promotionType);

#endif