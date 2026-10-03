#ifndef PIECE_H
#define PIECE_H

enum PieceType { NONE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING };
enum Color { WHITE, BLACK, EMPTY_COLOR };

struct Piece {
    PieceType type;
    Color color;

    Piece();
    Piece(PieceType t, Color c);
    
    char toChar() const;
    static Piece fromChar(char c);
};

#endif
