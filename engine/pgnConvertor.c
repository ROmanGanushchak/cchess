#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include "pgn.h"
#include "moves.h"

#define belong(elem, min, max) (elem >= min && elem <= max)
PROMOTION_TYPE getPromotionFromChar(char letter) {
    if (letter >= 'A' && letter <= 'Z') letter += 'a' - 'A';
    switch (letter) {
    case 'q': return PROMOTING_QUENE;
    case 'r': return PROMOTING_ROOK;
    case 'b': return PROMOTING_BISHOP;
    case 'n': return PROMOTING_KNIGHT;
    default: return PROMOTING_NONE;
    }
}

Boards getFigureFromChar(char letter) {
    switch (letter) {
    case 'P': return BPAWN;
    case 'R': return BROOK;
    case 'N': return BKNIGHT;
    case 'K': return BKING;
    case 'Q': return BQUEEN;
    case 'B': return BBISHOP;
    }
    return BOCCUPIED;
}

char getFigureChar(Boards figure) {
    switch (figure) {
    case BPAWN: return 'P';
    case BKNIGHT: return 'N';
    case BBISHOP: return 'B';
    case BROOK: return 'R';
    case BQUEEN: return 'Q';
    case BKING: return 'K';
    default: return '\0';
    }
}

STR filterPGNStr(STR _str) {
    char* str = malloc(_str.len);
    u32 size = _str.len;
    printf("_STR: %p\n", _str.arr);
    memcpy(str, _str.arr, size);
    u32 i = 0;
    for (; i<size; i++) {
        if (str[i] == '[') 
            for (; i<size && str[i] != ']'; i++);
        else if (str[i] == '1')
            break;
    }
    u32 offset = i;

    for (; i<size; i++) {
        if (str[i] == '(' || str[i] == '[' || str[i] == '{') {
            char closed = '\0';
            if (str[i] == '(') closed = ')';
            else if (str[i] == '[') closed = ']';
            else if (str[i] == '{') closed = '}';
            for (; i<size && str[i] != closed; i++, offset++);
            offset++;
            continue;
        }
        u32 j = i;
        for (; j<size && str[j] >= '0' && str[j] <= '9'; j++);
        if (str[j] == '.') {
            offset += j - i;
            i = j;
        }
        if (str[i] == '.') {
            for (j=i; j<size-1 && str[j+1] == '.'; j++, offset++);
            i = j;
            if (i-offset == 0 || str[i-offset-1] != '.')
                str[i-offset] = '.';
            else offset++;
            continue;
        }
        if (str[i] == 'x' || str[i] == '+') {
            offset++;
            continue;
        }
        if (str[i] == ' ' || str[i] == '.' || str[i] == '\n') {
            if (i-offset == 0 || str[i-offset-1] != '.')
                str[i-offset] = '.';
            else offset++;
            continue;
        }
        str[i-offset] = str[i];
    }
    size -= offset;
    str[size++] = '.';
    str[size] = '\0';
    return (STR){.arr=str, .cap=_str.len, .len=size};
}

u64 getPossibleFroms(BoardState board, Boards figure, u8 to) {
    u64 res = 0;
    u64 bcolor = board.turn ? board.figures[BCOLOR] : ~board.figures[BCOLOR];
    // printf("Figure: %d, to: %d\n", figure, to);
    switch (figure) {
    case BPAWN:
        // printf("Bpawn, to: %d\n", to);
        u64 pawns = board.figures[BPAWN] & bcolor;
        if (board.turn) {
            if ((board.figures[BOCCUPIED] >> to) & 1) 
                res = (((pawns & ~LEFT_EDGE ) >> (to-7)) & 1) << (to-7) |
                (( (pawns & ~(LEFT_EDGE << 7)) >> (to-9) ) & 1) << (to - 9);
            res |= ((pawns >> (to - 8)) & 1) << (to - 8) | 
                ((pawns >> (to - 16)) & 1) << (to - 16); 
        }
        else {
            if ((board.figures[BOCCUPIED] >> to) & 1)
                res = (( (pawns & ~(LEFT_EDGE << 7) ) >> (to+7)) & 1) << (to + 7) |
                (( (pawns & ~LEFT_EDGE) >> (to+9) ) & 1 ) << (to + 9); 
            res |= ((pawns >> (to + 8)) & 1) << (to + 8) |
                ((pawns >> (to + 16)) & 1) << (to + 16);
        }
        break;
    case BKING:
        res = ULL1 << __builtin_ctzll(board.figures[BKING] & bcolor);
        break;
    default:
        u64 moves = getCandidateMoves(&board, figure, to, !board.turn, true);
        // moves = removeInvalidMoves(&board, moves, to, figure);
        res = moves & board.figures[figure] & bcolor;
        break;
    }
    return res;
}


