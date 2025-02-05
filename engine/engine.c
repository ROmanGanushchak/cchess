#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "engine.h"
#include "moves.h"
#include "attacks.h"
#include "slidingMove.h"

BoardState initialBoardState;

void initInitialBoardState() {
    initialBoardState = (BoardState) {.halfMoves = 0, .turn = 1, 
        .isEnd=0, .shortCastleBlack=1, 
        .shortCastleWhite=1, .longCastleBlack=1, 
        .longCastleWhite=1};
    
    initialBoardState.figures[BCOLOR] = 0xFFFF;
    initialBoardState.figures[BOCCUPIED] = 0xFFFF | ((u64)0xFFFF << 48);
    initialBoardState.figures[BKING] = 0x10 | ((u64)0x10 << 56);
    initialBoardState.figures[BKNIGHT] = 0x42 | ((u64)0x42 << 56);
    initialBoardState.figures[BPAWN] = (0xFF << 8) | ((u64)0xFF << 48);
    initialBoardState.figures[BBISHOP] = 0x24 | ((u64)0x24 << 56);
    initialBoardState.figures[BROOK]   = 0x81 | ((u64)0x81 << 56);
    initialBoardState.figures[BQUEEN]  = 0x8  | ((u64)0x8 << 56);
}

void startEngine() {
    initInitialBoardState();
    initSlidingPiecesTables();
}

bool isGameEnd(GAME_STATES state) {
    return !(state == GAME_REGULAR || state == GAME_BLACK_CHECK || state == GAME_WHITE_CHECK);
}

GAME_STATES getIsCheck(u64 king, u64 attacked, bool side) {
    if (king & attacked) {
        if (side)
            return GAME_WHITE_CHECK;
        return GAME_BLACK_CHECK;
    }
    return GAME_REGULAR;
}

GAME_STATES isCheckmate(BoardState* board) {
    bool side = board->turn;
    u64 attacked = getAttacks(board, !side);
    u64 allies = board->figures[BOCCUPIED] & (side ? board->figures[BCOLOR] : ~board->figures[BCOLOR]);
    u64 king = board->figures[BKING] & (side ? board->figures[BCOLOR] : ~board->figures[BCOLOR]);
    if (possibleKingAttacks(board, side) & ~attacked & ~allies) 
        return getIsCheck(king, attacked, side);

    for (u8 i=0; i<64; i++) 
        if (( allies >> i ) & 1 && getPossibleMoves(board, i))
            return getIsCheck(king, attacked, side);
    
    if (king & attacked) {
        if (side)
            return GAME_BLACK_WON;
        return GAME_WHITE_WON;
    }
    return GAME_DRAW;   
}

GAME_STATES getGameState(BoardState* board) {
    GAME_STATES state = isCheckmate(board);
    if (isGameEnd(state)) {
        board->isEnd = true;
        return state;
    }
    if (board->halfMoves >= 100 || board->repeatCount >= 6)
        return GAME_DRAW;
    return state;
}

void printU64(u64 a) {
    for (i8 i=7; i>=0; i--) {
        for (u8 j=0; j<8; j++) {
            printf("%d", (a >> (i*8+j)) & 1, i*8+j);
        }printf("\n");
    }printf("\n");
}

void printBoard(BoardState* board) {
    for (i8 i=7; i>=0; i--) {
        printf("\033[38;5;195m%d | \033[0m", i+1);
        for (i8 j=0; j<8; j++) {
            u8 idx = i*8+j;
            if (( ( board->figures[BOCCUPIED] >> idx ) & 1) == 0) {
                printf(". ");
                continue;
            }
            u8 cnt = 0;
            char type = '\0';
            if (( board->figures[BBISHOP] >> idx ) & 1) {type = 'b'; cnt++;}
            if (( board->figures[BROOK] >> idx ) & 1) {type = 'r'; cnt++;}
            if (( board->figures[BQUEEN] >> idx ) & 1) {type = 'q'; cnt++;}
            if (( board->figures[BKNIGHT] >> idx ) & 1) {type = 'k'; cnt++;}
            if (( board->figures[BKING] >> idx ) & 1) {type = 'm'; cnt++;}
            if (( board->figures[BPAWN] >> idx ) & 1) {type = 'p'; cnt++;}
            if (cnt != 1) {
                printf("\nINCORRECT BOARD %d, cnt: %d\n", idx, cnt); 
                return;
            }
            if (( board->figures[BCOLOR] >> idx ) & 1) 
                printf("\033[38;5;214m%c\033[0m ", type + 'A' - 'a');
            else 
                printf("\033[38;5;50m%c\033[0m ", type);
        }
        printf("\n");
    }
    printf("    \033[38;5;195m");
    for (u8 i=1; i<=7; i++)
        printf("--", i);
    printf("-\n    ");
    for (u8 i=0; i<8; i++)
        printf("%c ", 'a'+i);
    printf("\033[0m\n");
}

Boards getFigureBoard(BoardState* board, u8 pos) {
    if (board->figures[BPAWN] & (ULL1 << pos)) return BPAWN;
    if (board->figures[BBISHOP] & (ULL1 << pos)) return BBISHOP;
    if (board->figures[BQUEEN] & (ULL1 << pos)) return BQUEEN;
    if (board->figures[BROOK] & (ULL1 << pos)) return BROOK;
    if (board->figures[BKNIGHT] & (ULL1 << pos)) return BKNIGHT;
    if (board->figures[BKING] & (ULL1 << pos)) return BKING;
    return BOCCUPIED;
}

FIGURE getFigureFromBoard(Boards board) {
    switch (board) {
    case BKING: return FKING;
    case BQUEEN: return FQUEEN;
    case BROOK: return FROOK;
    case BBISHOP: return FBISHOP;
    case BKNIGHT: return FKNIGHT;
    case BPAWN: return FPAWN;
    }
    assert(1 == 1 && "Incorrect board");
}