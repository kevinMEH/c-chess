#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "board.h"

Board board = {  };

void movePiece(int ox, int oy, int dx, int dy) {
    Piece *piece = pieceAt(ox, oy);
    
    // Cannot castle if King or Rook moves
    if(piece -> role == ROOK || piece -> role == KING)
        piece -> special = 1;
    
    if(piece -> role == PAWN) {
        if(oy - dy == 2 || dy - oy == 2) // Double move
            piece -> special = board.turnNumber;
        
        // If pawn doing en passant
        // If no piece at target but still moving diagonally
        if(!pieceAt(dx, dy) && (dx - ox == 1 || ox - dx == 1)) {
            capture(dx, oy);
        }
    }
    
    if(piece -> role == KING) {
        // Moving rooks
        // If king is castling right
        if(ox - dx == 2) {
            movePiece(0, oy, 3, oy);
        } else // if king is castling left
        if(dx - ox == 2) {
            movePiece(7, oy, 5, oy);
        }
    }

    if(pieceAt(dx, dy)) capture(dx, dy);
    board.content[dx][dy] = piece;
    updatePosition(piece, dx, dy);
    clearSquare(ox, oy);
    
    if(piece -> role == PAWN && (piece -> position.y == 7 || piece -> position.y == 0))
        piece -> role = promptPromotion();
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
