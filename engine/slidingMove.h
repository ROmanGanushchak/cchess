#ifndef SLIDING_MOVE_H
#define SLIDING_MOVE_H

#include "types.h"

void printU64(u64 a);
void initSlidingPiecesTables();
u64 getLinearMovement(u64 field, u8 pos);
u64 getDiagonalMovement(u64 field, u8 pos);

#endif