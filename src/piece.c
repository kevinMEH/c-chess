#include <stdio.h>
#include <stdlib.h>
#include "piece.h"
#include "board.h"

extern Board board;
Pin* pins[8];
int numPins;
Check* checks[2];
int numChecks;

void addMove(Piece *piece, int x, int y) {
    Position *position = malloc(sizeof(Position));
    position -> x = x;
    position -> y = y;
    piece -> moveset[piece -> movesetSize] = position;
    piece -> movesetSize = piece -> movesetSize + 1;
}

void addCheck(Piece* piece, Position** interPos, int size) {
    Check* check = malloc(sizeof(Check));
    check -> attacker = piece;
    check -> intermediatePositions = interPos;
    check -> positionsSize = size;
    checks[numChecks] = check;
    numChecks++;
}

void addCheckNoInter(Piece* piece) {
    Check* check = malloc(sizeof(Check));
    check -> attacker = piece;
    check -> intermediatePositions = NULL;
    check -> positionsSize = 0;
    checks[numChecks] = check;
    numChecks++;
}

void addPin(Piece* attacker, Piece* pinned, Rank rank) {
    Pin* pin = malloc(sizeof(Pin));
    pin -> attacker = attacker;
    pin -> pinned = pinned;
    pin -> rank = rank;
    pins[numPins] = pin;
    numPins++;
}

