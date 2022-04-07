#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"
#include "pieces.h"
#include "board.h"

extern Board board;

int main() {
    println("Welcome to c-chess!");
    init();
    
    Color whoseTurn = WHITE;
    board.turnNumber = 1;
    while(true) {
        printBoard();
        generateMoves();
        turn(whoseTurn);
        whoseTurn = whoseTurn == WHITE ? BLACK : WHITE;
        board.turnNumber++;
    }
}

void turn(Color color) {
    Position* piecePosition;
    Position* targetPosition;

    char* colorName = color == WHITE ? "white" : "black";
    char* capColorName = color == WHITE ? "White" : "Black";

    println("%s to move.", capColorName);

    while(true) {
        println("Select piece:");
        piecePosition = promptPosition();
        if(isEmpty(piecePosition -> x, piecePosition -> y)) {
            println("Invalid selection: No piece at %d %d", piecePosition -> x, piecePosition -> y);
            free(piecePosition);
            continue;
        }
        Piece* selectedPiece = pieceAt(piecePosition -> x, piecePosition -> y);
        if(selectedPiece -> color != color) {
            println("Invalid selection: Piece at %d %d is not a %s piece.", piecePosition -> x, piecePosition -> y, colorName);
            free(piecePosition);
            continue;
        }
        while(true) {
            println("Select target:");
            targetPosition = promptPosition();
            if(!inMoveset(selectedPiece, targetPosition)) {
                println("Invalid selection: %d %d is not part of the piece's moveset.", targetPosition -> x, targetPosition -> y);
                free(targetPosition);
                continue;
            }
            movePiece(piecePosition -> x, piecePosition -> y, targetPosition -> x, targetPosition -> y);
            break;
        }
        break;
    }
    free(piecePosition);
    free(targetPosition);
}

void generateMoves() {
    Piece **pieces = board.pieces;
    int pieceCount = board.pieceCount;
    for(int i = 0; i < pieceCount; i++) {
        generate(pieces[i]);
    }
}


void printBoard() {
    println("   ┌-----┬-----┬-----┬-----┬-----┬-----┬-----┬-----┐");
    for(int y = 7; y >= 1; y--) {
        printf(" %d ", y);
        for(int x = 0; x <= 7; x++) {
            printf("|  %s  ", pieceChar(x, y));
        }
        println("|");
        println("   |-----|-----|-----|-----|-----|-----|-----|-----|");
    }
    printf(" 0 ");
    for(int x = 0; x <= 7; x++) {
        printf("|  %s  ", pieceChar(x, 0));
    }
    println("|");
    println("   └-----┴-----┴-----┴-----┴-----┴-----┴-----┴-----┘");
    println("      0     1     2     3     4     5     6     7");
}

Position* promptPosition() {
    char coords[5]; // "x y\n\0"
    fgets(coords, 5, stdin);
    int x = strtol(&coords[0], NULL, 10);
    int y = strtol(&coords[2], NULL, 10);
    Position *position = malloc(sizeof(Position));
    position -> x = x;
    position -> y = y;
    return position;
}

char* pieceChar(int x, int y) {
    Piece* piece = pieceAt(x, y);
    if(piece) {
        static char string[13]; // 6 + 1 + 5 + 1
        char color[6];
        switch(piece -> color) {
            case BLACK: strcpy(color, GRAY); break;
            case WHITE: strcpy(color, CYAN); break;
        }
        snprintf(string, 13, "%s%c%s", color, getChessSymbol(piece -> color, piece -> role), RESET);
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