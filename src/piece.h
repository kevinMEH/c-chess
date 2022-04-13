#ifndef PIECES_H
#define PIECES_H

#include <stdbool.h>
#include "generate.h"

typedef enum Mode { CONSTRUCT, REGULAR, CHECK } Mode;
typedef enum Rank { ALL, ROW, COLUMN, TL_DIAGONAL, TR_DIAGONAL } Rank;

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
    int special;
    // King and Rook: Moved
    // Pawn: Turn moved
} Piece;

typedef struct Diagonal { // Specifically for Pawn diagonals
    Position left;
    Position right;
} Diagonal;



void addMove(Piece*, int, int);
void updatePosition(Piece*, int, int);
bool inMoveset(Piece*, Position*);

bool valid(int, int, Color);
bool targeted(int, int, Color);
bool inBounds(int, int);
bool isEmpty(int, int);
bool enemy(Color, Color);
void generate(Piece*, Mode, Rank);
void clearMoves(Piece*);
void pawn(Piece*);
Role promptPromotion();
void diagonals(Piece*, Diagonal*);
void bishop(Piece*);
void knight(Piece*);
void rook(Piece*);
void queen(Piece*);
void king(Piece*);

char getSymbol(Color, Role);

#endif