#pragma once

#include <string>

#include "Coordinate.h"

namespace chessman {

enum class ChessmanType : std::uint8_t {
    rook
#ifdef TEST_BUILD
    , test
#endif
};

class IChessMan {
public:
    virtual ~IChessMan() = default;

    virtual std::int32_t id() const = 0;
    virtual ChessmanType type() const = 0;
    virtual const Coordinate &getCurrentCoordinate() const = 0;
    virtual void setCurrentCoordinate(const Coordinate &coordinate) = 0;
};

}