void clearChecksAndPins() {
    for(int i = 0; i < numPins; i++) {
        Pin* pin = pins[i];
        free(pin);
    }
    numPins = 0;
    
    for(int i = 0; i < numChecks; i++) {
        Check* check = checks[i];
        Position** interPos = check -> intermediatePositions;
        int posSize = check -> positionsSize;
        for(int i = 0; i < posSize; i++) {
            free(interPos[i]);
        }
        free(interPos);
        free(check);
    }
    numChecks = 0;
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

bool valid(int x, int y, Color color) {
    // If in bounds, if it's empty or
    // if piece there is the enemy.
    if(!inBounds(x, y)) return false;
    if(isEmpty(x, y)) return true;
    Piece *target = pieceAt(x, y);
    return enemy(target, color);
}

Validity conValid(int x, int y, Color color) {
    if(!inBounds(x, y)) return INVALID;
    if(isEmpty(x, y)) return VALID;
    Piece* target = pieceAt(x, y);
    if(!enemy(target, color)) return INVALID;
    if(target -> role == KING) return VCHECK;
    return VALID;
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
    return x >= 0 && x <= 7 && y >= 0 && y <= 7;
}

bool isEmpty(int x, int y) {
    return board.content[x][y] == 0;
}

bool enemy(Piece* enemy, Color friendly) {
    return enemy -> color != friendly;
}

bool isKing(Piece* piece) {
    return piece -> role == KING;
}

bool enemyKing(Piece* piece, Color friendly) {
    return enemy(piece, friendly) && isKing(piece);
}

bool posBlocksCheck(int x, int y) {
    Check* check = checks[0];
    Position** interPos = check -> intermediatePositions;
    int posSize = check -> positionsSize;
    for(int i = 0; i < posSize; i++) {
        Position* position = interPos[i];
        if(x == position -> x && y == position -> y)
            return true;
    }
    return false;
}

/* 
    Generate:
    
    Mode: CONSTRUCT -> Construct pins and checkmates
            REGULAR -> Regularly generate
            CHECK -> 

    Pinned pieces -> Pieces that are targeted by a ranged enemy piece, and has the king along it's targetted file / diagonal.

    Searching for pins -> Piece goes through entire file / diagonal until:
        1. Piece encounters enemy king. It's a check, moved to activeChecks.
        2. Piece encounters enemy piece. Keeps going through entire rank:
            1. If it encounters a king: Pinning the piece. Move to pins.
            2. If it encounters another piece, self or enemy: Guaranteed to not be a pin. Break out of loop.
        3. Piece encounters self piece. Guaranteed to not be a pin. Break out of loop.
*/
void generate(Piece *piece, Mode mode, Rank rank) {
    switch(piece -> role) {
        case PAWN: pawn(piece, mode, rank); break;
        case BISHOP: bishop(piece, mode, rank); break;
        case KNIGHT: knight(piece, mode, rank); break;
        case ROOK: rook(piece, mode, rank); break;
        case QUEEN: queen(piece, mode, rank); break;
        case KING: king(piece, mode); break;
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

void clearAll(Piece** pieces, int piecesSize) {
    for(int i = 0; i < piecesSize; i++) {
        Piece* piece = pieces[i];
        clearMoves(piece);
    }
}

void checkModeAdd(Piece* piece, int x, int y, Mode mode) {
    // If king not in check, add
    // If king is in check, but piece killing attacker: Add
    // If king is in check, but piece blocks check: Add
    if(mode != CHECK) {
        addMove(piece, x, y);
    } else if(posBlocksCheck(x, y)) {
        addMove(piece, x, y);
    } else if(!isEmpty(x, y) && pieceAt(x, y) == checks[0] -> attacker) {
        addMove(piece, x, y);
    }
}

void pawn(Piece *piece, Mode mode, Rank rank) {
    int x = piece -> position.x;
    int y = piece -> position.y;
    Color color = piece -> color;
    int targetY;
    
    switch(color) {
        case BLACK: targetY = y - 1; break;
        case WHITE: targetY = y + 1; break;
    }
    
    // Can possibly move forward if pinned to column or all moves avail
    bool allOrCol = rank == ALL || rank == COLUMN;
    
    // Move forward
    // If is empty and ALL or pinned to COLUMN
    if(isEmpty(x, targetY) && allOrCol) {
        // If king not in check, add
        // If in check: Does piece block the check? If yes, add
        if(mode != CHECK) {
            addMove(piece, x, targetY);
        } else if(posBlocksCheck(x, targetY)) {
            addMove(piece, x, targetY);
        }
    }
        
    // If first move
    switch(color) {
        case BLACK: {
            // If on home row, target is empty and ALL or pinned to COLUMN
            if(y == 6 && isEmpty(x, y - 2) && allOrCol) {
                // If king not in check, add
                // If king is in check, but move blocks check: Add
                if(mode != CHECK) {
                    addMove(piece, x, y - 2);
                } else if(posBlocksCheck(x, y - 2)) {
                    addMove(piece, x, y - 2);
                }
            }
        } break;
        case WHITE: {
            // If on home row, target is empty and ALL or pinned to COLUMN
            if(y == 1 && isEmpty(x, y + 2) && allOrCol) {
                // If king not in check, add
                // If king is in check, but move blocks check: Add
                if(mode != CHECK) {
                    addMove(piece, x, y + 2);
                } else if(posBlocksCheck(x, y + 2)) {
                    addMove(piece, x, y + 2);
                }
            }
        } break;
    }
        
    // Left
    // If in bounds, isn't empty, and is enemy
    if(inBounds(x - 1, targetY) && !isEmpty(x - 1, targetY)
    && enemy(pieceAt(x - 1, targetY), color)) {

        // If not pinned or is pinned and along diagonal
        if(rank == ALL || ( color == WHITE && rank == TL_DIAGONAL )
        || ( color == BLACK && rank == TR_DIAGONAL )) {
            
            // If king not in check
            // If king is in check, but pawn is killing attacker

            // If king is in check, pawn cannot take AND block check
            // (Because the enemy piece prev there is already blocking it,
            // but our king is still in check) so no need to check for blocks
            if(mode != CHECK) {
                addMove(piece, x - 1, targetY);
            } else if(pieceAt(x - 1, targetY) == checks[0] -> attacker) {
                addMove(piece, x - 1, targetY);
            }
        }
    }
        
    // Right
    if(inBounds(x + 1, targetY) && !isEmpty(x + 1, targetY)
    && enemy(pieceAt(x + 1, targetY), color)) {

        if(rank == ALL || ( color == WHITE && rank == TR_DIAGONAL )
        || ( color == BLACK && rank == TL_DIAGONAL )) {
            
            if(mode != CHECK) {
                addMove(piece, x + 1, targetY);
            } else if(pieceAt(x + 1, targetY) == checks[0] -> attacker) {
                addMove(piece, x + 1, targetY);
            }
        }
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
                        // If pawn double moved last turn
                        if(board.turnNumber - leftPiece -> special == 1) {
                            // Valid En Passant!
                            // If free to move or pinned along TR_DIAGONAL
                            if( rank == ALL || rank == TR_DIAGONAL ) {
                                checkModeAdd(piece, x - 1, y - 1, mode);
                            }
                        }
                    }
                }
                // Right
                if(inBounds(x + 1, y) && !isEmpty(x + 1, y)) {
                    Piece *rightPiece = pieceAt(x + 1, y);
                    // If enemy pawn
                    if(rightPiece -> color == WHITE && rightPiece -> role == PAWN) {
                        // If pawn double moved last turn
                        if(board.turnNumber - rightPiece -> special == 1) {
                            // Valid En Passsant!
                            // If free to move or pinned along TL_DIAGONAL
                            if( rank == ALL || rank == TL_DIAGONAL ) {
                                checkModeAdd(piece, x + 1, y - 1, mode);
                            }
                        }
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
                        // If pawn double moved last turn
                        if(board.turnNumber - leftPiece -> special == 1) {
                            // Valid En passant!
                            // If free to move or pinned along TL_DIAGONAL
                            if( rank == ALL || rank == TL_DIAGONAL ) {
                                checkModeAdd(piece, x - 1, y + 1, mode);
                            }
                        }
                    }
                }
                // Right
                if(inBounds(x + 1, y) && !isEmpty(x + 1, y)) {
                    Piece *rightPiece = pieceAt(x + 1, y);
                    // If enemy pawn
                    if(rightPiece -> color == BLACK && rightPiece -> role == PAWN) {
                        // If pawn double moved last turn
                        if(board.turnNumber - rightPiece -> special == 1) {
                            // Valid En passant!
                            // If free to move or pinned along TR_DIAGONAL
                            if( rank == ALL || rank == TR_DIAGONAL ) {
                                checkModeAdd(piece, x + 1, y + 1, mode);
                            }
                        }
                    }
                }
            }
        } break;
    }

}

