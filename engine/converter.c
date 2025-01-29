#include <assert.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "converter.h"
#include "engine.h"

char getPieceLetter(GameState* state, u8 pos) {
    if (!( (state->figures[BOCCUPIED] >> pos) & 1 )) return '\0';
    char letter = '\0';
    if ((state->figures[BPAWN] >> pos) & 1) letter = 'p';
    else if ((state->figures[BBISHOP] >> pos) & 1 ) letter = 'b';
    else if ((state->figures[BKNIGHT] >> pos) & 1 ) letter = 'n';
    else if ((state->figures[BROOK] >> pos) & 1 ) letter = 'r';
    else if ((state->figures[BQUEEN] >> pos) & 1 ) letter = 'q';
    else if ((state->figures[BKING] >> pos) & 1 ) letter = 'k';
    assert(letter != '\0');

    if ((state->figures[BCOLOR] >> pos) & 1)
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

STR convertStateToFEN(GameState* state) {
    STR str = {.len=0, .cap=256, .arr=NULL};
    str.arr = (char*) malloc (str.cap);

    for (i8 i=7; i>=0; i--) {
        u8 emptyCnt = 0;
        for (u8 j=0; j<8; j++) {
            u8 idx = (i << 3) | j;
            if (!( (state->figures[BOCCUPIED] >> idx) & 1 )) {
                emptyCnt++;
                continue;
            }

            if (emptyCnt) {
                str.arr[str.len++] = '0' + emptyCnt;
                emptyCnt = 0;
            }
            str.arr[str.len++] = getPieceLetter(state, idx);
        }
        if (emptyCnt)
            str.arr[str.len++] = '0' + emptyCnt;
        if (i != 0)
            str.arr[str.len++] = '/';
    }
    str.arr[str.len++] = ' ';
    str.arr[str.len++] = (state->turn) ? 'w' : 'b';
    str.arr[str.len++] = ' ';

    u32 lenBiforeCastleAdded = str.len;
    if (state->doubleCastleWhite) str.arr[str.len++] = 'K';
    if (state->thribleCastleWhite) str.arr[str.len++] = 'Q';
    if (state->doubleCastleBlack) str.arr[str.len++] = 'k';
    if (state->thribleCastleBlack) str.arr[str.len++] = 'q';
    if (lenBiforeCastleAdded == str.len) str.arr[str.len++] = '-';
    str.arr[str.len++] = ' ';


    if ((state->figures[BPAWN] >> state->lastMove.to) & 1 && 
        (state->lastMove.to >> 3) == 3 || (state->lastMove.to >> 3) == 4 && 
        (state->lastMove.from >> 3) == 1 || (state->lastMove.from >> 3) == 6) 
    {
        u8 pos;
        if ((state->lastMove.to >> 3) == 3) 
            pos = state->lastMove.to - 8;
        else 
            pos = state->lastMove.to + 8;
        str.arr[str.len++] = 'a' + (pos & 7);
        str.arr[str.len++] = '1' + (pos >> 3);
    } else 
        str.arr[str.len++] = '-';
    
    str.arr[str.len] = '\0';
    assert(str.len+1 < str.cap);
    str.cap = str.len + 1;
    str.arr = realloc(str.arr, str.len+1);
    return str;
}


// add more validation
GameState convertFENToState(STR fen) {
    GameState state = {.doubleCastleBlack=0, .doubleCastleWhite=0, 
        .thribleCastleBlack=0, .thribleCastleWhite=0, 
        .isCheck=0, .isMate=0, .lastMove=(MOVE){.from=0, .to=0}};
    
    memset(&state.figures, 0, BOARD_COUNT*sizeof(state.figures[BOCCUPIED]));
    i32 idx = 56;
    i32 i=0;
    for (; i<fen.len && fen.arr[i] == ' '; i++);
    for (; idx >= 0; i++) {
        if (i >= fen.len) return (GameState){};
        if (fen.arr[i] >= '0' && fen.arr[i] <= '9') {
            idx += fen.arr[i] - '0';
        } else if ((fen.arr[i] >= 'a' && fen.arr[i] <= 'z') || (fen.arr[i] >= 'A' && fen.arr[i] <= 'Z')) {
            state.figures[getBoardFromStr(fen.arr[i])] |= ULL1 << idx;
            state.figures[BOCCUPIED] |= ULL1 << idx;
            state.figures[BCOLOR] |= (u64)(fen.arr[i] >= 'A' && fen.arr[i] <= 'Z') << idx;
            idx++;
        } else if (fen.arr[i] == '/') {
            if ((idx & 7) != 0) return (GameState){};
            idx -= 16;
        } else if (fen.arr[i] == ' ') {
            if (idx == 8) break;
            return (GameState){};
        }
    }

    if (fen.arr[i++] != ' ') return (GameState){};
    for (; i<fen.len && fen.arr[i] == ' '; i++);

    if (fen.arr[i] == 'w') state.turn = 1;
    else if (fen.arr[i] == 'b') state.turn = 0;
    else return (GameState){};
    i++;

    for (; i<fen.len && fen.arr[i] == ' '; i++);

    for (; fen.arr[i] != '-' && fen.arr[i] != ' '; i++) {
        if (i >= fen.len) return (GameState){};
        if (fen.arr[i] == 'K') state.doubleCastleWhite = 1;
        else if (fen.arr[i] == 'k') state.doubleCastleBlack = 1;
        else if (fen.arr[i] == 'Q') state.thribleCastleWhite = 1;
        else if (fen.arr[i] == 'q') state.thribleCastleBlack = 1;
        else return (GameState){};
    }

    for (; i<fen.len && fen.arr[i] == ' '; i++);
    if (i >= fen.len) return (GameState) {};

    if (fen.arr[i] == '-') return state;
    if (fen.arr[i] < 'a' || fen.arr[i] > 'h' || fen.arr[i+1] < '1' || fen.arr[i+1] > '8')
        return (GameState){};
    u8 pos = ( (fen.arr[i+1] - '1') << 3 ) | (fen.arr[i] - 'a');
    bool side = (pos >> 3) == 2;
    state.lastMove = (side) ? (MOVE){.from=8+(pos&7), .to=16+(pos&7)} : (MOVE){.from=48+(pos&7), .to=32+(pos&7)};
    return state;
}