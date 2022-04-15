#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "generate.h"
#include "board.h"
#include "piece.h"
#include "util.h"

extern Board board;

void game() {
    init();
    
    Color turn = WHITE;
    board.turnNumber = 1;
    
    while(true) {
        printBoard();
        generateMoves(turn);
        if(checkCheckmate(turn)) {
            println("Congratulations!");
            char* capColorName = turn == WHITE ? "Black" : "White";
            println("%s wins by checkmate!", capColorName);
            return;
        }
        if(checkStalemate(turn)) {
            println("Stalemate!");
            println("Nobody wins...");
            return;
        }
        promptMove(turn);
        turn = turn == WHITE ? BLACK : WHITE;
        board.turnNumber++;
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


void promptMove(Color color) {
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
        if(selectedPiece -> movesetSize == 0) {
            println("Piece at %d %d has no available moves.", piecePosition -> x, piecePosition -> y);
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
        snprintf(string, 13, "%s%c%s", color, getSymbol(piece -> color, piece -> role), RESET);
        return string;
    }
    return " ";
}
