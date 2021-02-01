#pragma once

#include <memory>

#include "RemoveCopyMove.h"
#include "Coordinate.h"

namespace chessman {
    class IChessMan;
}

namespace board {

enum class ReasonReject
{
    empty,
    boardStopped,
    incorrectCoordinate,
    idMismatch,
    incorrectId,
    duplicateId,
    waiterNotFound
};

class INotifier: public virtual RemoveCopyMove
{
public:
    ~INotifier() override = default;
    virtual void placed(std::uint32_t id, const Coordinate &to) noexcept = 0;
    virtual void moved(std::uint32_t id, const Coordinate &from, const Coordinate &to) noexcept = 0;
    virtual void cancelMoved(std::uint32_t id, const Coordinate &from, const Coordinate &to) noexcept = 0;
    virtual void removed(std::uint32_t id, const Coordinate &from) noexcept = 0;
    virtual void waitingForCell(std::uint32_t id, const Coordinate &from, const Coordinate &to) noexcept = 0;
    virtual void reject(std::uint32_t id, ReasonReject reason) noexcept = 0;
};

class IChessBoard : public virtual RemoveCopyMove {
public:
    ~IChessBoard() override = default;

    virtual void addNotifier(std::shared_ptr<INotifier> notifier) = 0;
    virtual void removeNotifier(std::shared_ptr<INotifier> notifier) = 0;

    virtual void placeFigure(const chessman::IChessMan &figure, const Coordinate &to) = 0;
    virtual void moveFigure(const chessman::IChessMan &figure, const Coordinate &to) = 0;
    virtual void cancelMoveFigure(const chessman::IChessMan &figure, const Coordinate &to) = 0;
    virtual void removeFigure(const chessman::IChessMan &figure) = 0;
    virtual std::uint8_t sizeBoard() const noexcept = 0;

    static constexpr std::uint32_t sEmptyCell = 0;
};


} // board