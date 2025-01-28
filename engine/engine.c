#include "engine.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "figures.h"
#include "slidingMove.h"

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

bool checkIsCheck(GameState* state, bool side) {

}

/* Just notes
move types:
regular move (taking on the passage)
castling
promoting a pawn
*/
// bool movePiece(GameState* stateP, u8 pos1, u8 pos2) {
//     assert(pos1 < 64 && pos2 < 64);
//     GameState state = *stateP;
//     if (!((state.figures[BOCCUPIED] >> pos1) & 1)) return false;
//     if (!(((state.figures[BCOLOR] >> pos1) & 1) ^ state.turn)) return false;

//     // for white for now
//     if ((state.figures[BPAWN] >> pos1) & 1) {
//         if (pos2 == pos1 + 8);
//     }

//     if (checkIsCheck(stateP, stateP->turn)) return false;
//     return true;
// }

// array_pos getPossibleMoves(GameState* state, uint8_t x, uint8_t y) {
    
// }

void startEngine() {
    initInitialGameState();
    initSlidingPiecesTables();
}

void printBoard(GameState* state) {
    for (int i=7; i>=0; i--) {
        printf("\033[38;5;195m%d | \033[0m", i+1);
        for (int j=0; j<8; j++) {
            int idx = i*8+j;
            if (( ( state->figures[BOCCUPIED] >> idx ) & 1) == 0) {
                printf(". ");
                continue;
            }
            char type = '\0';
            if (( state->figures[BBISHOP] >> idx ) & 1) type = 'b';
            if (( state->figures[BROOK] >> idx ) & 1) type = 'r';
            if (( state->figures[BQUEEN] >> idx ) & 1) type = 'q';
            if (( state->figures[BKNIGHT] >> idx ) & 1) type = 'k';
            if (( state->figures[BKING] >> idx ) & 1) type = 'm';
            if (( state->figures[BPAWN] >> idx ) & 1) type = 'p';
            if (type == '\0') {
                printf("\nINCORRECT BOARD %d\n", idx); 
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
    for (int i=1; i<=7; i++)
        printf("--", i);
    printf("-\n    ");
    for (int i=0; i<8; i++)
        printf("%c ", 'a'+i);
    printf("\033[0m\n");
}

int getFigure(GameState* state, u8 pos) {
    if (( ( state->figures[BOCCUPIED] >> pos ) & 1) == 0) return 0;
    if ((state->figures[BPAWN] >> pos) & 1) return BPAWN;
    if ((state->figures[BBISHOP] >> pos) & 1) return BBISHOP;
    if ((state->figures[BKING] >> pos) & 1) return BKING;
    if ((state->figures[BKNIGHT] >> pos) & 1) return BKNIGHT;
    if ((state->figures[BROOK] >> pos) & 1) return BROOK;
    if ((state->figures[BQUEEN] >> pos) & 1) return BQUEEN;
    return -1;
}