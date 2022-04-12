#include <stdlib.h>

#include "generate.h"
#include "board.h"

extern Board board;
Piece* blackKing = NULL;
Piece* whiteKing = NULL;

void init() {
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            clearSquare(i, j);
        }
    }

    Piece* whiteRookLeft = palloc();
    initPiece(whiteRookLeft, WHITE, ROOK, 0, 0);
    Piece* whiteKnightLeft = palloc();
    initPiece(whiteKnightLeft, WHITE, KNIGHT, 1, 0);
    Piece* whiteBishopLeft = palloc();
    initPiece(whiteBishopLeft, WHITE, BISHOP, 2, 0);
    Piece* whiteQueen = palloc();
    initPiece(whiteQueen, WHITE, QUEEN, 3, 0);
    Piece* whiteBishopRight = palloc();
    initPiece(whiteBishopRight, WHITE, BISHOP, 5, 0);
    Piece* whiteKnightRight = palloc();
    initPiece(whiteKnightRight, WHITE, KNIGHT, 6, 0);
    Piece* whiteRookRight = palloc();
    initPiece(whiteRookRight, WHITE, ROOK, 7, 0);

    for(int i = 0; i <= 7; i++) {
        Piece* pawn = palloc();
        initPiece(pawn, WHITE, PAWN, i, 1);
    }
    
    Piece* blackRookLeft = palloc();
    initPiece(blackRookLeft, BLACK, ROOK, 0, 7);
    Piece* blackKnightLeft = palloc();
    initPiece(blackKnightLeft, BLACK, KNIGHT, 1, 7);
    Piece* blackBishopLeft = palloc();
    initPiece(blackBishopLeft, BLACK, BISHOP, 2, 7);
    Piece* blackQueen = palloc();
    initPiece(blackQueen, BLACK, QUEEN, 3, 7);
    Piece* blackBishopRight = palloc();
    initPiece(blackBishopRight, BLACK, BISHOP, 5, 7);
    Piece* blackKnightRight = palloc();
    initPiece(blackKnightRight, BLACK, KNIGHT, 6, 7);
    Piece* blackRookRight = palloc();
    initPiece(blackRookRight, BLACK, ROOK, 7, 7);

    for(int i = 0; i <= 7; i++) {
        Piece* pawn = palloc();
        initPiece(pawn, BLACK, PAWN, i, 6);
    }

    whiteKing = palloc();
    initPiece(whiteKing, WHITE, KING, 4, 0);

    blackKing = palloc();
    initPiece(blackKing, BLACK, KING, 4, 7);
}

Piece* palloc() {
    return malloc(sizeof(Piece));
}

void initPiece(Piece* piece, Color color, Role role, int x, int y) {
    piece -> color = color;
    piece -> role = role;
    piece -> position.x = x;
    piece -> position.y = y;
    if(role == PAWN || role == ROOK || role == KING) {
        piece -> special = false;
    }
    board.content[x][y] = piece;
    board.pieces[board.pieceCount] = piece;
    board.pieceCount++;
}



void generateMoves(Color color) {
    Piece **pieces = board.pieces;
    int pieceCount = board.pieceCount;
    
    // Enemy pieces: Also construct pins
    for(int i = 0; i < pieceCount; i++) {
        Piece* piece = pieces[i];
        if(piece -> color != color && piece -> role != KING) {
            generate(piece, CONSTRUCT, ALL);
        }
    }

    generate(blackKing, REGULAR, ALL);
    generate(whiteKing, REGULAR, ALL);
    
    Mode mode;
    

    for(int i = 0; i < board.pieceCount; i++) {
        Piece* piece = board.pieces[i];
        if(piece -> color == color)
            generate(piece, mode, ALL);
    }

}

void clear(Piece** array, int size) {
    for(int i = 0; i < size; i++) {
        array[i] = NULL;
    }
}