Role promptPromotion() {
    while(true) {
        printf("Select which piece to promote your pawn into: N | B | R | Q\n");
        char response[3]; // c\n\0
        fgets(response, 3, stdin);
        
        switch(response[0]) {
            case 'N': return KNIGHT;
            case 'B': return BISHOP;
            case 'R': return ROOK;
            case 'Q': return QUEEN;
        }
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

void validCheckModeAdd(Piece* piece, int x, int y, Mode mode) {
    if(valid(x, y, piece -> color))
        checkModeAdd(piece, x, y, mode);
}

void conAddValidityNoInter(Piece* piece, int x, int y, Color color) {
    Validity validity = conValid(x, y, color);
    if(validity == INVALID) return;
    addMove(piece, x, y);
    if(validity == VCHECK) addCheckNoInter(piece);
}

void knight(Piece *piece, Mode mode, Rank rank) {
    int x = piece -> position.x;
    int y = piece -> position.y;
    Color color = piece -> color;
    
    if(mode == CONSTRUCT) {
        conAddValidityNoInter(piece, x - 1, y + 2, color);
        conAddValidityNoInter(piece, x - 2, y + 1, color);
        conAddValidityNoInter(piece, x - 2, y - 1, color);
        conAddValidityNoInter(piece, x - 1, y - 2, color);
        conAddValidityNoInter(piece, x + 1, y - 2, color);
        conAddValidityNoInter(piece, x + 2, y - 1, color);
        conAddValidityNoInter(piece, x + 2, y + 1, color);
        conAddValidityNoInter(piece, x + 1, y + 2, color);
    } else {
        // If pinned, can't move
        if(rank != ALL) return;
        validCheckModeAdd(piece, x - 1, y + 2, mode);
        validCheckModeAdd(piece, x - 2, y + 1, mode);
        validCheckModeAdd(piece, x - 2, y - 1, mode);
        validCheckModeAdd(piece, x - 1, y - 2, mode);
        validCheckModeAdd(piece, x + 1, y - 2, mode);
        validCheckModeAdd(piece, x + 2, y - 1, mode);
        validCheckModeAdd(piece, x + 2, y + 1, mode);
        validCheckModeAdd(piece, x + 1, y + 2, mode);
    }
    
}

void freeInterPos(Position** interPos, int posSize) {
    for(int i = 0; i < posSize; i++) {
        Position* position = interPos[i];
        free(position);
    }
    free(interPos);
}

// Returns whether to break or not
ConResult slidingValidateConstructBody(Piece* piece, int x, int y, bool* firstPassP, Piece** passedPieceP, Position** interPos, int* index, Rank rank) {
    Color color = piece -> color;
    if(isEmpty(x, y) && *firstPassP) {
        addMove(piece, x, y);
        Position* position = malloc(sizeof(Position));
        position -> x = x;
        position -> y = y;
        interPos[*index] = position;
        *index = *index + 1;
    } else if(*firstPassP) {
        // Not empty, but first pass: Blocked by something
        *firstPassP = false;
        Piece* targetPiece = pieceAt(x, y);
        // Blocked by friendly: Break, can't pin anything
        if(enemy(targetPiece, color)) {
            addMove(piece, x, y);
            // Blocked by king: Checking king
            // Else other enemy piece, move on
            if(isKing(targetPiece)) {
                addCheck(piece, interPos, *index);
                return CHECKED;
            } else {
                *passedPieceP = targetPiece;
            }
        } else return BREAK;
    } else if(isEmpty(x, y)) {
        return OTHER;
    } else {
        // Not empty, but passed an enemy piece before:
        // Is targeted piece enemy king? If yes, pinned!
        if(enemyKing(pieceAt(x, y), color)) {
            addPin(piece, *passedPieceP, rank);
        }
        return BREAK;
    }
    return OTHER;
}

void bishop(Piece *piece, Mode mode, Rank rank) {
    int rx = piece -> position.x;
    int ry = piece -> position.y;
    Color color = piece -> color;
    
    bool construct = mode == CONSTRUCT;
    
    // Top left
    if(construct) {
        bool firstPass = true;
        bool checked = false;
        Piece* passedPiece;
        Position** interPos = malloc(7 * sizeof(Position*));
        int index = 0;

        for(int x = rx - 1, y = ry + 1; x >= 0 && y <= 7; x--, y++) {
            ConResult result = slidingValidateConstructBody
                (piece, x, y, &firstPass, &passedPiece, 
                interPos, &index, TL_DIAGONAL);
            if(result == CHECKED) checked = true;
            if(result == BREAK || result == CHECKED) break;
        }
        
        if(!checked) freeInterPos(interPos, index);
    } else {
        if(rank == ALL || rank == TL_DIAGONAL) {
            for(int x = rx - 1, y = ry + 1; x >= 0 && y <= 7; x--, y++) {
                if(valid(x, y, color)) {
                    checkModeAdd(piece, x, y, mode);
                } else break;
            }
        }
    }
    // Top right
    if(construct) {
        bool firstPass = true;
        bool checked = false;
        Piece* passedPiece;
        Position** interPos = malloc(7 * sizeof(Position*));
        int index = 0;
        
        for(int x = rx + 1, y = ry + 1; x <= 7 && y <= 7; x++, y++) {
            ConResult result = slidingValidateConstructBody
                (piece, x, y, &firstPass, &passedPiece, 
                interPos, &index, TR_DIAGONAL);
            if(result == CHECKED) checked = true;
            if(result == BREAK || result == CHECKED) break;
        }
        
        if(!checked) freeInterPos(interPos, index);
    } else {
        if(rank == ALL || rank == TR_DIAGONAL) {
            for(int x = rx + 1, y = ry + 1; x <= 7 && y <= 7; x++, y++) {
                if(valid(x, y, color))
                    checkModeAdd(piece, x, y, mode);
                else break;
            }
        }
    }
    // Bottom left
    if(construct) {
        bool firstPass = true;
        bool checked = false;
        Piece* passedPiece;
        Position** interPos = malloc(7 * sizeof(Position*));
        int index = 0;

        for(int x = rx - 1, y = ry - 1; x >= 0 && y >= 0; x--, y--) {
            ConResult result = slidingValidateConstructBody
                (piece, x, y, &firstPass, &passedPiece, 
                interPos, &index, TR_DIAGONAL);
            if(result == CHECKED) checked = true;
            if(result == BREAK || result == CHECKED) break;
        }
        
        if(!checked) freeInterPos(interPos, index);
    } else {
        if(rank == ALL || rank == TR_DIAGONAL) {
            for(int x = rx - 1, y = ry - 1; x >= 0 && y >= 0; x--, y--) {
                if(valid(x, y, color))
                    checkModeAdd(piece, x, y, mode);
                else break;
            }
        }
    }
    // Bottom right
    if(construct) {
        bool firstPass = true;
        bool checked = false;
        Piece* passedPiece;
        Position** interPos = malloc(7 * sizeof(Position*));
        int index = 0;
        
        for(int x = rx + 1, y = ry - 1; x <= 7 && y >= 0; x++, y--) {
            ConResult result = slidingValidateConstructBody
                (piece, x, y, &firstPass, &passedPiece, 
                interPos, &index, TL_DIAGONAL);
            if(result == CHECKED) checked = true;
            if(result == BREAK || result == CHECKED) break;
        }
        
        if(!checked) freeInterPos(interPos, index);
    } else {
        if(rank == ALL || rank == TL_DIAGONAL) {
            for(int x = rx + 1, y = ry - 1; x <= 7 && y >= 0; x++, y--) {
                if(valid(x, y, color))
                    checkModeAdd(piece, x, y, mode);
                else break;
            }
        }
    }
}

void rook(Piece *piece, Mode mode, Rank rank) {
    int rx = piece -> position.x;
    int ry = piece -> position.y;
    Color color = piece -> color;
    
    bool construct = mode == CONSTRUCT;
    
    if(construct) {
        // If true to seperate out scopes
        // Up
        if(true) {
            bool firstPass = true;
            bool checked = false;
            Piece* passedPiece;
            Position** interPos = malloc(7 * sizeof(Position*));
            int index = 0;

            for(int y = ry + 1; y <= 7; y++) {
                ConResult result = slidingValidateConstructBody
                    (piece, rx, y, &firstPass, &passedPiece,
                    interPos, &index, COLUMN);
                if(result == CHECKED) checked = true;
                if(result == BREAK || result == CHECKED) break;
            }
            
            if(!checked) freeInterPos(interPos, index);
        }
        
        // Down
        if(true) {
            bool firstPass = true;
            bool checked = false;
            Piece* passedPiece;
            Position** interPos = malloc(7 * sizeof(Position*));
            int index = 0;

            for(int y = ry - 1; y >= 0; y--) {
                ConResult result = slidingValidateConstructBody
                    (piece, rx, y, &firstPass, &passedPiece,
                    interPos, &index, COLUMN);
                if(result == CHECKED) checked = true;
                if(result == BREAK || result == CHECKED) break;
            }
            
            if(!checked) freeInterPos(interPos, index);
        }
    } else {
        if(rank == ALL || rank == COLUMN) {
            for(int y = ry + 1; y <= 7; y++) {
                if(valid(rx, y, color))
                    checkModeAdd(piece, rx, y, mode);
                else break;
            }

            for(int y = ry - 1; y >= 0; y--) {
                if(valid(rx, y, color))
                    checkModeAdd(piece, rx, y, mode);
                else break;
            }
        }
    }

    if(construct) {
        // Left
        if(true) {
            bool firstPass = true;
            bool checked = false;
            Piece* passedPiece;
            Position** interPos = malloc(7 * sizeof(Position*));
            int index = 0;

            for(int x = rx - 1; x >= 0; x--) {
                ConResult result = slidingValidateConstructBody
                    (piece, x, ry, &firstPass, &passedPiece,
                    interPos, &index, ROW);
                if(result == CHECKED) checked = true;
                if(result == BREAK || result == CHECKED) break;
            }
            
            if(!checked) freeInterPos(interPos, index);
        }

        // Right
        if(true) {
            bool firstPass = true;
            bool checked = false;
            Piece* passedPiece;
            Position** interPos = malloc(7 * sizeof(Position*));
            int index = 0;

            for(int x = rx + 1; x <= 7; x++) {
                ConResult result = slidingValidateConstructBody
                    (piece, x, ry, &firstPass, &passedPiece,
                    interPos, &index, ROW);
                if(result == CHECKED)  checked = true;
                if(result == BREAK || result == CHECKED) break;
            }
            
            if(!checked) freeInterPos(interPos, index);
        }

    } else {
        if(rank == ALL || rank == ROW) {
            for(int x = rx - 1; x >= 0; x--) {
                if(valid(x, ry, color))
                    checkModeAdd(piece, x, ry, mode);
                else break;
            }

            for(int x = rx + 1; x <= 7; x++) {
                if(valid(x, ry, color))
                    checkModeAdd(piece, x, ry, mode);
                else break;
            }
        }
    }
}

void queen(Piece *piece, Mode mode, Rank rank) {
    rook(piece, mode, rank);
    bishop(piece, mode, rank);
}

void king(Piece *piece, Mode mode) {
    int x = piece -> position.x;
    int y = piece -> position.y;
    Color color = piece -> color;
    bool construct = mode == CONSTRUCT;
    
    if(construct) {
        conAddValidityNoInter(piece, x - 1, y + 1, color);
        conAddValidityNoInter(piece, x, y + 1, color);
        conAddValidityNoInter(piece, x + 1, y + 1, color);
        conAddValidityNoInter(piece, x - 1, y, color);
        conAddValidityNoInter(piece, x + 1, y, color);
        conAddValidityNoInter(piece, x - 1, y - 1, color);
        conAddValidityNoInter(piece, x, y - 1, color);
        conAddValidityNoInter(piece, x + 1, y - 1, color);
    } else {
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
        
        
    // Castling
    // King has not moved and is not in check.
    if(piece -> special == 0 && !targeted(x, y, color)) {
        Piece *leftRook = pieceAt(0, y);
        Piece *rightRook = pieceAt(7, y);
        
        if(leftRook) {
            // Rook has not moved
            if(leftRook -> role == ROOK && leftRook -> special == 0) {
                // Intermediate squares not in check.
                if(isEmpty(1, y) && !targeted(1, y, color)
                && isEmpty(2, y) && !targeted(2, y, color)
                && isEmpty(3, y) && !targeted(3, y, color)) {
                    addMove(piece, 2, y);
                }
            }
        }
        
        if(rightRook) {
            // Rook has not moved
            if(rightRook -> role == ROOK && rightRook -> special == 0) {
                // Intermediate squares not in check.
                if(isEmpty(5, y) && !targeted(5, y, color)
                && isEmpty(6, y) && !targeted(6, y, color)) {
                    addMove(piece, 6, y);
                }
            }
        }
    }
}


char getSymbol(Color color, Role role) {
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