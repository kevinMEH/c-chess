#ifndef PIECES_H
#define PIECES_H

#include <stdbool.h>


typedef struct Position {
    int x;
    int y;
} Position;

typedef enum Color { BLACK, WHITE } Color;

typedef enum Role { PAWN, BISHOP, KNIGHT, ROOK, QUEEN, KING } Role;

typedef struct Piece {
    Color color;
    Role role;
    Position position;
    int movesetSize;
    Position *moveset[28];
    bool special;
    // King and Rook: Moved
    // Pawn:  
} Piece;

typedef struct Diagonal { // Specifically for Pawn diagonals
    Position left;
    Position right;
} Diagonal;

#include "board.h"

void addMove(Piece*, int, int);
void updatePosition(Piece*, int, int);
bool inMoveset(Piece*, Position*);

void generate(Piece*);
void clearMoves(Piece*);
void pawn(Piece*);
void diagonals(Piece*, Diagonal*);
void bishop(Piece*);
void knight(Piece*);
void rook(Piece*);
void queen(Piece*);
void king(Piece*);

#endif