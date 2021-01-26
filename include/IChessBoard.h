#pragma once

#include "RemoveCopyMove.h"
#include "Coordinate.h"

template <std::uint8_t SIZE = 8>
class IChessBoard : public RemoveCopyMove {
public:
    enum class ErrorCode {
        ERROR, OCCUPIED_PLACE, SUCCESS
    };

    static constexpr int getSize()  {
        return SIZE;
    }

    virtual ~IChessBoard() = default;
    virtual ErrorCode moveFigure(Coordinate from, Coordinate to) = 0;
    virtual ErrorCode placeFigure(Coordinate to) = 0;
    virtual void removeFigure(Coordinate from);
};
