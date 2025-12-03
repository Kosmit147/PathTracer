#pragma once

#include <chrono>

#include "common.hpp"

class Timer
{
public:
    explicit Timer() { start(); }

    auto start() -> void { _start = std::chrono::high_resolution_clock::now(); }
    [[nodiscard]] auto elapsed_s() const -> double { return static_cast<double>(time()) * 0.001 * 0.001; }
    [[nodiscard]] auto elapsed_ms() const -> double { return static_cast<double>(time()) * 0.001; }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> _start;

private:
    [[nodiscard]] auto time() const -> usize
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - _start)
            .count();
    }
};
