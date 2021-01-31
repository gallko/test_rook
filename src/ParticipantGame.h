#pragma once

#include <memory>
#include <mutex>
#include <condition_variable>

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
    ParticipantGame(std::shared_ptr<board::IChessBoard> board, pthread_barrier_t *barrier);
    ~ParticipantGame() override;

    void startGame() override;
    void stopGame() override;

protected:
    void placed(std::uint32_t id, const Coordinate &to) noexcept override;
    void moved(std::uint32_t id, const Coordinate &from, const Coordinate &to) noexcept override;
    void cancelMoved(std::uint32_t id, const Coordinate &from, const Coordinate &to) noexcept override;
    void removed(std::uint32_t id, const Coordinate &from) noexcept override;
    void waitingForCell(std::uint32_t id, const Coordinate &from, const Coordinate &to) noexcept override;
    void reject(std::uint32_t id, board::ReasonReject reason) noexcept override;

    void onStart() override;
    void loop() override;
    void onStop() override;

private:
    enum class ReasonWeakUp;

    pthread_barrier_t *mBarrier;
    std::shared_ptr<board::IChessBoard> mBoard;
    std::shared_ptr<chessman::IChessMan> mChessMan;

    std::mutex mMutex;
    std::condition_variable mWait;
    ReasonWeakUp mReasonWeakUp;

};

enum class ParticipantGame::ReasonWeakUp
{ // in order of importance
    stop, next_step, fake
};

