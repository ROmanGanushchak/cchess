#include "engine.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "figures.h"
#include "slidingMove.h"

GameState initialGameState;

void initInitialGameState() {
    initialGameState = (GameState) {.turn = 0, 
        .isCheck=0, .isMate=0, .doubleCastleBlack=1, 
        .doubleCastleWhite=1, .thribleCastleBlack=1, 
        .thribleCastleWhite=1};
    
    initialGameState.figures[BCOLOR] = 0xFFFF;
    initialGameState.figures[BOCCUPIED] = 0xFFFF | ((u64)0xFFFF << 48);
    initialGameState.figures[BKING] = 0x08 | ((u64)0x08 << 56);
    initialGameState.figures[BKNIGHT] = 0x42 | ((u64)0x42 << 56);
    initialGameState.figures[BPAWN] = (0xFF << 8) | ((u64)0xFF << 48);
    initialGameState.figures[BBISHOP] = 0x24 | (0x24 << 56);
    initialGameState.figures[BROOK]   = 0x81 | (0x81 << 56);
    initialGameState.figures[BQUEEN]  = 0x8  | (0x8 << 56);
    // initialGameState.rooks   = 0 | (7 << 8) | (56 << 16) | (63 << 24);
    // initialGameState.bishops = 2 | (5 << 8) | (58 << 16) | (61 << 24);
    // initialGameState.queens  = 3 | ((u16)59 << 8);
}

bool checkIsCheck(GameState* state, bool side) {

}

bool movePiece(GameState* stateP, uint8_t pos1, uint8_t pos2) {
    assert(pos1 < 64 && pos2 < 64);
    GameState state = *stateP;
    if (!((state.figures[BOCCUPIED] >> pos1) & 1)) return false;
    if (!(((state.figures[BCOLOR] >> pos1) & 1) ^ state.turn)) return false;

    // for white for now
    if ((state.figures[BPAWN] >> pos1) & 1) {
        if (pos2 == pos1 + 8);
    }

    if (checkIsCheck(stateP, stateP->turn)) return false;

    
    return true;
}

array_pos getPossibleMoves(GameState* state, uint8_t x, uint8_t y) {
    
}

void startEngine() {
    initInitialGameState();
    initSlidingPiecesTables();
}