#pragma once
#include <chrono>
#include <string>

class Timer {
public:
    Timer();

    void Reset();
    int64_t GetMilliseconds() const;
    std::string GetMillisecondsString() const;

private:
    std::chrono::steady_clock::time_point last_reset_;
};