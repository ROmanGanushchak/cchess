#include <stdio.h>
#include <string.h>
#include "engine.h"
#include "moves.h"
#include "fen.h"
#include "pgn.h"

int main() {
    startEngine();
    STR str;
    str.arr = "1.c4 e5 2.Nc3 Nf6 3.Nf3 Nc6 4.e3 Bb4 5.Qc2 Bxc3 6.bxc3 O-O 7.e4 d6 8.Be2 Ne7\n9.O-O Ng6 10.Re1 Re8 11.a4 c6 12.d3 d5 13.h3 h6 14.Bf1 dxe4 15.dxe4 Be6 16.Be3 Nd7\n17.a5 b6 18.Qa4 Rc8 19.Red1 Qc7 20.Ne1 Ngf8 21.Nd3 c5 22.Ne1 Nb8 23.Nc2 Nc6\n24.axb6 axb6 25.f3 Qb7 26.Bf2 Ra8 27.Qb5 Nd7 28.Ne3 Rxa1 29.Rxa1 Ra8 30.Rxa8+ Qxa8\n31.Nd5 Na5 32.g3 Kh8 33.f4 exf4 34.gxf4 Qd8 35.Bg2 f6 36.Bg3 Kg8 37.Kh2 Kf7\n38.Bf3 Kg8 39.Kg2 f5 40.e5 Kf7 41.h4 g6 42.Kf2 h5 43.Kg2 Kg7 44.Bf2 Kf7 45.Ne3 Kg7\n46.Nd5 Kf7 47.Kh2 Kg7 48.Kg3 Qb8 49.Kh2 Qd8 50.Ne3 Nf8 51.Kg3 Qb8 52.Bg2 Bd7\n53.Qb1 Be6 54.Bd5 Bd7 55.Nf1 Qd8 56.Nh2 Be6 57.Qa2 Bf7 58.Nf3 Ne6 59.Qb1 Qc7\n60.Qb5 Nb7 61.Nd2 Nbd8 62.Qa4 Kf8 63.Nf1 Be8 64.Qa1 Bc6 65.Ne3 Qb7 66.Qb1 Ke8\n67.Bxc6+ Qxc6 68.Nd5 Nc7 69.Nxb6 Nde6 70.Nc8 Kd7 71.Nd6 Ne8 72.Nb5 Qe4 73.Qd1+ Ke7\n74.Qd2 Qxc4 75.Na3 Qb3 76.Nc2 Qa4 77.Be3 Qe4 78.Ne1 N8c7 79.Nd3 Qd5 80.c4 Qxc4\n81.Nxc5 Nd5 82.Qa5 Qe2 83.Qa7+ Nec7 84.Bf2 Qg4+ 85.Kh2 Qxf4+ 86.Kg1 Qg4+\n87.Kh2 Qe2 88.Kg1 Qg4+ 89.Kh2 Qf4+ 90.Kg1 Qxe5 91.Qa4 Kf7 92.Nd3 Qd6 93.Qd4 Ne6\n94.Ne5+ Kf6 95.Ng4+ Ke7 96.Qa7+ Qc7 97.Qa3+ Kf7 98.Ne3 Nxe3 99.Qxe3 Qf4 100.Qa7+ Qc7\n101.Qe3 Qd7 102.Qb3 Qc6 103.Qa2 Qb7 104.Be3 Qe7 105.Qc4 Kg7 106.Qc3+ Qf6\n107.Qb4 f4 108.Bf2 Kh6 109.Qd6 g5 110.hxg5+ Qxg5+ 111.Kh2 Qf5 112.Qb8 Qc2\n113.Kg1 Qd1+ 114.Kh2 Qc2 115.Kg1 Qd1+ 116.Kh2 Qe2 117.Kg1 Qg4+ 118.Kh2 f3\n119.Bg3 Qd4 120.Qe5 Qxe5 121.Bxe5 h4  0-1";
    str.len = strlen(str.arr);
    convertStrToPgn(str);
    return 0;

    // printf("HistSize: %d\n", sizeof(PGN_MOVE));
    // startEngine();

    // BoardState board = initialBoardState, saved = initialBoardState;
    // PGN pgn = createPGN(initialBoardState);
    // printBoard(&board);
    // char buffer[256];
    // while (true) {
    //     scanf("%s", buffer);
    //     if (strncmp(buffer, "mvback", 6) == 0) {
    //         if (!makeMoveBack(&board, &pgn)) 
    //             printf("NoMoves to go back\n");
    //     }
    //     else if (strncmp(buffer, "mvforw", 6) == 0) {
    //         if (!makeMoveForward(&board, &pgn)) 
    //             printf("NoMoves to go forward\n");
    //     }
    //     else if (strncmp(buffer, "mv", 3) == 0 || strncmp(buffer, "promote", 7) == 0) {
    //         printf("Regular move\n");
    //         scanf("%s %s", buffer+3, buffer+6);
    //         u8 pos1 = ((buffer[4]-'1')<<3) + buffer[3]-'a';
    //         u8 pos2 = ((buffer[7]-'1')<<3) + buffer[6]-'a';
    //         u8 promotingType = PROMOTING_NONE;
    //         if (strncmp(buffer, "mv", 2)) {
    //             scanf(" %c", buffer);
    //             switch (buffer[0]) {
    //             case 'r':
    //                 promotingType = PROMOTING_ROOK;
    //                 break;
    //             case 'b':
    //                 promotingType = PROMOTING_BISHOP;
    //                 break;
    //             case 'q':
    //                 promotingType = PROMOTING_QUENE;
    //                 break;
    //             case 'k':
    //                 promotingType = PROMOTING_KNIGHT;
    //                 break;
    //             }
    //         }
    //         if (!makeMove(&board, &pgn, pos1, pos2, promotingType)) {
    //             printf("Invalid move\n");
    //             goto ItEnd;
    //         }
    //         GAME_STATES state = pgn.moves[pgn.len-1].gameState;
    //         switch(state) {
    //         case GAME_BLACK_CHECK:
    //             printf("Black king is checked\n");
    //             break;
    //         case GAME_WHITE_CHECK:
    //             printf("White king is checked\n");
    //             break;
    //         case GAME_BLACK_WON:
    //             printf("Black won, white king is chackmated\n");
    //             break;
    //         case GAME_WHITE_WON:
    //             printf("White won, black king is chackmated\n");
    //             break;
    //         case GAME_DRAW:
    //             printf("Game ended with draw\n");
    //             break;
    //         case GAME_REGULAR:
    //             printf("Turn: %d\n", board.turn);
    //             break;
    //         }
    //     } else if (strncmp(buffer, "save", 4) == 0) {
    //         saved = board;
    //         printf("Saved\n");
    //     } else if (strncmp(buffer, "restore", 7) == 0) {
    //         board = saved;
    //         printf("Restored\n");
    //     } else if (strncmp(buffer, "showmv", 6) == 0) {
    //         scanf("%s", buffer);
    //         u8 pos = ((buffer[1]-'1')<<3) + buffer[0]-'a';
    //         printU64(getPossibleMoves(&board, pos));
    //     } else if (strncmp(buffer, "start-again", 11) == 0) {
    //         saved = board;
    //         board = initialBoardState;
    //         printf("New game started, if u wish to go back use command restore\n");
    //     } else if (strncmp(buffer, "export", 6) == 0) {
    //         STR str = convertStateToFEN(&board);
    //         printf("Game: %.*s\n", str.len, str.arr);
    //     } else if (strncmp(buffer, "import", 6) == 0) {
    //         STR str = {.len=0, .cap=256, .arr=buffer};
    //         fgets(str.arr, str.cap, stdin);
    //         str.len = strlen(str.arr);
    //         board = convertFENToState(str);
    //     }

    //     ItEnd:
    //     printBoard(&board);
    //     printf("\n");
    // }

    // return 0;
}