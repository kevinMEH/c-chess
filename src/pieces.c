#include <stdio.h>
#include <stdlib.h>
#include "pieces.h"

extern Board board;

void addMove(Piece *piece, int x, int y) {
    Position *position = malloc(sizeof(Position));
    position -> x = x;
    position -> y = y;
    piece -> moveset[piece -> movesetSize] = position;
    piece -> movesetSize = piece -> movesetSize + 1;
}

void updatePosition(Piece *piece, int x, int y) {
    piece -> position.x = x;
    piece -> position.y = y;
}

bool inMoveset(Piece *piece, Position *position) {
    Position **moveset = piece -> moveset;
    int movesetSize = piece -> movesetSize;
    for(int i = 0; i < movesetSize; i++) {
        Position *move = moveset[i];
        if(move -> x == position -> x && move -> y == position -> y)
            return true;
    }
    return false;
}


void generate(Piece *piece) {
    clearMoves(piece);
    switch(piece -> role) {
        case PAWN: pawn(piece); break;
        case BISHOP: bishop(piece); break;
        case KNIGHT: knight(piece); break;
        case ROOK: rook(piece); break;
        case QUEEN: queen(piece); break;
        case KING: king(piece); break;
    }
}

void clearMoves(Piece *piece) {
    Position **moveset = piece -> moveset;
    int movesetSize = piece -> movesetSize;
    for(int i = 0; i < movesetSize; i++) {
        Position *move = moveset[i];
        moveset[i] = NULL;
        free(move);
    }
    piece -> movesetSize = 0;
}

void pawn(Piece *piece) {
    int x = piece -> position.x;
    int y = piece -> position.y;
    Color color = piece -> color;
    int targetY;
    
    switch(color) {
        case BLACK: targetY = y - 1; break;
        case WHITE: targetY = y + 1; break;
    }
    
    // TODO: Make sure pawn auto promotes or else
    // the move added may not be in bounds.
    if(isEmpty(x, targetY))
        addMove(piece, x, targetY);
        
    // Left
    if(!isEmpty(x - 1, targetY) && enemy(pieceAt(x - 1, targetY) -> color, color))
        addMove(piece, x - 1, targetY);
        
    // Right
    if(!isEmpty(x + 1, targetY) && enemy(pieceAt(x + 1, targetY) -> color, color))
        addMove(piece, x + 1, targetY);
        
    // If first move
    switch(color) {
        case BLACK: {
            if(y == 6 && isEmpty(x, y - 2)) {
                addMove(piece, x, y - 2);
            }
        } break;
        case WHITE: {
            if(y == 1 && isEmpty(x, y + 2)) {
                addMove(piece, x, y + 2);
            }
        } break;
    }
    
    // En passant
    switch(color) {
        case BLACK: {
            if(y == 3) {
                // Left
                if(inBounds(x - 1, y) && !isEmpty(x - 1, y)) {
                    Piece *leftPiece = pieceAt(x - 1, y);
                    // If enemy pawn
                    if(leftPiece -> color == WHITE && leftPiece -> role == PAWN) {
                        if(board.turnNumber - leftPiece -> special == 1) addMove(piece, x - 1, y - 1);
                    }
                }
                // Right
                if(inBounds(x + 1, y) && !isEmpty(x + 1, y)) {
                    Piece *rightPiece = pieceAt(x + 1, y);
                    // If enemy pawn
                    if(rightPiece -> color == WHITE && rightPiece -> role == PAWN) {
                        // If pawn double moved last turn
                        if(board.turnNumber - rightPiece -> special == 1) addMove(piece, x + 1, y - 1);
                    }
                }
            }
        } break;
        case WHITE: {
            if(y == 4) {
                // Left
                if(inBounds(x - 1, y) && !isEmpty(x - 1, y)) {
                    Piece *leftPiece = pieceAt(x - 1, y);
                    // If enemy pawn
                    if(leftPiece -> color == BLACK && leftPiece -> role == PAWN) {
                        if(board.turnNumber - leftPiece -> special == 1) addMove(piece, x - 1, y + 1);
                    }
                }
                // Right
                if(inBounds(x + 1, y) && !isEmpty(x + 1, y)) {
                    Piece *rightPiece = pieceAt(x + 1, y);
                    // If enemy pawn
                    if(rightPiece -> color == BLACK && rightPiece -> role == PAWN) {
                        if(board.turnNumber - rightPiece -> special == 1) addMove(piece, x + 1, y + 1);
                    }
                }
            }
        } break;
    }
}

