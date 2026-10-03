#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include "Timer.h"
#include "SaveLoad.h"
#include <vector>
#include <string>

class Game {
private:
    Board board;
    Timer timer;
    int mode; // 0 = PVP, 1 = PVAI
    std::vector<std::string> history;
    std::vector<std::string> redo_stack;
    
public:
    Game();
    void newGame(int m, int time_minutes);
    void playTerminal();
    
    // GUI Bridge methods
    void handleGuiInit(int m, int time_min);
    void handleGuiMove(const std::string& move_str);
    void handleGuiAiMove();
    void handleGuiUndo();
    void handleGuiRedo();
    void handleGuiState();
    
    void loadFromSave(const std::string& file);
    void saveToSave(const std::string& file);
    
    void printStateJSON();
    std::string getStatusString();
};

#endif
