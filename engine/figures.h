#ifndef FIGURES_H
#define FIGURES_H
#include "types.h"
#include "engine.h"
#include <limits.h>

u64 getAttacks(GameState* state, bool side);
u64 possibleKnightAttacks(GameState* state, bool side);
u64 possiblePawnAttacks(GameState* state, bool side);
u64 possibleKingAttacks(GameState* state, bool side);

#endif