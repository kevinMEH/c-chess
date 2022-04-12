#ifndef GAME_H
#define GAME_H

#include "piece.h"

void game();

void printBoard();
void promptMove(Color);
Position *promptPosition();
char *pieceChar(int, int);

#endif