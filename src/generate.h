#ifndef GENERATE_H
#define GENERATE_H

#include "piece.h"


void init();
Piece *palloc();
void initPiece(Piece*, Color, Role, int, int);

void generateMoves(Color);

void clear(Piece**, int);

#endif