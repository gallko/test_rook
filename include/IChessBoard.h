#pragma once

#include <memory>

#include "RemoveCopyMove.h"
#include "Coordinate.h"

namespace chessman {
    class IChessMan;
}

namespace board {

enum class ErrorCode : std::uint8_t {
    error, success, occupied_place
};

enum class PlaceType : std::uint8_t {
    empty, occupied_by_rook /*, occupied_by_* */
#ifdef TEST_BUILD
    , test
#endif
};

class IChessBoard : public RemoveCopyMove {
public:
    ~IChessBoard() override = default;

    virtual ErrorCode moveFigure(const std::shared_ptr<chessman::IChessMan> &figure, const Coordinate &to) = 0;
    virtual ErrorCode placeFigure(const std::shared_ptr<chessman::IChessMan> &figure, const Coordinate &to) = 0;
    virtual ErrorCode removeFigure(const std::shared_ptr<chessman::IChessMan> &figure) = 0;
    virtual std::uint8_t sizeBoard() const noexcept = 0;
};

} // board