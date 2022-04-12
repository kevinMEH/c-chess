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

bool valid(int, int, Color);
bool targeted(int, int, Color);
bool inBounds(int, int);
bool isEmpty(int, int);
bool enemy(Color, Color);
Piece *pieceAt(int, int);


#endif