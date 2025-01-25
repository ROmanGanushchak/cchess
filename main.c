#include <stdio.h>
#include <string.h>
#include "engine.h"
#include "figures.h"
#include "slidingMove.h"

int main() {
    startEngine();
    GameState state = initialGameState;
    GameState cust;
    memset(&cust, 0, sizeof(cust));
    cust.figures[BQUEEN] |= 1 << 28;
    cust.figures[BPAWN] |= 1 << 19 | 1 << 20 | ULL1 << 42;
    cust.figures[BCOLOR] |= 1 << 28;
    cust.figures[BOCCUPIED] |= cust.figures[BQUEEN] | cust.figures[BPAWN];

    printU64(getAttacks(&cust, 0));
    return 0;
}