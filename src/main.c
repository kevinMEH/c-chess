#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "main.h"
#include "pieces.h"
#include "board.h"

extern Board board;

int main() {
    println("Welcome to c-chess!");
    init();
    
    printBoard();
}

void printBoard() {
    println("┌-----┬-----┬-----┬-----┬-----┬-----┬-----┬-----┐");
    for(int y = 7; y >= 1; y--) {
        for(int x = 0; x <= 7; x++) {
            printf("|  %s  ", pieceChar(x, y));
        }
        println("|");
        println("|-----|-----|-----|-----|-----|-----|-----|-----|");
    }
    for(int x = 0; x <= 7; x++) {
        printf("|  %s  ", pieceChar(x, 0));
    }
    println("|");
    println("└-----┴-----┴-----┴-----┴-----┴-----┴-----┴-----┘");
}

char* pieceChar(int x, int y) {
    Piece* piece = pieceAt(x, y);
    if(piece) {
        static char string[13]; // 6 + 1 + 5 + 1
        char color[6];
        switch(piece -> color) {
            case BLACK: strcpy(color, ANSI_COLOR_GRAY); break;
            case WHITE: strcpy(color, ANSI_COLOR_CYAN); break;
        }
        snprintf(string, 13, "%s%c%s", color, getChessSymbol(piece -> color, piece -> role), ANSI_COLOR_RESET);
        return string;
    }
    return " ";
}

char getChessSymbol(Color color, Role role) {
    switch(color) {
        case BLACK: {
            switch(role) {
                case PAWN: return 'p';
                case KNIGHT: return 'n';
                case BISHOP: return 'b';
                case ROOK: return 'r';
                case QUEEN: return 'q';
                case KING: return 'k';
            }
        }
        case WHITE: {
            switch(role) {
                case PAWN: return 'P';
                case KNIGHT: return 'N';
                case BISHOP: return 'B';
                case ROOK: return 'R';
                case QUEEN: return 'Q';
                case KING: return 'K';
            }
        }
    }
}

int println(const char *format, ...) {
    va_list arg;
    int done;
    
    va_start(arg, format);
    done = vfprintf(stdout, format, arg);
    va_end(arg);
    puts("");
    
    return ++done;
}