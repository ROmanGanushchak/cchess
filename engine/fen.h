#ifndef CONVERTER_H
#define CONVERTER_H
#include "types.h"
#include "engine.h"

char getPieceLetter(BoardState* board, u8 pos);
STR convertStateToFEN(BoardState* board);
BoardState convertFENToState(STR fen);

#endif