#ifndef SAVELOAD_H
#define SAVELOAD_H

#include <string>
#include <vector>

struct GameStateData {
    int mode;
    long long white_time;
    long long black_time;
    long long last_timestamp;
    std::string current_fen;
    std::vector<std::string> history;
    std::vector<std::string> redo_stack;
};

class SaveLoad {
public:
    static bool save(const std::string& filename, const GameStateData& data);
    static bool load(const std::string& filename, GameStateData& data);
};

#endif
