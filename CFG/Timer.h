#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer {
private:
    long long white_time_ms;
    long long black_time_ms;
    long long last_timestamp_ms;
    bool running;
    bool white_turn;

    long long getCurrentTimeMs() const;

public:
    Timer();
    
    void start(long long initial_time_ms);
    void update(bool is_white_turn);
    void setTimes(long long w_ms, long long b_ms, long long last_ts);
    long long getLastTimestamp() const;
    
    long long getWhiteTime() const;
    long long getBlackTime() const;
    
    bool isTimeUp(bool is_white_turn);
};

#endif
