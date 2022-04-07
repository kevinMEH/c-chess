#ifndef MAIN_H
#define MAIN_H

#include "pieces.h"

#define GRAY "\x1b[30m" // 6
#define CYAN "\x1b[36m" // 6
#define RESET "\x1b[0m" // 5

void printBoard();
void turn(Color);
void generateMoves();
Position *promptPosition();
char *pieceChar(int, int);
char getChessSymbol(Color, Role);
int println(const char*, ...);

#endif