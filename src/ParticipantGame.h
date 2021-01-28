#pragma once

#include <thread>
#include <memory>
#include <string>
#include <mutex>

class IState;

class ParticipantGame {
public:
    ParticipantGame();
    void start(pthread_barrier_t *barrier);
    void stop();

    void changeState(std::shared_ptr<IState> newState);

private:
    void main_loop();

    pthread_barrier_t *mBarrierStart;
    std::unique_ptr<std::thread> mTread;
    const std::string mName;

    std::shared_ptr<IState> mState;
};

