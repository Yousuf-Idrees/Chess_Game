#include "AI.h"
#include <algorithm>

void AI::clearTT() {
    transpositionTable.clear();
}

void AI::orderMoves(std::vector<Move>& moves, Board& board) {
    for (Move& m : moves) {
        m.score = 0;
        Piece captured = board.grid[m.r2][m.c2];
        if (captured.type != NONE) {
            int points = 0;
            switch(captured.type) {
                case QUEEN: points = 900; break;
                case ROOK: points = 500; break;
                case BISHOP: case KNIGHT: points = 300; break;
                case PAWN: points = 100; break;
                default: break;
            }
            m.score += 10 * points; 
        }
        if (m.promotion != 0) m.score += 800;
        if ((m.r2 == 3 || m.r2 == 4) && (m.c2 == 3 || m.c2 == 4)) m.score += 10;
    }
    
    std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
        return a.score > b.score;
    });
}

int AI::minimax(Board& board, int depth, int alpha, int beta, bool maximizing) {
    std::string hash = board.toFEN();
    if (transpositionTable.count(hash) && transpositionTable[hash].depth >= depth) {
        return transpositionTable[hash].score;
    }

    if (depth == 0) return board.evaluate();

    std::vector<Move> moves = board.generateLegalMoves();
    if (moves.empty()) {
        if (board.inCheck(board.turn)) return maximizing ? -99999 : 99999;
        return 0; // Stalemate
    }

    orderMoves(moves, board);

    if (maximizing) {
        int maxEval = -1000000;
        for (Move& m : moves) {
            Board copy = board;
            copy.applyMove(m);
            int eval = minimax(copy, depth - 1, alpha, beta, false);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
        }
        transpositionTable[hash] = {depth, maxEval};
        return maxEval;
    } else {
        int minEval = 1000000;
        for (Move& m : moves) {
            Board copy = board;
            copy.applyMove(m);
            int eval = minimax(copy, depth - 1, alpha, beta, true);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
        }
        transpositionTable[hash] = {depth, minEval};
        return minEval;
    }
}

Move AI::getBestMove(Board& board, int depth) {
    clearTT();
    std::vector<Move> moves = board.generateLegalMoves();
    orderMoves(moves, board);
    
    int bestVal = -1000000;
    Move bestMove = moves[0];
    
    int alpha = -1000000;
    int beta = 1000000;

    for (Move& m : moves) {
        Board copy = board;
        copy.applyMove(m);
        int moveVal = minimax(copy, depth - 1, alpha, beta, false);
        if (moveVal > bestVal) {
            bestVal = moveVal;
            bestMove = m;
        }
        alpha = std::max(alpha, bestVal);
    }
    
    return bestMove;
}
