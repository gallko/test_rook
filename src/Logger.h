#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include "TreadBase.h"
#include "IChessBoard.h"
#include "IGameElement.h"

class Logger
        : public TreadBase
        , public board::INotifier
        , public IGameElement
{
public:
    explicit Logger(std::ostream &os);
    ~Logger() override;

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
    void onStop() override;

private:
    enum class Action;
    enum class ReasonWeakUp;
    struct LogStruct;
    friend std::ostream& operator<<(std::ostream& os, const Logger::Action& action);
    void print(const LogStruct logStruct);

    std::ostream &mOut;

    std::mutex mMutex;
    std::condition_variable mWait;
    ReasonWeakUp mReasonWeakUp;
    std::queue<LogStruct> mQueueLog;
};

enum class Logger::Action
{
    placed,
    moved,
    cancelMoved,
    removed,
    waitingForCell,
    reject
};

enum class Logger::ReasonWeakUp
{
    print_log, stop, exit, fake
};

struct Logger::LogStruct {
    LogStruct(Action action,
              std::uint32_t id,
              board::Coordinate fromCoordinate,
              board::Coordinate toCoordinate,
              board::ReasonReject reasonReject)
        : mAction(action)
        , mId(id)
        , mFromCoordinate(std::move(fromCoordinate))
        , mToCoordinate(std::move(toCoordinate))
        , mReasonReject(reasonReject)
    {}

    Action mAction;
    std::uint32_t mId;
    board::Coordinate mFromCoordinate;
    board::Coordinate mToCoordinate;
    board::ReasonReject mReasonReject;
};