OPTIONAL_PGN_VIEW convertStrToPgn(STR _str) {
    STR filtered = filterPGNStr(_str);
    char* str = filtered.arr;
    u32 size = filtered.len;
    printf("Filtered: %.*s\n", size, str);

    u32 movesCount = 0;
    for (u32 i=0; i<size-1; i++)
        movesCount += str[i] == '.';

    BoardState board = initialBoardState;
    PGN pgn = createPGN(board);
    u32 i = 0;
    u32 it = 0;
    while (i < size) {
        it++;
        for (; i<size && str[i] == '.'; i++);
        if (i == size || (str[i] == '1')) break;
        
        u8 from=64, to=64;
        if (str[i] == 'O' && str[i+1] == '-') {
            printf("Castling\n");
            bool isLong = str[i + 3] == '-';
            if (board.turn) {
                from = 4;
                to = isLong ? 2 : 6;
            } else {
                from = 60;
                to = isLong ? 58 : 62;
            }
            if (isLong)
                i += 5;
            else i += 3;
            goto execute_move;
        }

        Boards figure = BPAWN;

        if (str[i] >= 'A' && str[i] <= 'Z') 
            figure = getFigureFromChar(str[i++]);
        u64 mask = ULONG_LONG_MAX;
        if (belong(str[i], 'a', 'h') && belong(str[i+1], '1', '8')) {
            to = ((str[i+1] - '1') << 3) + str[i] - 'a';
            i += 2;
        } else if (belong(str[i], 'a', 'h') && belong(str[i+1], 'a', 'h') && belong(str[i+2], '1', '8')) {
            mask = LEFT_EDGE << (str[i] - 'a');
            to = ((str[i+2] - '1') << 3) + str[i+1] - 'a';
            i += 3;
        } else if (belong(str[i], '1', '8') && belong(str[i+1], 'a', 'h') && belong(str[i+2], '1', '8')) {
            mask = BOTTOM_ROW << ( (str[i] - '1') << 3 );
            to = ((str[i+2] - '1') << 3) + str[i+1] - 'a';
            i += 3;
        } else if (belong(str[i], 'a', 'h') && belong(str[i+1], '1', '8') && belong(str[i+2], 'a', 'h') && belong(str[i+2], '1', '8')) {
            mask = 0;
            to = ((str[i+3] - '1') << 3) + str[i+2] - 'a';
            from = ((str[i+1] - '1') << 3) + str[i] - 'a';
            i += 4;
        } else break;

        printBoard(&board);
        printf("Going to: %d, piece: %d, it: %d\n", to, figure, it);

        if (from == 64 && figure == BPAWN) {
            u64 pawns = mask & board.figures[BPAWN] & (board.turn ? board.figures[BCOLOR] : ~board.figures[BCOLOR]);
            if (board.turn) {
                if ((pawns >> (to - 8)) & 1) 
                    from = to - 8;
                else if (((pawns >> (to - 16)) & 1)) 
                    from = to - 16;
                else if (( (pawns & ~LEFT_EDGE ) >> (to-7)) & 1) 
                    from = to-7;
                else if (( (pawns & ~(LEFT_EDGE << 7)) >> (to-9) ) & 1) 
                    from = to-9;
            } else {
                if ((pawns >> (to + 8)) & 1) 
                    from = to + 8;
                else if (((pawns >> (to + 16)) & 1)) 
                    from = to + 16;
                else if (( (pawns & ~(LEFT_EDGE << 7) ) >> (to+7)) & 1) 
                    from = to+7;
                else if (( (pawns & ~LEFT_EDGE) >> (to+9) ) & 1) 
                    from = to+9;
            }
        } else if (from == 64 && figure == BKING) 
            from = __builtin_ctzll(board.figures[BKING] & (board.turn ? board.figures[BCOLOR] : ~board.figures[BCOLOR]));
        else if (from == 64) {
            u64 moves = getCandidateMoves(&board, figure, to, !board.turn, true);
            // moves = removeInvalidMoves(&board, moves, to, figure);
            u64 figures = moves & mask & board.figures[figure] & (board.turn ? board.figures[BCOLOR] : ~board.figures[BCOLOR]);
            if (__builtin_popcountll(figures) != 1) {
                printf("Invalid position\n");
                return (OPTIONAL_PGN_VIEW){.isValid=false};
            }
            from = __builtin_ctzll(figures);
        }
        if (from == 64) {
            printf("Couldnt find a from for move, figure: %d, to: %d\n", figure, to);
            return (OPTIONAL_PGN_VIEW){.isValid=false};
        }

        PROMOTION_TYPE promotion = PROMOTING_NONE;
        if (str[i] == '=') {
            if (figure != BPAWN) {
                printf("Trying to promote no pawn\n");
                return (OPTIONAL_PGN_VIEW){.isValid=false};
            }
            promotion = getPromotionFromChar(str[i+1]);
            i += 2;
        }

        execute_move:
        PGN_MOVE move = applyMoveWithCheck(&board, from, to, promotion);
        addMove(&pgn, move);
    }

    printBoard(&board);
    printf("ITS: %d\n", it);

    PGN* _pgn = malloc(sizeof(PGN));
    *_pgn = pgn;
    return (OPTIONAL_PGN_VIEW){.isValid=true, .view=(PGN_VIEW){
        .board=initialBoardState, .idx=0, .pgn=_pgn
    }};
}

