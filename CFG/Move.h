#ifndef MOVE_H
#define MOVE_H

#include <string>

struct Move {
    int r1, c1;
    int r2, c2;
    char promotion; // 'q', 'r', 'b', 'n' or 0
    int score;      // For Greedy Move Ordering

    Move();
    Move(int r1, int c1, int r2, int c2, char prom = 0);

    std::string toString() const;
    static Move fromString(const std::string& s);
};

#endif
