#ifndef FIGURES_H
#define FIGURES_H
#include "types.h"
#include "engine.h"
#include <limits.h>

u64 possibleKingAttacks(GameState* state, bool side);
u64 possibleRookAttacks(GameState* state, bool side);
u64 possiblePawnAttacks(GameState* state, bool side);
u64 possibleQueneAttacks(GameState* state, bool side);
u64 possibleBishopAttacks(GameState* state, bool side);
u64 possibleKnightAttacks(GameState* state, bool side);

u64 possibleMovesKing(GameState* state);
u64 possibleMovesQuene(GameState* state);
u64 possibleMovesRook(GameState* state);
u64 possibleMovesBishop(GameState* state);
u64 possibleMovesKnight(GameState* state);
u64 possibleMovesPawns(GameState* state);

#endif