STR convertPgnToStr(PGN* pgn) {
    if (memcpy(&pgn->initBoard, &initialBoardState, sizeof(BoardState)) == 0) 
        return (STR){.arr=NULL};
    BoardState board = initialBoardState;    
    u32 size = 256, top = 0;
    char* str = malloc(size);

    for (int i=0; i<pgn->len; i++) {
        if (size - top < 64) {
            size <<= 1;
            str = realloc(str, size);
        }
        if ((i & 1) == 0) {
            top += sprintf(str+top, "%d", i/2+1);
            str[top++] = '.';
        }

        PGN_MOVE move = pgn->moves[i];
        Boards figure = move.piece;
        bool isNeedToPrintFrom = true, isSourcePrinted=false;

        if (move.moveType == MOVE_CASTLING) {
            str[top++] = 'O'; str[top++] = '-'; str[top++] = 'O';
            if (move.to < move.from) {
                str[top++] = 'O'; 
                str[top++] = '-';
            }
        } else {
            if (figure != BPAWN)
                str[top++] = getFigureChar(figure);
            u64 froms = getPossibleFroms(board, figure, move.to);
            u8 figureCount = __builtin_popcountll(froms);
            assert(figureCount != 0 && "Couldnt find source figure");
            if (figureCount > 1) {
                if (__builtin_popcountll(froms & (LEFT_EDGE << (move.from & 7))) == 1) 
                    str[top++] = 'a' + (move.from & 7);
                else if (__builtin_popcountll(froms & (BOTTOM_ROW << ((move.from >> 3) << 3))) == 1)
                    str[top++] = '1' + (move.from >> 3);
                else {
                    str[top++] = 'a' + (move.from & 7);
                    str[top++] = '1' + (move.from >> 3);
                }
            }
            if (move.captured != BOCCUPIED || move.moveType == MOVE_EN_PASSAGE) {
                if (figureCount == 1 && figure == BPAWN)
                    str[top++] = 'a' + (move.from & 7);
                str[top++] = 'x';
            }
            str[top++] = 'a' + (move.to & 7);
            str[top++] = '1' + (move.to >> 3);
        }

        if (move.promotion != PROMOTING_NONE) {
            str[top++] = '=';
            str[top++] = getFigureChar(getBoardFromPromotion(move.promotion));
        }

        // needs to add ++ for double check
        if (move.gameState == GAME_BLACK_CHECK || move.gameState == GAME_WHITE_CHECK)
            str[top++] = '+';
        printf("i: %d -> state: %d\n", i, move.gameState);
        str[top++] = ' ';

        if (move.moveType == MOVE_EN_PASSAGE) {
            memcpy(str+top, "e.p. ", 5);
            top += 5;
        }
        applyMove(&board, move);
    }
    str[top] = '\0';
    return (STR){.arr=str, .len=top, .cap=size};
}