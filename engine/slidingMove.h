#ifndef SLIDING_MOVE_H
#define SLIDING_MOVE_H

#include "types.h"

extern u8 reverseU8Map[256];

/* modules init method, has to be called once before the module usage */
void initSlidingPiecesTables();

void printU64(u64 a);
u64 getLinearMovement(u64 field, u8 pos);
u64 horizontalMovement(u64 field, u8 pos);
u64 getDiagonalMovement(u64 field, u64 reversed, u8 pos);
u64 reverseRaws(u64 field);

#endif