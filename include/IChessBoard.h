#pragma once

#include <memory>

#include "RemoveCopyMove.h"
#include "Coordinate.h"

namespace chessman {
    class IChessMan;
}

namespace board {

class INotifier: public RemoveCopyMove
{
public:
    ~INotifier() override = default;
    virtual void moved(std::int32_t id, const Coordinate &to) = 0;
    virtual void placed(std::int32_t id, const Coordinate &to) = 0;
    virtual void removed(std::int32_t id, const Coordinate &from) = 0;
    virtual void waitingForCell(std::int32_t id, const Coordinate &to) = 0;
    virtual void reject(std::int32_t id) = 0;
};

class IChessBoard : public RemoveCopyMove {
public:
    ~IChessBoard() override = default;

    virtual bool moveFigure(const chessman::IChessMan &figure, const Coordinate &to, std::shared_ptr<INotifier> notifier) = 0;
    virtual bool placeFigure(const chessman::IChessMan &figure, const Coordinate &to, std::shared_ptr<INotifier> notifier) = 0;
    virtual bool removeFigure(const chessman::IChessMan &figure, std::shared_ptr<INotifier> notifier) = 0;
    virtual std::uint8_t sizeBoard() const noexcept = 0;
};


} // board