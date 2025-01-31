#include <stdio.h>
#include "../types.h"

u64 diagonalMask[15];
u8 reverseU8Map[256];
void initReverseU8Map() {
    for (u16 i=0; i<256; i++) {
        u8 reversed = 0;
        for (u8 j=0; j<8; j++) 
            if ((i >> j) & 1) 
                reversed |= 1 << (7-j);
        reverseU8Map[i] = reversed;
    }
}

u64 reverseU64(u64 a) {
    u64 b = 0;
    for (u8 i=0; i<64; i += 8) {
        b = (b << 8) | reverseU8Map[a & 0xFF];
        a >>= 8;
    }
    return b;
}

void initSlidingPiecesTables() {
    initReverseU8Map();

    int i, j, num;
    for (i=0; i<13; i++) 
        diagonalMask[i] = 0;

    // main diagonals
    for (i=0; i<8; i++) {
        num = i << 3;
        while (num >= 0) {
            diagonalMask[i] |= ULL1 << num;
            num -= 7;
        }
    }
    diagonalMask[7] ^= 1;

    for (j=1; j<8; j++, i++) {
        num = 56 + j;
        while (num > j*8) {
            diagonalMask[i] |= ULL1 << num;
            num -= 7;
        }
    }
}

u64 horizontalMovement(u64 field, u8 pos) {
    u64 upper = ( ULL1 << (pos+1) ) * (pos != 63);
    u64 lower = ( ULL1 << (64 - pos) ) * (pos != 0);
    return ( ( field - upper ) ^ reverseU64( reverseU64(field) - lower ) ) & ((u64)0xFF << ( (pos >> 3) << 3));
}

u64 verticalMovement(u64 field, u8 pos) {
    u64 mask = LEFT_EDGE << (pos & 7);
    field &= mask;
    u64 upper = ( ULL1 << (pos+1) ) * (pos != 63);
    u64 lower = ( ULL1 << (64 - pos) ) * (pos != 0);
    return ( ( field - upper ) ^ ( reverseU64( reverseU64(field) - lower )) ) & mask;
}

u64 getLinearMovement(u64 field, u8 pos) {
    return horizontalMovement(field, pos) | verticalMovement(field, pos);
}

u8 getDiagonalNumber(u8 pos) {
    return (pos & 7) + (pos >> 3);
}

u64 fieldAttackFormula(u64 field, u64 pos) {
    u64 upper = ( ULL1 << (pos+1) ) * (pos != 63);
    u64 lower = ( ULL1 << (64 - pos) ) * (pos != 0);
    return ( ( field - upper ) ^ ( reverseU64( reverseU64(field) - lower )) );
}

u64 reverseRaws(u64 field) {
    u64 reverseRowField = 0;
    for (u8 i=0; i<64; i+=8) 
        reverseRowField |= (u64)reverseU8Map[(field >> i) & 0xFF] << i;
    return reverseRowField;
}

u64 getDiagonalMovement(u64 field, u64 reversed, u8 pos) {
    u8 reversedRowPos = (7 - (pos & 7)) | (pos & ~7);
    u64 mask1 = diagonalMask[getDiagonalNumber(pos)];
    u64 mask2 = diagonalMask[getDiagonalNumber(reversedRowPos)];
    reversed = fieldAttackFormula(reversed & mask2, reversedRowPos) & mask2;

    u64 antiDiagonalMoves = 0;
    for (i8 i=0; i<64; i+=8) 
        antiDiagonalMoves |= (u64)reverseU8Map[(reversed >> i) & 0xFF] << i;
    return ( fieldAttackFormula(field & mask1, pos) & mask1 ) | antiDiagonalMoves ;
}