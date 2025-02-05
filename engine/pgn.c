#include <malloc.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "pgn.h"
#include "moves.h"
#include "unsafeMoves.h"
#include "attacks.h"

#define belong(elem, min, max) (elem >= min && elem <= max)

void addMove(PGN* pgn, PGN_MOVE move) {
    if (pgn->len + 1 > pgn->cap) {
        pgn->cap <<= 1;
        pgn->moves = realloc(pgn->moves, pgn->cap * sizeof(PGN_MOVE));
    }
    pgn->moves[pgn->len++] = move;
}

PGN createPGN(BoardState board) {
    PGN pgn = (PGN){.cap=16, .len=0, .initBoard=board, .idx=-1};
    pgn.moves = malloc(pgn.cap * sizeof(PGN_MOVE));
    return pgn;
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

/* If first small letter, move with pawn, otherwise read the figure.
If then is x ignore it. If first letter is O then it is castling.
If a single position, then just move any piece that is suitable, if single letter from that column, if 2 positions then move from -> to  */

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


// PGN_MOVE getPGNViewMove(PGN_VIEW* view, u16 idx) {
//     printf("Got len: %d, idx: %d\n", getPGN_VIEW_len(view), idx);
//     i32 forkIdx = view->forkstack.len-1;
//     for (; forkIdx >= 0 && view->forkstack.arr[forkIdx].idx < idx; forkIdx--);
//     if (forkIdx != -1) {
//         i32 _idx = idx - view->forkstack.arr[forkIdx].idx;
//         assert(_idx < view->forkstack.arr[forkIdx].len);
//         return view->forkstack.arr[forkIdx].savedMoves[_idx];
//     }
//     assert(idx < view->pgn->len);
//     return view->pgn->moves[idx];
// }

// void saveMoveToView(PGN_VIEW* view, PGN_MOVE move) {
//     if (view->forkstack.len == 0) {
//         addMove(view->pgn, move);
//         return;
//     }
//     FORK_INFO *fork = &view->forkstack.arr[view->forkstack.len-1];
//     if (fork->len == fork->cap) {
//         if (fork->cap == 0) {
//             fork->cap = 8;
//             fork->savedMoves = malloc(fork->cap * sizeof(fork->savedMoves[0]));
//         } else {
//             fork->cap *= 2;
//             fork->savedMoves = realloc(fork->savedMoves, fork->cap * sizeof(fork->savedMoves[0]));
//         }
//     }
//     fork->savedMoves[fork->len++] = move;
// }

// void createNewFork(PGN_VIEW* view, u16 idx) {
//     FORK_ARR *forks = &view->forkstack;
//     assert(forks->arr[forks->len-1].idx < idx && "Forks have to be consecutive");
//     if (forks->len == forks->cap) {
//         if (forks->cap == 0) {
//             forks->cap = 8;
//             forks->arr = malloc(forks->cap * sizeof(forks->arr[0]));
//         } else {
//             forks->cap *= 2;
//             forks->arr = realloc(forks->arr, forks->cap * sizeof(forks->arr[0]));
//         }
//     }
//     forks->arr[forks->len++] = (FORK_INFO){.cap=0, .len=0, .idx=idx, .savedMoves=NULL};
// }

// void exitFork(PGN_VIEW* view) {
//     if (view->forkstack.len == 0) {
//         fprintf(stderr, "Trying to delete non-existing fork for PGN_VIEW, %s:%d\n", __FILE__, __LINE__);
//         return;
//     }
//     free(view->forkstack.arr[view->forkstack.len-1].savedMoves);
//     view->forkstack.len--;
// }

// u32 getPGN_VIEW_len(PGN_VIEW* view) {
//     if (view->forkstack.len == 0)
//         return view->pgn->len;
//     FORK_INFO lastFork = view->forkstack.arr[view->forkstack.len-1];
//     return lastFork.idx + lastFork.len;
// }