#ifndef MAIN_H
#define MAIN_H

#include "pieces.h"

#define ANSI_COLOR_GRAY "\x1b[30m" // 6
#define ANSI_COLOR_CYAN "\x1b[36m" // 6
#define ANSI_COLOR_RESET "\x1b[0m" // 5

void printBoard();
char *pieceChar(int, int);
char getChessSymbol(Color, Role);
int println(const char*, ...);

#endif