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



Piece *pieceAt(int x, int y) {
    return board.content[x][y];
}
