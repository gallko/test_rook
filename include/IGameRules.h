#pragma once

#include <memory>

#include "Coordinate.h"

namespace chessman {
    class IChessMan;
}

class IGameRules
{
public:
    virtual ~IGameRules() = default;

    virtual bool checkStep(const std::shared_ptr<chessman::IChessMan> &chessMan, const Coordinate &coordinate) const = 0;
    virtual Coordinate generateStep(const std::shared_ptr<chessman::IChessMan> &chessMan) const = 0;
};