#ifndef FIGURES_H
#define FIGURES_H
#include "types.h"
#include "engine.h"
#include <limits.h>

u64 getAttacks(BoardState* board, bool side);
u64 possibleKnightAttacks(u64 knights, u64 color, bool side);
u64 possiblePawnAttacks(BoardState* board, bool side);
u64 possibleKingAttacks(BoardState* board, bool side);

#endif