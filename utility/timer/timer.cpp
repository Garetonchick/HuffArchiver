#include "timer.h"

Timer::Timer() {
    Reset();
}

void Timer::Reset() {
    last_reset_ = std::chrono::steady_clock::now();
}

int64_t Timer::GetMilliseconds() const {
    std::chrono::steady_clock::time_point current = std::chrono::steady_clock::now();

    return std::chrono::duration_cast<std::chrono::milliseconds>(current - last_reset_).count();
}

std::string Timer::GetMillisecondsString() const {
    return std::to_string(GetMilliseconds()) + "ms";
}