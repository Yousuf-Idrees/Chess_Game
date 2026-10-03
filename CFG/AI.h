#ifndef AI_H
#define AI_H

#include "Board.h"
#include <unordered_map>
#include <string>

struct TTEntry {
    int depth;
    int score;
};

class AI {
private:
    std::unordered_map<std::string, TTEntry> transpositionTable;

public:
    void clearTT();
    void orderMoves(std::vector<Move>& moves, Board& board);
    int minimax(Board& board, int depth, int alpha, int beta, bool maximizing);
    Move getBestMove(Board& board, int depth);
};

#endif
