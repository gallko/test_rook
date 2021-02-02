#include <iostream>
#include "ParticipantGame.h"
#include "GameRules.h"
#include "IChessMan.h"
#include "state/WaitForConfirmStep.h"

using namespace board;

ParticipantGame::ParticipantGame(std::shared_ptr<board::IChessBoard> board, size_t countStep,
                                 std::weak_ptr<pthread_barrier_t> startBarrier,
                                 std::weak_ptr<pthread_barrier_t> endBarrier)
        : TreadBase("ParticipantGame")
        , mStartBarrier(std::move(startBarrier))
        , mEndBarrier(std::move(endBarrier))
        , mBoard(std::move(board))
        , mChessMan(nullptr)
        , mMutex()
        , mWait()
        , mReasonWeakUp(ParticipantGame::ReasonWeakUp::start)
        , mCounterStep(countStep)
        , pThis(nullptr)
{

}

ParticipantGame::~ParticipantGame()
{
    TreadBase::join();
}

/* ************************************************************
 * IMPL IGameElement
 * ************************************************************/
void ParticipantGame::startGame()
{
    if (std::lock_guard lock(mMutex); mReasonWeakUp == ReasonWeakUp::start)
    {
        mReasonWeakUp = ReasonWeakUp::next_step;
        mChessMan = GameRules::makeChessMan(chessman::ChessmanType::rook);
        mBoard->addNotifier(shared_from_this());
        TreadBase::start();
    }
}

void ParticipantGame::stopGame()
{
    mBoard->removeNotifier(shared_from_this());
    std::lock_guard lock(mMutex);
    mReasonWeakUp = ParticipantGame::ReasonWeakUp::stop;
    mWait.notify_all();
}

/* ************************************************************
 * IMPL TreadBase
 * ************************************************************/
void ParticipantGame::onStart()
{
    pThis = shared_from_this();
    if (auto barrier = mStartBarrier.lock(); barrier)
    {
        pthread_barrier_wait(barrier.get());
    }
    mState = std::make_unique<WaitForConfirmStep>(mBoard, mChessMan);
    mBoard->placeFigure(*mChessMan, GameRules::generateFirstStep());
}

void ParticipantGame::loop()
{
    while (!mState->stop())
    {
        std::unique_ptr<Event> event;
        {
            std::unique_lock lock(mMutex);
            mWait.wait_for(lock, mState->waitPeriod(), [&]() {
                return !mEvents.empty();
            });
            if (mReasonWeakUp == ReasonWeakUp::stop)
            {
                mEvents.emplace_back(std::make_unique<Event>(ParticipantGame::Event::Type::stop,
                                     board::invalidCoordinate,
                                     board::ReasonReject::empty));
            }
            if (!mEvents.empty())
            {
                event = std::move(mEvents.front());
                mEvents.pop_front();
            }
        }

        mState = mState->doWork(std::move(event));
    }
}

void ParticipantGame::onStop()
{
    if (auto barrier = mEndBarrier.lock(); barrier)
    {
        pthread_barrier_wait(barrier.get());
    }
    pThis.reset();
}

/* ************************************************************
 * IMPL board::INotifier
 * ************************************************************/
void ParticipantGame::placed(std::uint32_t id, const board::Coordinate &to) noexcept
{
    if (std::lock_guard lock(mMutex); id == mChessMan->getID() && mReasonWeakUp != ParticipantGame::ReasonWeakUp::stop)
    {
        mReasonWeakUp = ParticipantGame::ReasonWeakUp::next_step;
        mEvents.emplace_back(std::make_unique<Event>(Event::Type::placed, to, board::ReasonReject::empty));
        mWait.notify_all();
    }
}

void ParticipantGame::moved(std::uint32_t id, const Coordinate &from, const Coordinate &to) noexcept
{
    if (std::lock_guard lock(mMutex); id == mChessMan->getID() && mReasonWeakUp != ParticipantGame::ReasonWeakUp::stop)
    {
        mEvents.emplace_back(std::make_unique<Event>(Event::Type::moved, to, ReasonReject::empty));
        if (--mCounterStep)
        {
            mReasonWeakUp = ParticipantGame::ReasonWeakUp::next_step;
        } else {
            mReasonWeakUp = ParticipantGame::ReasonWeakUp::stop;
        }
        mWait.notify_all();
    }
}

void ParticipantGame::cancelMoved(std::uint32_t id, const Coordinate &from, const Coordinate &to) noexcept
{
    if (std::lock_guard lock(mMutex); id == mChessMan->getID() && mReasonWeakUp != ParticipantGame::ReasonWeakUp::stop)
    {
        mReasonWeakUp = ParticipantGame::ReasonWeakUp::next_step;
        mEvents.emplace_back(std::make_unique<Event>(Event::Type::cancelMoved, to, ReasonReject::empty));
        mWait.notify_all();
    }
}

void ParticipantGame::removed(std::uint32_t id, const Coordinate &from) noexcept
{
    if (std::lock_guard lock(mMutex); id == mChessMan->getID() && mReasonWeakUp != ParticipantGame::ReasonWeakUp::stop)
    {
        mReasonWeakUp = ParticipantGame::ReasonWeakUp::next_step;
        mEvents.emplace_back(std::make_unique<Event>(Event::Type::remove, invalidCoordinate, ReasonReject::empty));
        mWait.notify_all();
    }
}

void ParticipantGame::waitingForCell(std::uint32_t id, const Coordinate &from, const Coordinate &to) noexcept
{
    if (std::lock_guard lock(mMutex); id == mChessMan->getID() && mReasonWeakUp != ParticipantGame::ReasonWeakUp::stop)
    {
        mReasonWeakUp = ParticipantGame::ReasonWeakUp::next_step;
        mEvents.emplace_back(std::make_unique<Event>(Event::Type::waitingForCell, to, ReasonReject::empty));
        mWait.notify_all();
    }
}

void ParticipantGame::reject(std::uint32_t id, board::ReasonReject reason) noexcept
{
    if (std::lock_guard lock(mMutex); id == mChessMan->getID() && mReasonWeakUp != ParticipantGame::ReasonWeakUp::stop)
    {
        mReasonWeakUp = ParticipantGame::ReasonWeakUp::next_step;
        mEvents.emplace_back(std::make_unique<Event>(Event::Type::reject, invalidCoordinate, reason));
        mWait.notify_all();
    }
}

/* ************************************************************
 * IMPL private
 * ************************************************************/
