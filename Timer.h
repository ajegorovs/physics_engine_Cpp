#pragma once

#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <iostream>

class Timer {
private:
    std::chrono::time_point<std::chrono::steady_clock> start_time; // To store the start time

public:
    void line_init(const std::string& className);
    void line_end(const std::string& className);
};

#endif