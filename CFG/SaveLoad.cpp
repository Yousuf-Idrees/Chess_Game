#include "SaveLoad.h"
#include <fstream>
#include <iostream>

bool SaveLoad::save(const std::string& filename, const GameStateData& data) {
    std::ofstream out(filename);
    if (!out) return false;
    out << data.mode << "\n";
    out << data.white_time << "\n";
    out << data.black_time << "\n";
    out << data.last_timestamp << "\n";
    out << data.current_fen << "\n";
    
    out << data.history.size() << "\n";
    for (const auto& h : data.history) out << h << "\n";
    
    out << data.redo_stack.size() << "\n";
    for (const auto& r : data.redo_stack) out << r << "\n";
    return true;
}

bool SaveLoad::load(const std::string& filename, GameStateData& data) {
    std::ifstream in(filename);
    if (!in) return false;
    in >> data.mode;
    in >> data.white_time;
    in >> data.black_time;
    in >> data.last_timestamp;
    std::getline(in >> std::ws, data.current_fen);
    
    size_t hsize;
    if (in >> hsize) {
        data.history.clear();
        for (size_t i = 0; i < hsize; ++i) {
            std::string h;
            std::getline(in >> std::ws, h);
            data.history.push_back(h);
        }
    }
    
    size_t rsize;
    if (in >> rsize) {
        data.redo_stack.clear();
        for (size_t i = 0; i < rsize; ++i) {
            std::string r;
            std::getline(in >> std::ws, r);
            data.redo_stack.push_back(r);
        }
    }
    return true;
}