void diagonals(Piece *piece, Diagonal *diagonal) {
    int x = piece -> position.x;
    int y;
    // Compute target y position
    switch(piece -> color) {
        case BLACK: y = piece -> position.y - 1; break;
        case WHITE: y = piece -> position.y + 1; break;
    }
    // Left diagonal
    diagonal -> left.x = x - 1;
    diagonal -> left.y = y;
    // Right diagonal
    diagonal -> right.x = x + 1;
    diagonal -> right.y = y;
    // No need to check for bounds since whoever's using this
    // information should be checking it themselves anyways
}

void knight(Piece *piece) {
    int x = piece -> position.x;
    int y = piece -> position.y;
    Color color = piece -> color;
    
    // LTT
    if(valid(x - 1, y + 2, color))
        addMove(piece, x - 1, y + 2);
    // LLT
    if(valid(x - 2, y + 1, color))
        addMove(piece, x - 2, y + 1);
    // LLB
    if(valid(x - 2, y - 1, color))
        addMove(piece, x - 2, y - 1);
    // LBB
    if(valid(x - 1, y - 2, color))
        addMove(piece, x - 1, y - 2);

    // RBB
    if(valid(x + 1, y - 2, color))
        addMove(piece, x + 1, y - 2);
    // RRB
    if(valid(x + 2, y - 1, color))
        addMove(piece, x + 2, y - 1);
    // RRT
    if(valid(x + 2, y + 1, color))
        addMove(piece, x + 2, y + 1);
    // RTT
    if(valid(x + 1, y + 2, color))
        addMove(piece, x + 1, y + 2);
}

void bishop(Piece *piece) {
    // No variable shadowing in C...
    int rx = piece -> position.x;
    int ry = piece -> position.y;
    
    // Top left
    for(int x = rx - 1, y = ry + 1; x >= 0 && y <= 7; x--, y++) {
        if(valid(x, y, piece -> color))
            addMove(piece, x, y);
        else break;
    }
    // Top right
    for(int x = rx + 1, y = ry + 1; x <= 7 && y <= 7; x++, y++) {
        if(valid(x, y, piece -> color))
            addMove(piece, x, y);
        else break;
    }
    // Bottom left
    for(int x = rx - 1, y = ry - 1; x >= 0 && y >= 0; x--, y--) {
        if(valid(x, y, piece -> color))
            addMove(piece, x, y);
        else break;
    }
    // Bottom right
    for(int x = rx + 1, y = ry - 1; x <= 7 && y >= 0; x++, y--) {
        if(valid(x, y, piece -> color))
            addMove(piece, x, y);
        else break;
    }
}

void rook(Piece *piece) {
    int rx = piece -> position.x;
    int ry = piece -> position.y;
    
    // Up
    for(int y = ry + 1; y <= 7; y++) {
        if(valid(rx, y, piece -> color))
            addMove(piece, rx, y);
        else break;
    }
    // Down
    for(int y = ry - 1; y >= 0; y--) {
        if(valid(rx, y, piece -> color))
            addMove(piece, rx, y);
        else break;
    }
    // Left
    for(int x = rx - 1; x >= 0; x--) {
        if(valid(x, ry, piece -> color))
            addMove(piece, x, ry);
        else break;
    }
    // Right
    for(int x = rx + 1; x <= 7; x++) {
        if(valid(x, ry, piece -> color))
            addMove(piece, x, ry);
        else break;
    }
}

void queen(Piece *piece) {
    rook(piece);
    bishop(piece);
}

void king(Piece *piece) {
    int x = piece -> position.x;
    int y = piece -> position.y;
    Color color = piece -> color;
    
    // Top
    if(valid(x - 1, y + 1, color) && !targeted(x - 1, y + 1, color))
        addMove(piece, x - 1, y + 1);
    if(valid(x, y + 1, color) && !targeted(x, y + 1, color))
        addMove(piece, x, y + 1);
    if(valid(x + 1, y + 1, color) && !targeted(x + 1, y + 1, color))
        addMove(piece, x + 1, y + 1);
    // Middle
    if(valid(x - 1, y, color) && !targeted(x - 1, y, color))
        addMove(piece, x - 1, y);
    if(valid(x + 1, y, color) && !targeted(x + 1, y, color))
        addMove(piece, x + 1, y);
    // Bottom
    if(valid(x - 1, y - 1, color) && !targeted(x - 1, y - 1, color))
        addMove(piece, x - 1, y - 1);
    if(valid(x, y - 1, color) && !targeted(x, y - 1, color))
        addMove(piece, x, y - 1);
    if(valid(x + 1, y - 1, color) && !targeted(x + 1, y - 1, color))
        addMove(piece, x + 1, y - 1);
        
}

