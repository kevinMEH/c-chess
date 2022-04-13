#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include "piece.h"

typedef struct Board {
    Piece *content[8][8];
    Piece *pieces[32];
    int pieceCount;
    int turnNumber;
} Board;

void movePiece(int, int, int, int);
void clearSquare(int, int);
void capture(int, int);

Piece *pieceAt(int, int);

#endif