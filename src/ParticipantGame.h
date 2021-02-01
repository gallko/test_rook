#pragma once

#include <memory>
#include <mutex>
#include <condition_variable>
#include <list>
#include <chrono>

#include "IGameElement.h"
#include "TreadBase.h"
#include "IChessBoard.h"


class ParticipantGame
        : public IGameElement
        , public TreadBase
        , public board::INotifier
        , public std::enable_shared_from_this<ParticipantGame>
{
public:
    class IState;
    struct Event;

    ParticipantGame(std::shared_ptr<board::IChessBoard> board, pthread_barrier_t *barrier);
    ~ParticipantGame() override;

    void startGame() override;
    void stopGame() override;

protected:
    void placed(std::uint32_t id, const board::Coordinate &to) noexcept override;
    void moved(std::uint32_t id, const board::Coordinate &from, const board::Coordinate &to) noexcept override;
    void cancelMoved(std::uint32_t id, const board::Coordinate &from, const board::Coordinate &to) noexcept override;
    void removed(std::uint32_t id, const board::Coordinate &from) noexcept override;
    void waitingForCell(std::uint32_t id, const board::Coordinate &from, const board::Coordinate &to) noexcept override;
    void reject(std::uint32_t id, board::ReasonReject reason) noexcept override;

    void onStart() override;
    void loop() override;

private:
    enum class ReasonWeakUp;

    pthread_barrier_t *mBarrier;
    std::shared_ptr<board::IChessBoard> mBoard;
    std::shared_ptr<chessman::IChessMan> mChessMan;

    std::mutex mMutex;
    std::condition_variable mWait;
    ReasonWeakUp mReasonWeakUp;
    std::unique_ptr<Event> mEvent;

    std::unique_ptr<IState> mState;
};

enum class ParticipantGame::ReasonWeakUp
{ // in order of importance
    start, stop, next_step
};

struct ParticipantGame::Event {
    enum class Type {
        stop, placed, moved, remove, cancelMoved, waitingForCell, reject
    };

    Event(Type type, std::uint32_t id, board::Coordinate from, board::Coordinate to, board::ReasonReject reasonReject)
            : mTypeEvent(type)
            , mId(id)
            , mFromCoordinate(std::move(from))
            , mToCoordinate(std::move(to))
            , mReasonReject(reasonReject)
    {

    }

    const Type mTypeEvent;
    const std::uint32_t mId;
    const board::Coordinate mFromCoordinate;
    const board::Coordinate mToCoordinate;
    const board::ReasonReject mReasonReject;
};

class ParticipantGame::IState {
public:
    virtual ~IState() = default;

    virtual std::unique_ptr<IState> doWork(std::unique_ptr<Event>) = 0;
    virtual std::chrono::milliseconds waitPeriod() const = 0;
    virtual bool stop() const = 0;
};