#include <stdio.h>
#include <string.h>
#include "engine.h"
#include "figures.h"
#include "slidingMove.h"
#include "moves.h"

int main() {
    startEngine();
    // GameState state = initialGameState;
    // GameState cust;
    // memset(&cust, 0, sizeof(cust));
    // cust.turn = 1;
    // cust.figures[BQUEEN] |= ULL1 << 35;
    // cust.figures[BPAWN] |= 1 << 19 | 1 << 20 | ULL1 << 42;
    // cust.figures[BROOK] |= ULL1 << 16;
    // cust.figures[BBISHOP] |= ULL1 << 10;
    // cust.figures[BKING] |= ULL1 << 3;
    // cust.figures[BCOLOR] |= cust.figures[BQUEEN] | cust.figures[BBISHOP];
    // cust.figures[BOCCUPIED] |= cust.figures[BQUEEN] | cust.figures[BPAWN] | cust.figures[BROOK] | cust.figures[BBISHOP] | cust.figures[BKING];

    // printBoard(&cust);
    // printf("IsPossible: %d\n", movePiece(&cust, 35, 34, 0));
    // printf("IsPossible: %d\n", movePiece(&cust, 3, 11, 0));
    // printf("IsPossible: %d\n", movePiece(&cust, 11, 12, 0));
    // printf("IsPossible: %d\n", movePiece(&cust, 12, 13, 0));
    // printf("IsPossible: %d\n", movePiece(&cust, 13, 21, 0));
    // printf("IsPossible: %d\n", movePiece(&cust, 20, 12, 0));
    // printf("\n");
    // printU64(getPossibleMoves(&cust, 16));
    // printf("\n");

    // printBoard(&cust);

    GameState board = initialGameState;
    printBoard(&board);
    char buffer[256];
    while (true) {
        scanf("%s %s %s", buffer, buffer+3, buffer+6);
        if (buffer[0] == 'm' && buffer[1] == 'v') {
            u8 pos1 = ((buffer[4]-'1')<<3) + buffer[3]-'a';
            u8 pos2 = ((buffer[7]-'1')<<3) + buffer[6]-'a';
            printf("Pos1: %d, Pos2: %d\n", pos1, pos2);
            bool isMoved = movePiece(&board, pos1, pos2, 0);
            if (!isMoved) printf("Invalid move\n");
        }

        printf("Turn: %d\n", board.turn);
        printBoard(&board);
        printf("\n");
    }

    return 0;
}