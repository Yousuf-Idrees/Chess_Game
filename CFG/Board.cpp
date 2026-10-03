#include "Board.h"
#include <iostream>
#include <sstream>

Board::Board() { setupInitial(); }

void Board::setupInitial() {
    loadFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

void Board::loadFEN(const std::string& fen) {
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            grid[r][c] = Piece();
            
    std::istringstream ss(fen);
    std::string board_part, turn_part, castle_part, ep_part, half_part, full_part;
    ss >> board_part >> turn_part >> castle_part >> ep_part >> half_part >> full_part;
    
    int r = 0, c = 0;
    for (char ch : board_part) {
        if (ch == '/') { r++; c = 0; }
        else if (isdigit(ch)) { c += (ch - '0'); }
        else { grid[r][c++] = Piece::fromChar(ch); }
    }
    
    turn = (turn_part == "w") ? WHITE : BLACK;
    
    castlingK = castlingQ = castlingk = castlingq = false;
    if (castle_part != "-") {
        for (char ch : castle_part) {
            if (ch == 'K') castlingK = true;
            if (ch == 'Q') castlingQ = true;
            if (ch == 'k') castlingk = true;
            if (ch == 'q') castlingq = true;
        }
    }
    
    ep_col = -1;
    if (ep_part != "-") { ep_col = ep_part[0] - 'a'; }
    
    half_move = half_part.empty() ? 0 : std::stoi(half_part);
    full_move = full_part.empty() ? 1 : std::stoi(full_part);
}

std::string Board::toFEN() const {
    std::string fen = "";
    for (int r = 0; r < 8; ++r) {
        int empty = 0;
        for (int c = 0; c < 8; ++c) {
            if (grid[r][c].type == NONE) { empty++; }
            else {
                if (empty > 0) { fen += std::to_string(empty); empty = 0; }
                fen += grid[r][c].toChar();
            }
        }
        if (empty > 0) fen += std::to_string(empty);
        if (r < 7) fen += "/";
    }
    fen += (turn == WHITE) ? " w " : " b ";
    
    std::string castle = "";
    if (castlingK) castle += "K";
    if (castlingQ) castle += "Q";
    if (castlingk) castle += "k";
    if (castlingq) castle += "q";
    if (castle.empty()) castle = "-";
    fen += castle + " ";
    
    if (ep_col == -1) fen += "- ";
    else {
        fen += (char)('a' + ep_col);
        fen += (turn == WHITE) ? "6 " : "3 ";
    }
    
    fen += std::to_string(half_move) + " " + std::to_string(full_move);
    return fen;
}

void Board::print() const {
    std::cout << "  a b c d e f g h\n";
    for (int r = 0; r < 8; ++r) {
        std::cout << 8 - r << " ";
        for (int c = 0; c < 8; ++c) {
            std::cout << grid[r][c].toChar() << " ";
        }
        std::cout << 8 - r << "\n";
    }
    std::cout << "  a b c d e f g h\n";
}

bool Board::isSquareAttacked(int r, int c, Color attackerColor) {
    for (int ir = 0; ir < 8; ++ir) {
        for (int ic = 0; ic < 8; ++ic) {
            Piece p = grid[ir][ic];
            if (p.type != NONE && p.color == attackerColor) {
                int dr = r - ir;
                int dc = c - ic;
                if (p.type == PAWN) {
                    int dir = (p.color == WHITE) ? -1 : 1;
                    if (dr == dir && (dc == 1 || dc == -1)) return true;
                } else if (p.type == KNIGHT) {
                    if ((abs(dr) == 2 && abs(dc) == 1) || (abs(dr) == 1 && abs(dc) == 2)) return true;
                } else if (p.type == KING) {
                    if (abs(dr) <= 1 && abs(dc) <= 1) return true;
                } else {
                    if (p.type == BISHOP || p.type == QUEEN) {
                        if (abs(dr) == abs(dc)) {
                            bool clear = true;
                            int stepR = (dr > 0) ? 1 : -1;
                            int stepC = (dc > 0) ? 1 : -1;
                            for (int i = 1; i < abs(dr); ++i) {
                                if (grid[ir + i*stepR][ic + i*stepC].type != NONE) { clear = false; break; }
                            }
                            if (clear) return true;
                        }
                    }
                    if (p.type == ROOK || p.type == QUEEN) {
                        if (dr == 0 || dc == 0) {
                            bool clear = true;
                            int stepR = (dr == 0) ? 0 : ((dr > 0) ? 1 : -1);
                            int stepC = (dc == 0) ? 0 : ((dc > 0) ? 1 : -1);
                            int steps = std::max(abs(dr), abs(dc));
                            for (int i = 1; i < steps; ++i) {
                                if (grid[ir + i*stepR][ic + i*stepC].type != NONE) { clear = false; break; }
                            }
                            if (clear) return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool Board::inCheck(Color c) {
    for (int r = 0; r < 8; ++r) {
        for (int col = 0; col < 8; ++col) {
            if (grid[r][col].type == KING && grid[r][col].color == c) {
                return isSquareAttacked(r, col, c == WHITE ? BLACK : WHITE);
            }
        }
    }
    return false;
}

void Board::applyMove(Move m) {
    Piece p = grid[m.r1][m.c1];
    Piece captured = grid[m.r2][m.c2];
    
    // Handle castling move
    if (p.type == KING && abs(m.c2 - m.c1) == 2) {
        if (m.c2 == 6) { 
            grid[m.r1][5] = grid[m.r1][7];
            grid[m.r1][7] = Piece();
        } else if (m.c2 == 2) { 
            grid[m.r1][3] = grid[m.r1][0];
            grid[m.r1][0] = Piece();
        }
    }
    
    // En passant capture
    if (p.type == PAWN && m.c2 != m.c1 && captured.type == NONE) {
        grid[m.r1][m.c2] = Piece();
    }
    
    grid[m.r2][m.c2] = p;
    grid[m.r1][m.c1] = Piece();
    
    if (m.promotion != 0) {
        grid[m.r2][m.c2] = Piece::fromChar(turn == WHITE ? toupper(m.promotion) : m.promotion);
    }
    
    if (p.type == KING) {
        if (turn == WHITE) { castlingK = false; castlingQ = false; }
        else { castlingk = false; castlingq = false; }
    }
    if (p.type == ROOK) {
        if (m.r1 == 7 && m.c1 == 0) castlingQ = false;
        if (m.r1 == 7 && m.c1 == 7) castlingK = false;
        if (m.r1 == 0 && m.c1 == 0) castlingq = false;
        if (m.r1 == 0 && m.c1 == 7) castlingk = false;
    }
    
    if (p.type == PAWN && abs(m.r2 - m.r1) == 2) {
        ep_col = m.c1;
    } else {
        ep_col = -1;
    }
    
    if (p.type == PAWN || captured.type != NONE) half_move = 0;
    else half_move++;
    
    if (turn == BLACK) full_move++;
    turn = (turn == WHITE) ? BLACK : WHITE;
}

std::vector<Move> Board::generatePseudoLegalMoves() {
    std::vector<Move> moves;
    int dir = (turn == WHITE) ? -1 : 1;
    int start_row = (turn == WHITE) ? 6 : 1;
    
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece p = grid[r][c];
            if (p.type == NONE || p.color != turn) continue;
            
            if (p.type == PAWN) {
                if (r + dir >= 0 && r + dir < 8 && grid[r + dir][c].type == NONE) {
                    if (r + dir == 0 || r + dir == 7) {
                        moves.push_back(Move(r, c, r + dir, c, 'q'));
                        moves.push_back(Move(r, c, r + dir, c, 'r'));
                        moves.push_back(Move(r, c, r + dir, c, 'b'));
                        moves.push_back(Move(r, c, r + dir, c, 'n'));
                    } else {
                        moves.push_back(Move(r, c, r + dir, c));
                        if (r == start_row && grid[r + 2*dir][c].type == NONE) {
                            moves.push_back(Move(r, c, r + 2*dir, c));
                        }
                    }
                }
                for (int dc : {-1, 1}) {
                    if (c + dc >= 0 && c + dc < 8) {
                        bool capture = (grid[r + dir][c + dc].type != NONE && grid[r + dir][c + dc].color != turn);
                        bool ep = (r == (turn == WHITE ? 3 : 4) && ep_col == c + dc);
                        if (capture || ep) {
                            if (r + dir == 0 || r + dir == 7) {
                                moves.push_back(Move(r, c, r + dir, c + dc, 'q'));
                                moves.push_back(Move(r, c, r + dir, c + dc, 'r'));
                                moves.push_back(Move(r, c, r + dir, c + dc, 'b'));
                                moves.push_back(Move(r, c, r + dir, c + dc, 'n'));
                            } else {
                                moves.push_back(Move(r, c, r + dir, c + dc));
                            }
                        }
                    }
                }
            } else if (p.type == KNIGHT) {
                int dr[] = {-2, -2, -1, -1, 1, 1, 2, 2};
                int dc[] = {-1, 1, -2, 2, -2, 2, -1, 1};
                for (int i = 0; i < 8; ++i) {
                    int nr = r + dr[i], nc = c + dc[i];
                    if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                        if (grid[nr][nc].color != turn) moves.push_back(Move(r, c, nr, nc));
                    }
                }
            } else if (p.type == KING) {
                int dr[] = {-1, -1, -1, 0, 0, 1, 1, 1};
                int dc[] = {-1, 0, 1, -1, 1, -1, 0, 1};
                for (int i = 0; i < 8; ++i) {
                    int nr = r + dr[i], nc = c + dc[i];
                    if (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                        if (grid[nr][nc].color != turn) moves.push_back(Move(r, c, nr, nc));
                    }
                }
                if (turn == WHITE) {
                    if (castlingK && grid[7][5].type == NONE && grid[7][6].type == NONE &&
                        !isSquareAttacked(7, 4, BLACK) && !isSquareAttacked(7, 5, BLACK) && !isSquareAttacked(7, 6, BLACK)) {
                        moves.push_back(Move(7, 4, 7, 6));
                    }
                    if (castlingQ && grid[7][1].type == NONE && grid[7][2].type == NONE && grid[7][3].type == NONE &&
                        !isSquareAttacked(7, 4, BLACK) && !isSquareAttacked(7, 3, BLACK) && !isSquareAttacked(7, 2, BLACK)) {
                        moves.push_back(Move(7, 4, 7, 2));
                    }
                } else {
                    if (castlingk && grid[0][5].type == NONE && grid[0][6].type == NONE &&
                        !isSquareAttacked(0, 4, WHITE) && !isSquareAttacked(0, 5, WHITE) && !isSquareAttacked(0, 6, WHITE)) {
                        moves.push_back(Move(0, 4, 0, 6));
                    }
                    if (castlingq && grid[0][1].type == NONE && grid[0][2].type == NONE && grid[0][3].type == NONE &&
                        !isSquareAttacked(0, 4, WHITE) && !isSquareAttacked(0, 3, WHITE) && !isSquareAttacked(0, 2, WHITE)) {
                        moves.push_back(Move(0, 4, 0, 2));
                    }
                }
            } else {
                std::vector<std::pair<int, int>> dirs;
                if (p.type == BISHOP || p.type == QUEEN) dirs.insert(dirs.end(), {{-1,-1},{-1,1},{1,-1},{1,1}});
                if (p.type == ROOK || p.type == QUEEN) dirs.insert(dirs.end(), {{-1,0},{1,0},{0,-1},{0,1}});
                for (auto d : dirs) {
                    int nr = r + d.first, nc = c + d.second;
                    while (nr >= 0 && nr < 8 && nc >= 0 && nc < 8) {
                        if (grid[nr][nc].type == NONE) {
                            moves.push_back(Move(r, c, nr, nc));
                        } else {
                            if (grid[nr][nc].color != turn) moves.push_back(Move(r, c, nr, nc));
                            break;
                        }
                        nr += d.first; nc += d.second;
                    }
                }
            }
        }
    }
    return moves;
}

std::vector<Move> Board::generateLegalMoves() {
    std::vector<Move> pseudo = generatePseudoLegalMoves();
    std::vector<Move> legal;
    for (const Move& m : pseudo) {
        Board copy = *this;
        copy.applyMove(m);
        if (!copy.inCheck(this->turn)) {
            legal.push_back(m);
        }
    }
    return legal;
}

bool Board::makeMove(Move m) {
    std::vector<Move> legal = generateLegalMoves();
    for (const Move& l : legal) {
        if (l.r1 == m.r1 && l.c1 == m.c1 && l.r2 == m.r2 && l.c2 == m.c2) {
            if (l.promotion == m.promotion || (m.promotion == 0 && l.promotion != 0)) {
                applyMove(l.promotion ? l : m);
                return true;
            }
        }
    }
    return false;
}

bool Board::isCheckmate() {
    return inCheck(turn) && generateLegalMoves().empty();
}

bool Board::isStalemate() {
    return !inCheck(turn) && generateLegalMoves().empty();
}

int getPieceValue(PieceType t) {
    switch(t) {
        case PAWN: return 100;
        case KNIGHT: return 320;
        case BISHOP: return 330;
        case ROOK: return 500;
        case QUEEN: return 900;
        default: return 0;
    }
}

// Basic positional heuristics to make AI smarter without deep search
int getPositionalScore(PieceType t, Color c, int r, int col) {
    int score = 0;
    int rank = (c == WHITE) ? r : 7 - r; // 0 is opponent's back rank, 7 is own back rank
    
    if (t == PAWN) {
        if (rank == 1 || rank == 2) score += 30; // Advanced pawns are good
        if (col == 3 || col == 4) score += 10; // Center pawns
    } else if (t == KNIGHT) {
        if (rank < 6 && col > 1 && col < 6) score += 20; // Centralized knights
        if (rank == 7 || col == 0 || col == 7) score -= 20; // Edge knights are bad
    } else if (t == BISHOP) {
        if (rank < 6 && col > 1 && col < 6) score += 10;
    } else if (t == KING) {
        if (rank == 7 && (col < 3 || col > 4)) score += 30; // Castled/safely tucked king
    }
    return score;
}

int Board::evaluate() {
    int score = 0;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece p = grid[r][c];
            if (p.type != NONE) {
                int val = getPieceValue(p.type) + getPositionalScore(p.type, p.color, r, c);
                if (p.color == WHITE) score += val;
                else score -= val;
            }
        }
    }
    return (turn == WHITE) ? score : -score;
}
