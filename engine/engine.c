#include "engine.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "figures.h"
#include "slidingMove.h"
#include "moves.h"

GameState initialGameState;

void initInitialGameState() {
    initialGameState = (GameState) {.turn = 1, 
        .isCheck=0, .isMate=0, .doubleCastleBlack=1, 
        .doubleCastleWhite=1, .thribleCastleBlack=1, 
        .thribleCastleWhite=1};
    
    initialGameState.figures[BCOLOR] = 0xFFFF;
    initialGameState.figures[BOCCUPIED] = 0xFFFF | ((u64)0xFFFF << 48);
    initialGameState.figures[BKING] = 0x10 | ((u64)0x10 << 56);
    initialGameState.figures[BKNIGHT] = 0x42 | ((u64)0x42 << 56);
    initialGameState.figures[BPAWN] = (0xFF << 8) | ((u64)0xFF << 48);
    initialGameState.figures[BBISHOP] = 0x24 | ((u64)0x24 << 56);
    initialGameState.figures[BROOK]   = 0x81 | ((u64)0x81 << 56);
    initialGameState.figures[BQUEEN]  = 0x8  | ((u64)0x8 << 56);
}

void startEngine() {
    initInitialGameState();
    initSlidingPiecesTables();
}

void printBoard(GameState* state) {
    for (i8 i=7; i>=0; i--) {
        printf("\033[38;5;195m%d | \033[0m", i+1);
        for (i8 j=0; j<8; j++) {
            u8 idx = i*8+j;
            if (( ( state->figures[BOCCUPIED] >> idx ) & 1) == 0) {
                printf(". ");
                continue;
            }
            u8 cnt = 0;
            char type = '\0';
            if (( state->figures[BBISHOP] >> idx ) & 1) {type = 'b'; cnt++;}
            if (( state->figures[BROOK] >> idx ) & 1) {type = 'r'; cnt++;}
            if (( state->figures[BQUEEN] >> idx ) & 1) {type = 'q'; cnt++;}
            if (( state->figures[BKNIGHT] >> idx ) & 1) {type = 'k'; cnt++;}
            if (( state->figures[BKING] >> idx ) & 1) {type = 'm'; cnt++;}
            if (( state->figures[BPAWN] >> idx ) & 1) {type = 'p'; cnt++;}
            if (cnt != 1) {
                printf("\nINCORRECT BOARD %d, cnt: %d\n", idx, cnt); 
                return;
            }
            if (( state->figures[BCOLOR] >> idx ) & 1) 
                printf("\033[38;5;214m%c\033[0m ", type + 'A' - 'a');
            else 
                printf("\033[38;5;50m%c\033[0m ", type);
        }
        printf("\n");
    }
    printf("    \033[38;5;195m");
    for (u8 i=1; i<=7; i++)
        printf("--", i);
    printf("-\n    ");
    for (u8 i=0; i<8; i++)
        printf("%c ", 'a'+i);
    printf("\033[0m\n");
}

u8 getFigureBoard(GameState* state, u8 pos) {
    if (state->figures[BPAWN] & (ULL1 << pos)) return BPAWN;
    if (state->figures[BBISHOP] & (ULL1 << pos)) return BBISHOP;
    if (state->figures[BQUEEN] & (ULL1 << pos)) return BQUEEN;
    if (state->figures[BROOK] & (ULL1 << pos)) return BROOK;
    if (state->figures[BKNIGHT] & (ULL1 << pos)) return BKNIGHT;
    if (state->figures[BKING] & (ULL1 << pos)) return BKING;
    return BOCCUPIED;
}

bool isCkeckmate(GameState* state) {
    bool side = state->turn;
    u64 attacked = getAttacks(state, !side);
    u64 allies = state->figures[BOCCUPIED] & (side ? state->figures[BCOLOR] : ~state->figures[BCOLOR]);
    u64 king = state->figures[BKING] & (side ? state->figures[BCOLOR] : ~state->figures[BCOLOR]);
    if (!( king & attacked && (possibleKingAttacks(state, side) & ~attacked & ~allies) == 0 ))
        return false;

    for (u8 i=0; i<64; i++) 
        if (( allies >> i ) & 1 && getPossibleMoves(state, i)) 
            return false;
    return true;
}

void printU64(u64 a) {
    for (i8 i=7; i>=0; i--) {
        for (u8 j=0; j<8; j++) {
            printf("%d", (a >> (i*8+j)) & 1, i*8+j);
        }printf("\n");
    }printf("\n");
}