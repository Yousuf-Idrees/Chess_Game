#include "Game.h"
#include "AI.h"
#include <iostream>
#include <thread>
#include <chrono>

Game::Game() : mode(0) {}

void Game::newGame(int m, int time_minutes) {
    board.setupInitial();
    timer.start(time_minutes * 60 * 1000LL);
    mode = m;
    history.clear();
    redo_stack.clear();
    history.push_back(board.toFEN());
}

void Game::saveToSave(const std::string& file) {
    timer.update(board.turn == WHITE);
    GameStateData d;
    d.mode = mode;
    d.white_time = timer.getWhiteTime();
    d.black_time = timer.getBlackTime();
    d.last_timestamp = timer.getLastTimestamp();
    d.current_fen = board.toFEN();
    d.history = history;
    d.redo_stack = redo_stack;
    SaveLoad::save(file, d);
}

void Game::loadFromSave(const std::string& file) {
    GameStateData d;
    if (SaveLoad::load(file, d)) {
        mode = d.mode;
        timer.setTimes(d.white_time, d.black_time, d.last_timestamp);
        board.loadFEN(d.current_fen);
        history = d.history;
        redo_stack = d.redo_stack;
    }
}

std::string Game::getStatusString() {
    if (timer.isTimeUp(true)) return "black_won_time";
    if (timer.isTimeUp(false)) return "white_won_time";
    if (board.isCheckmate()) return board.turn == WHITE ? "black_won_mate" : "white_won_mate";
    if (board.isStalemate()) return "draw_stalemate";
    if (board.inCheck(board.turn)) return "check";
    return "active";
}

void Game::printStateJSON() {
    std::cout << "{\n";
    std::cout << "  \"fen\": \"" << board.toFEN() << "\",\n";
    std::cout << "  \"turn\": \"" << (board.turn == WHITE ? "w" : "b") << "\",\n";
    std::cout << "  \"white_time\": " << timer.getWhiteTime() / 1000 << ",\n";
    std::cout << "  \"black_time\": " << timer.getBlackTime() / 1000 << ",\n";
    std::cout << "  \"status\": \"" << getStatusString() << "\"\n";
    std::cout << "}\n";
}

void Game::handleGuiInit(int m, int time_min) {
    newGame(m, time_min);
    saveToSave("save.dat");
    printStateJSON();
}

void Game::handleGuiMove(const std::string& move_str) {
    loadFromSave("save.dat");
    Move m = Move::fromString(move_str);
    timer.update(board.turn == WHITE);
    
    if (getStatusString() == "active" || getStatusString() == "check") {
        if (board.makeMove(m)) {
            history.push_back(board.toFEN());
            redo_stack.clear();
        }
    }
    saveToSave("save.dat");
    printStateJSON();
}

void Game::handleGuiAiMove() {
    loadFromSave("save.dat");
    timer.update(board.turn == WHITE);
    if ((getStatusString() == "active" || getStatusString() == "check") && board.turn == BLACK && mode == 1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1500)); // Realistic thinking delay
        AI ai;
        Move m = ai.getBestMove(board, 4);
        if (board.makeMove(m)) {
            history.push_back(board.toFEN());
            redo_stack.clear();
        }
    }
    saveToSave("save.dat");
    printStateJSON();
}

void Game::handleGuiUndo() {
    loadFromSave("save.dat");
    if (history.size() > 1) {
        redo_stack.push_back(history.back());
        history.pop_back();
        if (mode == 1 && history.size() > 1) {
            redo_stack.push_back(history.back());
            history.pop_back();
        }
        board.loadFEN(history.back());
    }
    saveToSave("save.dat");
    printStateJSON();
}

void Game::handleGuiRedo() {
    loadFromSave("save.dat");
    if (!redo_stack.empty()) {
        history.push_back(redo_stack.back());
        redo_stack.pop_back();
        if (mode == 1 && !redo_stack.empty()) {
            history.push_back(redo_stack.back());
            redo_stack.pop_back();
        }
        board.loadFEN(history.back());
    }
    saveToSave("save.dat");
    printStateJSON();
}

void Game::handleGuiState() {
    loadFromSave("save.dat");
    timer.update(board.turn == WHITE);
    saveToSave("save.dat");
    printStateJSON();
}

void Game::playTerminal() {
    newGame(1, 10);
    std::string cmd;
    while (getStatusString() == "active" || getStatusString() == "check") {
        board.print();
        std::cout << "White Time: " << timer.getWhiteTime()/1000 << "s | Black Time: " << timer.getBlackTime()/1000 << "s\n";
        
        if (mode == 1 && board.turn == BLACK) {
            std::cout << "AI thinking...\n";
            AI ai;
            Move m = ai.getBestMove(board, 4);
            board.makeMove(m);
            history.push_back(board.toFEN());
            timer.update(board.turn == WHITE);
            continue;
        }
        
        std::cout << "Enter move (e.g. e2e4), undo, redo, save, load, quit: ";
        std::cin >> cmd;
        if (cmd == "quit") break;
        if (cmd == "undo") {
            if (history.size() > 2) {
                history.pop_back(); history.pop_back();
                board.loadFEN(history.back());
            }
            continue;
        }
        if (cmd == "save") { saveToSave("save_term.dat"); std::cout << "Saved.\n"; continue; }
        if (cmd == "load") { loadFromSave("save_term.dat"); std::cout << "Loaded.\n"; continue; }
        
        Move m = Move::fromString(cmd);
        if (board.makeMove(m)) {
            history.push_back(board.toFEN());
            timer.update(board.turn == WHITE);
        } else {
            std::cout << "Invalid move!\n";
        }
    }
    std::cout << "Game Over: " << getStatusString() << "\n";
}
