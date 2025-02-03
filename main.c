#include <stdio.h>
#include <string.h>
#include "engine.h"
#include "moves.h"
#include "fen.h"
#include "pgn.h"

int main() {
    printf("HistSize: %d\n", sizeof(PGN_MOVE));
    startEngine();

    BoardState board = initialBoardState, saved = initialBoardState;
    PGN pgn = createPGN(initialBoardState);
    PGN_VIEW view = (PGN_VIEW){.board=board, .pgn=&pgn};
    printBoard(&view.board);
    char buffer[256];
    while (true) {
        scanf("%s", buffer);
        if (strncmp(buffer, "mvback", 6) == 0) {
            if (!makeMoveBack(&view.board, &view)) 
                printf("NoMoves to go back\n");
        }
        else if (strncmp(buffer, "mvforw", 6) == 0) {
            if (!makeMoveForward(&view.board, &view)) 
                printf("NoMoves to go forward\n");
        }
        else if (strncmp(buffer, "mv", 3) == 0 || strncmp(buffer, "promote", 7) == 0) {
            printf("Regular move\n");
            scanf("%s %s", buffer+3, buffer+6);
            u8 pos1 = ((buffer[4]-'1')<<3) + buffer[3]-'a';
            u8 pos2 = ((buffer[7]-'1')<<3) + buffer[6]-'a';
            u8 promotingType = PROMOTING_NONE;
            if (strncmp(buffer, "mv", 2)) {
                scanf(" %c", buffer);
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
            if (!makeMove(&view, pos1, pos2, promotingType)) {
                printf("Invalid move\n");
                goto ItEnd;
            }
            GAME_STATES state = pgn.moves[pgn.len-1].gameState;
            switch(state) {
            case GAME_BLACK_CHECK:
                printf("Black king is checked\n");
                break;
            case GAME_WHITE_CHECK:
                printf("White king is checked\n");
                break;
            case GAME_BLACK_WON:
                printf("Black won, white king is chackmated\n");
                break;
            case GAME_WHITE_WON:
                printf("White won, black king is chackmated\n");
                break;
            case GAME_DRAW:
                printf("Game ended with draw\n");
                break;
            case GAME_REGULAR:
                printf("Turn: %d\n", view.board.turn);
                break;
            }
        } else if (strncmp(buffer, "save", 4) == 0) {
            saved = view.board;
            printf("Saved\n");
        } else if (strncmp(buffer, "restore", 7) == 0) {
            view.board = saved;
            printf("Restored\n");
        } else if (strncmp(buffer, "showmv", 6) == 0) {
            scanf("%s", buffer);
            u8 pos = ((buffer[1]-'1')<<3) + buffer[0]-'a';
            printU64(getPossibleMoves(&view.board, pos));
        } else if (strncmp(buffer, "start-again", 11) == 0) {
            printf("New game started, if u wish to go back use command restore\n");
            saved = view.board;
            board = initialBoardState;
            pgn = createPGN(initialBoardState);
            view = (PGN_VIEW){.board=board, .pgn=&pgn};
        } else if (strncmp(buffer, "export", 6) == 0) {
            STR str = convertStateToFEN(&view.board);
            printf("Game: %.*s\n", str.len, str.arr);
        } else if (strncmp(buffer, "import", 6) == 0) {
            STR str = {.len=0, .cap=256, .arr=buffer};
            fgets(str.arr, str.cap, stdin);
            str.len = strlen(str.arr);
            board = convertFENToState(str);
            pgn = createPGN(board);
            view = (PGN_VIEW){
                .board=convertFENToState(str),
                .pgn=&pgn,
                .idx=0
            };
        }

        ItEnd:
        printBoard(&view.board);
        printf("\n");
    }

    return 0;
}