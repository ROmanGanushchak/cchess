#include <stdio.h>
#include "engine.h"
#include "figures.h"
#include "slidingMove.h"

int main() {
    startEngine();
    GameState state = initialGameState;
    printf("%d, %d\n", state.queens >> 8, state.queens & 0xFF);
    printU64(possibleRookAttacks(&state, 1));
    printf("GameStateSize: %d\n", sizeof(GameState));
    return 0;
}