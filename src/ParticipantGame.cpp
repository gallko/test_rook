#include "ParticipantGame.h"
#include "IState.h"

void ParticipantGame::start(pthread_barrier_t *barrier) {
    if (!mTread) {
        mTread = std::make_unique<std::thread>(&ParticipantGame::main_loop, this);
        pthread_setname_np(mTread->native_handle(), mName.c_str());
    }
}

void ParticipantGame::main_loop()
{
    pthread_barrier_wait(mBarrierStart);
    while (mState) {
        changeState(mState->doWork());
    }
}

void ParticipantGame::stop()
{
    changeState(nullptr);
}

void ParticipantGame::changeState(std::shared_ptr<IState> newState)
{

}

