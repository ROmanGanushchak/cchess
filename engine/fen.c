#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "fen.h"
#include "engine.h"

char getPieceLetter(BoardState* board, u8 pos) {
    if (!( (board->figures[BOCCUPIED] >> pos) & 1 )) return '\0';
    char letter = '\0';
    if ((board->figures[BPAWN] >> pos) & 1) letter = 'p';
    else if ((board->figures[BBISHOP] >> pos) & 1 ) letter = 'b';
    else if ((board->figures[BKNIGHT] >> pos) & 1 ) letter = 'n';
    else if ((board->figures[BROOK] >> pos) & 1 ) letter = 'r';
    else if ((board->figures[BQUEEN] >> pos) & 1 ) letter = 'q';
    else if ((board->figures[BKING] >> pos) & 1 ) letter = 'k';
    assert(letter != '\0');

    if ((board->figures[BCOLOR] >> pos) & 1)
        letter += 'A' - 'a';
    return letter;
}

Boards getBoardFromStr(char piece) {
    if (piece >= 'A' && piece <= 'Z') piece += 'a' - 'A';
    if (piece == 'p') return BPAWN;
    if (piece == 'n') return BKNIGHT;
    if (piece == 'r') return BROOK;
    if (piece == 'b') return BBISHOP;
    if (piece == 'q') return BQUEEN;
    if (piece == 'k') return BKING;
    assert(1 && "Received unvalid piece to convert");
}

STR convertStateToFEN(BoardState* board) {
    STR str = {.len=0, .cap=256, .arr=NULL};
    str.arr = (char*) malloc (str.cap);

    for (i8 i=7; i>=0; i--) {
        u8 emptyCnt = 0;
        for (u8 j=0; j<8; j++) {
            u8 idx = (i << 3) | j;
            if (!( (board->figures[BOCCUPIED] >> idx) & 1 )) {
                emptyCnt++;
                continue;
            }

            if (emptyCnt) {
                str.arr[str.len++] = '0' + emptyCnt;
                emptyCnt = 0;
            }
            str.arr[str.len++] = getPieceLetter(board, idx);
        }
        if (emptyCnt)
            str.arr[str.len++] = '0' + emptyCnt;
        if (i != 0)
            str.arr[str.len++] = '/';
    }
    str.arr[str.len++] = ' ';
    str.arr[str.len++] = (board->turn) ? 'w' : 'b';
    str.arr[str.len++] = ' ';

    u32 lenBiforeCastleAdded = str.len;
    if (board->shortCastleWhite) str.arr[str.len++] = 'K';
    if (board->longCastleWhite) str.arr[str.len++] = 'Q';
    if (board->shortCastleBlack) str.arr[str.len++] = 'k';
    if (board->longCastleBlack) str.arr[str.len++] = 'q';
    if (lenBiforeCastleAdded == str.len) str.arr[str.len++] = '-';
    str.arr[str.len++] = ' ';


    if ((board->figures[BPAWN] >> board->lastMove.to) & 1 && 
        (board->lastMove.to >> 3) == 3 || (board->lastMove.to >> 3) == 4 && 
        (board->lastMove.from >> 3) == 1 || (board->lastMove.from >> 3) == 6) 
    {
        u8 pos;
        if ((board->lastMove.to >> 3) == 3) 
            pos = board->lastMove.to - 8;
        else 
            pos = board->lastMove.to + 8;
        str.arr[str.len++] = 'a' + (pos & 7);
        str.arr[str.len++] = '1' + (pos >> 3);
    } else 
        str.arr[str.len++] = '-';
    
    str.arr[str.len++] = ' ';

    if (board->halfMoves / 100 != 0) str.arr[str.len++] = ((board->halfMoves / 100) % 10) + '0';
    if (board->halfMoves / 10 != 0) str.arr[str.len++] = ((board->halfMoves / 10) % 10) + '0';
    str.arr[str.len++] = (board->halfMoves % 10) + '0';

    str.arr[str.len++] = ' ';
    str.arr[str.len++] = (board->repeatCount >> 1) + '0';
    
    str.arr[str.len] = '\0';
    assert(str.len+1 < str.cap);
    str.cap = str.len + 1;
    str.arr = realloc(str.arr, str.len+1);
    return str;
}


// add more validation
BoardState convertFENToState(STR fen) {
    BoardState board = {.shortCastleBlack=0, .shortCastleWhite=0, 
        .longCastleBlack=0, .longCastleWhite=0, 
        .isEnd=0, .lastMove=(MOVE){.from=0, .to=0}};
    
    memset(&board.figures, 0, BOARD_COUNT*sizeof(board.figures[BOCCUPIED]));
    i32 idx = 56;
    i32 i=0;
    for (; i<fen.len && fen.arr[i] == ' '; i++);
    for (; idx >= 0; i++) {
        if (i >= fen.len) return (BoardState){};
        if (fen.arr[i] >= '0' && fen.arr[i] <= '9') {
            idx += fen.arr[i] - '0';
        } else if ((fen.arr[i] >= 'a' && fen.arr[i] <= 'z') || (fen.arr[i] >= 'A' && fen.arr[i] <= 'Z')) {
            board.figures[getBoardFromStr(fen.arr[i])] |= ULL1 << idx;
            board.figures[BOCCUPIED] |= ULL1 << idx;
            board.figures[BCOLOR] |= (u64)(fen.arr[i] >= 'A' && fen.arr[i] <= 'Z') << idx;
            idx++;
        } else if (fen.arr[i] == '/') {
            if ((idx & 7) != 0) return (BoardState){};
            idx -= 16;
        } else if (fen.arr[i] == ' ') {
            if (idx == 8) break;
            return (BoardState){};
        }
    }

    if (fen.arr[i++] != ' ') return (BoardState){};
    for (; i<fen.len && fen.arr[i] == ' '; i++);

    i32 halfMoves, temp;
    char turn;
    char castles[256], enPassage[256];

    sscanf(fen.arr+i, "%c %s %s %d %d", &turn, castles, enPassage, &halfMoves, &temp);
    board.halfMoves = halfMoves;
    board.turn = turn == 'w';
    for (u8 j=0; j<4 && castles[j] != '\0' && castles[0] != '-'; j++) {
        if (castles[j] == 'K') board.shortCastleWhite = 1;
        else if (castles[j] == 'k') board.shortCastleBlack = 1;
        else if (castles[j] == 'Q') board.longCastleWhite = 1;
        else if (castles[j] == 'q') board.longCastleBlack = 1;
        else return (BoardState){};
    }

    if (enPassage[0] != '-') {
        i32 pos = ( (enPassage[1] - '1') << 3 ) | (enPassage[0] - 'a');
        if (pos < 0 || pos >= 64) return (BoardState){};
        bool side = (pos >> 3) == 2;
        board.lastMove = (side) ? (MOVE){.from=8+(pos&7), .to=24+(pos&7)} : (MOVE){.from=48+(pos&7), .to=32+(pos&7)};
    }
    return board;
}