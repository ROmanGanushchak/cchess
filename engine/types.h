#ifndef TYPES_H
#define TYPES_H

#define i8 char
#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define i32 int
#define u64 unsigned long long

#define ULL1 ((u64)1)
#define LEFT_EDGE (u64)0x0101010101010101
#define BOTTOM_ROW (u64)0xFF
#define RIGHT_EDGE LEFT_EDGE >> 7

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))

int abs(int a);

typedef struct {
    u32 len;
    u32 cap;
    char* arr;
} STR;

#define DEFINE_PAIR(T, R, Name) \
typedef struct { \
    T first; \
    R second; \
} Name;

#endif