#ifndef FIGURES_H
#define FIGURES_H
#include <limits.h>
#include "../types.h"
#include "../engine.h"

u64 getAttacks(BoardState* board, bool side);
u64 possibleKnightAttacks(u64 knights, u64 color, bool side);
u64 possiblePawnAttacks(BoardState* board, bool side);
u64 _possibleKingAttacks(u64 kings);
u64 possibleKingAttacks(BoardState* board, bool side);

#endif