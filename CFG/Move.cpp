#include "Move.h"

Move::Move() : r1(0), c1(0), r2(0), c2(0), promotion(0), score(0) {}
Move::Move(int r1, int c1, int r2, int c2, char prom) 
    : r1(r1), c1(c1), r2(r2), c2(c2), promotion(prom), score(0) {}

std::string Move::toString() const {
    std::string s = "";
    s += (char)('a' + c1);
    s += (char)('8' - r1);
    s += (char)('a' + c2);
    s += (char)('8' - r2);
    if (promotion) s += promotion;
    return s;
}

Move Move::fromString(const std::string& s) {
    if (s.length() < 4) return Move();
    int c1 = s[0] - 'a';
    int r1 = '8' - s[1];
    int c2 = s[2] - 'a';
    int r2 = '8' - s[3];
    char prom = (s.length() > 4) ? s[4] : 0;
    return Move(r1, c1, r2, c2, prom);
}
