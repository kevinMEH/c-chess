#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "board.h"

Board board = {  };

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
    Piece* whiteKing = palloc();
    initPiece(whiteKing, WHITE, KING, 3, 0);
    Piece* whiteQueen = palloc();
    initPiece(whiteQueen, WHITE, QUEEN, 4, 0);
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
    Piece* blackKing = palloc();
    initPiece(blackKing, BLACK, KING, 3, 7);
    Piece* blackQueen = palloc();
    initPiece(blackQueen, BLACK, QUEEN, 4, 7);
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

void movePiece(int ox, int oy, int dx, int dy) {
    Piece *piece = pieceAt(ox, oy);
    
    if(piece -> role == PAWN) {
        if(oy - dy == 2 || dy - oy == 2) // Double move
            piece -> special = board.turnNumber;
        
        // If pawn doing en passant
        // If no piece at target but still moving diagonally
        if(!pieceAt(dx, dy) && (dx - ox == 1 || ox - dx == 1)) {
            capture(dx, oy);
        }
    }

    if(pieceAt(dx, dy)) capture(dx, dy);
    board.content[dx][dy] = piece;
    updatePosition(piece, dx, dy);
    clearSquare(ox, oy);
}

void capture(int x, int y) {
    Piece *piece = pieceAt(x, y);
    for(int i = 0; i < board.pieceCount; i++) {
        if(board.pieces[i] == piece) {
            // Shift all subsequent elements forward
            // Effectively deletes element at i
            for(int j = i; j < board.pieceCount - 1; j++) {
                board.pieces[j] = board.pieces[j + 1];
            }
            // Removes last element (Which should be same
            // as second to last element at this point.)
            board.pieces[board.pieceCount - 1] = 0;
            break;
        }
    }
    board.pieceCount--;
    free(piece);
    clearSquare(x, y);
}

void clearSquare(int x, int y) {
    board.content[x][y] = 0;
}


bool valid(int x, int y, Color color) {
    // If in bounds, if it's empty or
    // if piece there is the enemy.
    if(!inBounds(x, y)) return false;
    if(isEmpty(x, y)) return true;
    Piece *target = pieceAt(x, y);
    return enemy(color, target -> color);
}

// Check all enemy pieces and check if square is targeted.
// Primarily used for castling and King movements.
bool targeted(int x, int y, Color color) {
    Color enemy;
    switch(color) {
        case BLACK: enemy = WHITE; break;
        case WHITE: enemy = BLACK; break;
    }

    for(int i = 0; i < board.pieceCount; i++) {
        Piece *piece = board.pieces[i];
        if(piece -> color == enemy) {
            if(piece -> role == PAWN) {
                Diagonal diagonal = { { .x = 999, .y = 999 }, { .x = 999, .y = 999 } };
                diagonals(piece, &diagonal);
                if(diagonal.left.x == x && diagonal.left.y == y) return true;
                if(diagonal.right.x == x && diagonal.right.y == y) return true;
            } else {
                for(int i = 0; i < piece -> movesetSize; i++) {
                    if(piece -> moveset[i] -> x == x && piece -> moveset[i] -> y == y)
                        return true;
                }
            }
        }
    }

    return false;
}

bool inBounds(int x, int y) {
    return x >= 0 && x <= 7 && y >= 0 && x <= 7;
}

bool isEmpty(int x, int y) {
    return board.content[x][y] == 0;
}

bool enemy(Color color, Color target) {
    switch(color) {
        case BLACK: return target == WHITE;
        case WHITE: return target == BLACK;
    }
}

Piece *pieceAt(int x, int y) {
    return board.content[x][y];
}
