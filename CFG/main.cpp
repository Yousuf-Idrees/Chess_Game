#include "Game.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    Game game;
    if (argc > 1 && std::string(argv[1]) == "gui") {
        if (argc < 3) return 0;
        std::string cmd = argv[2];
        if (cmd == "init") {
            int mode = (std::string(argv[3]) == "pvp") ? 0 : 1;
            int time_m = std::stoi(argv[4]);
            game.handleGuiInit(mode, time_m);
        } else if (cmd == "move") {
            game.handleGuiMove(argv[3]);
        } else if (cmd == "aimove") {
            game.handleGuiAiMove();
        } else if (cmd == "state") {
            game.handleGuiState();
        } else if (cmd == "undo") {
            game.handleGuiUndo();
        } else if (cmd == "redo") {
            game.handleGuiRedo();
        }
    } else {
        std::cout << "Starting Interactive Terminal Game...\n";
        game.playTerminal();
    }
    return 0;
}
