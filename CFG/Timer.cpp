#include "Timer.h"

Timer::Timer() : white_time_ms(0), black_time_ms(0), last_timestamp_ms(0), running(false), white_turn(true) {}

long long Timer::getCurrentTimeMs() const {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

void Timer::start(long long initial_time_ms) {
    white_time_ms = initial_time_ms;
    black_time_ms = initial_time_ms;
    last_timestamp_ms = getCurrentTimeMs();
    running = true;
    white_turn = true;
}

void Timer::update(bool is_white_turn) {
    if (!running) return;
    long long now = getCurrentTimeMs();
    long long elapsed = now - last_timestamp_ms;
    
    if (white_turn) {
        white_time_ms -= elapsed;
        if (white_time_ms < 0) white_time_ms = 0;
    } else {
        black_time_ms -= elapsed;
        if (black_time_ms < 0) black_time_ms = 0;
    }
    
    last_timestamp_ms = now;
    white_turn = is_white_turn;
}

void Timer::setTimes(long long w_ms, long long b_ms, long long last_ts) {
    white_time_ms = w_ms;
    black_time_ms = b_ms;
    last_timestamp_ms = (last_ts == 0) ? getCurrentTimeMs() : last_ts;
    running = true;
}

long long Timer::getLastTimestamp() const {
    return last_timestamp_ms;
}

long long Timer::getWhiteTime() const {
    long long t = white_time_ms;
    if (running && white_turn) {
        t -= (getCurrentTimeMs() - last_timestamp_ms);
    }
    return t > 0 ? t : 0;
}

long long Timer::getBlackTime() const {
    long long t = black_time_ms;
    if (running && !white_turn) {
        t -= (getCurrentTimeMs() - last_timestamp_ms);
    }
    return t > 0 ? t : 0;
}

bool Timer::isTimeUp(bool is_white_turn) {
    if (is_white_turn) return getWhiteTime() <= 0;
    return getBlackTime() <= 0;
}
