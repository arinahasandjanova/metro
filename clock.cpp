#include "metro.h"
#include <iomanip>
#include <sstream>

Clock::Clock() : seconds(5 * 3600) {} // Start at 05:00

void Clock::tick() {
    seconds += 60; // Advance 1 minute
}

void Clock::add_seconds(int sec) {
    seconds += sec;
}

std::string Clock::get_time() const {
    int hours = (seconds / 3600) % 24;
    int minutes = (seconds % 3600) / 60;
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << hours << ":" << std::setw(2) << minutes;
    return ss.str();
}

bool Clock::is_after_midnight() const {
    return seconds >= 24 * 3600;
}