#pragma once

#include "Coordinate.h"

namespace chessman {

enum class ChessmanType : std::uint8_t {
    rook
#ifdef TEST_BUILD
    , test
#endif
};

class IChessman {
public:
    virtual ChessmanType type() const = 0;
    virtual std::string name() const = 0;
    virtual Coordinate getCurrentCoordinate() const = 0;
    virtual void setCurrentCoordinate(const Coordinate &coordinate) = 0;
};

}
