#ifndef CONVERTER_H
#define CONVERTER_H
#include "types.h"
#include "engine.h"

char getPieceLetter(GameState* state, u8 pos);
STR convertStateToFEN(GameState* state);
GameState convertFENToState(STR fen);

#endif