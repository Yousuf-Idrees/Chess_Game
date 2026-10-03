#ifndef BOARD_H
#define BOARD_H

#include "Piece.h"
#include "Move.h"
#include <string>
#include <vector>

class Board {
public:
    Piece grid[8][8];
    Color turn;
    bool castlingK, castlingQ, castlingk, castlingq;
    int ep_col;
    int half_move;
    int full_move;

    Board();
    
    void setupInitial();
    void loadFEN(const std::string& fen);
    std::string toFEN() const;
    void print() const;
    
    bool makeMove(Move m);
    void applyMove(Move m);
    
    std::vector<Move> generateLegalMoves();
    std::vector<Move> generatePseudoLegalMoves();
    
    bool isSquareAttacked(int r, int c, Color attackerColor);
    bool inCheck(Color c);
    bool isCheckmate();
    bool isStalemate();
    
    int evaluate();
};

#endif
