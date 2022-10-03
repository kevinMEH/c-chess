#ifndef PIECES_H
#define PIECES_H

#include <stdbool.h>

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

typedef struct Pin {
    Piece *attacker;
    Piece *pinned;
    Rank rank;
} Pin;

typedef struct Check {
    Piece *attacker;
    Position** intermediatePositions;
    int positionsSize;
} Check;

typedef enum Validity { INVALID, VALID, VCHECK } Validity;

typedef enum ConResult { BREAK, CHECKED, OTHER } ConResult;


#include "generate.h"


void addMove(Piece*, int, int);
void addCheck(Piece*, Position**, int);
void addCheckNoInter(Piece*);
void addPin(Piece*, Piece*, Rank);
void clearChecksAndPins();
void updatePosition(Piece*, int, int);
bool inMoveset(Piece*, Position*);

bool valid(int, int, Color);
Validity conValid(int, int, Color);
bool targeted(int, int, Color);
bool inBounds(int, int);
bool isEmpty(int, int);
bool enemy(Piece*, Color);
bool isKing(Piece*);
bool enemyKing(Piece*, Color);
bool posBlocksCheck(int, int);

void generate(Piece*, Mode, Rank);
void clearMoves(Piece*);
void clearAll(Piece**, int);

void checkModeAdd(Piece*, int, int, Mode);

void pawn(Piece*, Mode, Rank);
Role promptPromotion();
void diagonals(Piece*, Diagonal*);
void freeInterPos(Position**, int);
ConResult slidingValidateConstructBody(Piece*, int, int, bool*, Piece**, Position**, int*, Rank);
void bishop(Piece*, Mode, Rank);
void knight(Piece*, Mode, Rank);
void rook(Piece*, Mode, Rank);
void queen(Piece*, Mode, Rank);
void king(Piece*, Mode);

char getSymbol(Color, Role);

#endif