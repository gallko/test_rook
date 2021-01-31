#pragma once

#include <string>

#include "Coordinate.h"

namespace chessman {

enum class ChessmanType : std::uint8_t {
    rook
};

class IChessMan {
public:
    virtual ~IChessMan() = default;

    virtual std::uint32_t getID() const = 0;
    virtual ChessmanType getType() const = 0;
    virtual const Coordinate &getCurrentCoordinate() const = 0;
    virtual void setCurrentCoordinate(const Coordinate &coordinate) = 0;
};

}
