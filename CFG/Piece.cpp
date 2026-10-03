#include "Piece.h"
#include <cctype>

Piece::Piece() : type(NONE), color(EMPTY_COLOR) {}
Piece::Piece(PieceType t, Color c) : type(t), color(c) {}

char Piece::toChar() const {
    char c = '.';
    switch (type) {
        case PAWN: c = 'p'; break;
        case KNIGHT: c = 'n'; break;
        case BISHOP: c = 'b'; break;
        case ROOK: c = 'r'; break;
        case QUEEN: c = 'q'; break;
        case KING: c = 'k'; break;
        default: return '.';
    }
    return (color == WHITE) ? std::toupper(c) : c;
}

Piece Piece::fromChar(char c) {
    Color col = std::isupper(c) ? WHITE : BLACK;
    c = std::tolower(c);
    PieceType t = NONE;
    switch (c) {
        case 'p': t = PAWN; break;
        case 'n': t = KNIGHT; break;
        case 'b': t = BISHOP; break;
        case 'r': t = ROOK; break;
        case 'q': t = QUEEN; break;
        case 'k': t = KING; break;
    }
    return Piece(t, t == NONE ? EMPTY_COLOR : col);
}
