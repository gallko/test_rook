#include "ParticipantGame.h"
#include "IState.h"
#include "IChessBoard.h"
#include "IGameRules.h"

ParticipantGame::ParticipantGame(std::shared_ptr<board::IChessBoard> board, std::shared_ptr<IGameRules> gameRules)
{

}

ParticipantGame::~ParticipantGame()
{
    mTread->join();
}

void ParticipantGame::start(pthread_barrier_t *barrier)
{
    if (!mTread) {
        mTread = std::make_unique<std::thread>(&ParticipantGame::main_loop, this, barrier);
        pthread_setname_np(mTread->native_handle(), mName.c_str());
    }
}

void ParticipantGame::stop()
{
    changeState(nullptr);
    std::lock_guard lock(mMutex);
    mReasonWeakUp = ReasonWeakUp::stop;
    mWait.notify_all();
}

void ParticipantGame::notifyPlaceVacated()
{
    std::lock_guard lock(mMutex);
    if (mReasonWeakUp != ReasonWeakUp::stop)
    {
        mReasonWeakUp = ReasonWeakUp::next_step;
    }
    mWait.notify_all();
}

void ParticipantGame::main_loop(pthread_barrier_t *barrier)
{
    if (barrier)
    {
        pthread_barrier_wait(barrier);
    }

    while (mState) {
        auto localState = std::atomic_load(&mState);
        if (localState)
        {
            changeState(localState->doWork());
        }
    }
}

void ParticipantGame::changeState(std::shared_ptr<IState> newState)
{
    if (mState) {
        if (newState)
        {
            std::atomic_store(&mState, std::move(newState));
        } else {
            // we must be sure on leave the function the mState variable is nullptr
            while (!std::atomic_compare_exchange_weak_explicit(
                    &mState,
                    &newState,
                    newState,
                    std::memory_order_release,
                    std::memory_order_relaxed))
            {
                /* empty */
            }
        }
    }
}
