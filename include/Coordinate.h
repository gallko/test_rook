#pragma once
#include <cinttypes>

struct Coordinate {
    Coordinate(std::uint8_t x, std::uint8_t y)
        : mX(x), mY(y) { }
    const std::uint8_t mX = 0;
    const std::uint8_t mY = 0;
};