#include <stdio.h>
#include "types.h"

u64 diagonalMask[15], antidiagonalMask[15];

void printU64(u64 a) {
    for (int i=7; i>=0; i--) {
        for (int j=0; j<8; j++) {
            printf("%d", (a >> (i*8+j)) & 1, i*8+j);
        }printf("\n");
    }
}

u64 reverseU64(u64 a) {
    u64 b = 0;
    for (int i=0; i<64; i++) 
        if ((a >> i) & 1) 
            b |= ULL1 << (63-i);
    return b;
}

void initSlidingPiecesTables() {
    int i, j, num;
    for (i=0; i<13; i++) {
        antidiagonalMask[i] = 0; 
        diagonalMask[i] = 0;
    }
    
    // anti diagonals
    for (i=0; i<8; i++) {
        num = (7-i) << 3;
        while (num < 64) {
            antidiagonalMask[i] |= ULL1 << num;
            num += 9;
        }
    }
    for (j=1; j<8; j++, i++) {
        num = j;
        while (num < 64 - (j << 3)) {
            antidiagonalMask[i] |= ULL1 << num;
            num += 9;
        }
    }

    // main diagonals
    for (i=0; i<8; i++) {
        num = i << 3;
        while (num >= 0) {
            diagonalMask[i] |= ULL1 << num;
            num -= 7;
        }
    }
    for (j=1; j<8; j++, i++) {
        num = 56 + j;
        while (num > j*8) {
            diagonalMask[i] |= ULL1 << num;
            num -= 7;
        }
    }
}

u64 horizontalMovement(u64 field, u8 pos) {
    return ( (field - (ULL1 << (pos+1))) ^ reverseU64(reverseU64(field) - (ULL1 << (64-pos))) ) & (0xFF << ( (pos >> 3) << 3));
}

u64 verticalMovement(u64 field, u8 pos) {
    u64 mask = LEFT_EDGE << (pos & 7);
    u64 f = field & mask;
    return ( ( f - (ULL1 << (pos+1)) ) ^ ( reverseU64( reverseU64(f) - (ULL1 << (64 - pos)) )) ) & mask;
}

u64 getLinearMovement(u64 field, u8 pos) {
    return horizontalMovement(field, pos) | verticalMovement(field, pos);
}

u8 getDiagonalNumber(u8 pos) {
    return (pos & 7) + (pos >> 3);
}

u8 getAntiDiagonalNumber(u8 pos) {
    return (pos & 7) - (pos >> 3) + 7;
}

u64 getDiagonalMovement(u64 field, u8 pos) {
    u64 mask = antidiagonalMask[getAntiDiagonalNumber(pos)] | diagonalMask[getDiagonalNumber(pos)];
    field = field & mask;
    return ( ( field - (ULL1 << (pos+1)) ) ^ ( reverseU64( reverseU64(field) - (ULL1 << (64 - pos)) )) ) & mask;
}