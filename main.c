#include <stdio.h>
#include <string.h>
#include "engine.h"
#include "figures.h"
#include "slidingMove.h"
#include "moves.h"

int main() {
    startEngine();

    GameState board = initialGameState, saved = initialGameState;
    printBoard(&board);
    char buffer[256];
    while (true) {
        scanf("%s", buffer);
        // printf("Buffer: %s\n", buffer);
        if ((buffer[0] == 'm' && buffer[1] == 'v') || strncmp(buffer, "promote", 7) == 0) {
            scanf("%s %s", buffer+3, buffer+6);
            u8 pos1 = ((buffer[4]-'1')<<3) + buffer[3]-'a';
            u8 pos2 = ((buffer[7]-'1')<<3) + buffer[6]-'a';
            u8 promotingType = PROMOTING_NONE;
            if (strncmp(buffer, "mv", 2)) {
                printf("Scanning type\n");
                scanf(" %c", buffer);
                printf("Buffer0: %c\n", buffer[0]);
                switch (buffer[0]) {
                case 'r':
                    promotingType = PROMOTING_ROOK;
                    break;
                case 'b':
                    promotingType = PROMOTING_BISHOP;
                    break;
                case 'q':
                    promotingType = PROMOTING_QUENE;
                    break;
                case 'k':
                    promotingType = PROMOTING_KNIGHT;
                    break;
                }
            }
            printf("Promoting: %d\n", promotingType);
            bool isMoved = movePiece(&board, pos1, pos2, promotingType);
            if (!isMoved) printf("Invalid move\n");
        } else if (strncmp(buffer, "save", 4) == 0) {
            saved = board;
            printf("Saved\n");
        } else if (strncmp(buffer, "restore", 7) == 0) {
            board = saved;
            printf("Restored\n");
        } else if (strncmp(buffer, "showmv", 6) == 0) {
            scanf("%s", buffer);
            u8 pos = ((buffer[1]-'1')<<3) + buffer[0]-'a';
            printU64(getPossibleMoves(&board, pos));
        }

        printf("Turn: %d\n", board.turn);
        printBoard(&board);
        printf("\n");
    }

    return 0;
}