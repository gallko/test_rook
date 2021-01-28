#pragma once

#include <thread>
#include <memory>
#include <string>
#include <mutex>
#include <condition_variable>


class IState;
class IGameRules;
namespace board {
    class IChessBoard;
}

class ParticipantGame
{
public:
    ParticipantGame(std::shared_ptr<board::IChessBoard> board, std::shared_ptr<IGameRules> gameRules);
    ~ParticipantGame();

    void start(pthread_barrier_t *barrier);
    void stop();
    void notifyPlaceVacated();

private:
    enum class ReasonWeakUp;
    void main_loop(pthread_barrier_t *barrier);
    void changeState(std::shared_ptr<IState> newState);

    std::unique_ptr<std::thread> mTread;
    const std::string mName;

    std::shared_ptr<IState> mState;

    std::mutex mMutex;
    std::condition_variable mWait;
    ReasonWeakUp mReasonWeakUp;
};

enum class ParticipantGame::ReasonWeakUp
{ // in order of importance
    stop, next_step, fake
};

