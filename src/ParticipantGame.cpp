#include "ParticipantGame.h"
#include "GameRules.h"
#include "state/WaitForConfirmStep.h"


using namespace board;

ParticipantGame::ParticipantGame(std::shared_ptr<board::IChessBoard> board, pthread_barrier_t *barrier)
        : TreadBase("ParticipantGame")
        , mBarrier(barrier)
        , mBoard(std::move(board))
        , mChessMan(nullptr)
        , mMutex()
        , mWait()
        , mReasonWeakUp(ParticipantGame::ReasonWeakUp::start)
{

}

ParticipantGame::~ParticipantGame()
{
    join();
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
    if (mBarrier)
    {
        pthread_barrier_wait(mBarrier);
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
                return mEvent != nullptr;
            });
            if (mReasonWeakUp != ReasonWeakUp::stop)
            {
                event = std::move(mEvent);
            } else {
                event = std::make_unique<Event>(ParticipantGame::Event::Type::stop, 0,
                                                board::invalidCoordinate, board::invalidCoordinate,
                                                board::ReasonReject::empty);
            }
        }

        mState = mState->doWork(std::move(event));
    }
}

void ParticipantGame::onStop()
{

}

/* ************************************************************
 * IMPL board::INotifier
 * ************************************************************/
void ParticipantGame::placed(std::uint32_t id, const board::Coordinate &to) noexcept
{
    if (std::lock_guard lock(mMutex); id == mChessMan->getID() && mReasonWeakUp != ParticipantGame::ReasonWeakUp::stop)
    {
        mReasonWeakUp = ParticipantGame::ReasonWeakUp::next_step;
        mEvent = std::make_unique<Event>(Event::Type::placed, id, invalidCoordinate, to, board::ReasonReject::empty);
        mWait.notify_all();
    }
}

void ParticipantGame::moved(std::uint32_t id, const Coordinate &from, const Coordinate &to) noexcept
{
    if (std::lock_guard lock(mMutex); id == mChessMan->getID() && mReasonWeakUp != ParticipantGame::ReasonWeakUp::stop)
    {
        mReasonWeakUp = ParticipantGame::ReasonWeakUp::next_step;
        mEvent = std::make_unique<Event>(Event::Type::moved, id, from, to, ReasonReject::empty);
        mWait.notify_all();
    }
}

void ParticipantGame::cancelMoved(std::uint32_t id, const Coordinate &from, const Coordinate &to) noexcept
{
    if (std::lock_guard lock(mMutex); id == mChessMan->getID() && mReasonWeakUp != ParticipantGame::ReasonWeakUp::stop)
    {
        mReasonWeakUp = ParticipantGame::ReasonWeakUp::next_step;
        mEvent = std::make_unique<Event>(Event::Type::cancelMoved, id, from, to, ReasonReject::empty);
        mWait.notify_all();
    }
}

void ParticipantGame::removed(std::uint32_t id, const Coordinate &from) noexcept
{
    if (std::lock_guard lock(mMutex); id == mChessMan->getID() && mReasonWeakUp != ParticipantGame::ReasonWeakUp::stop)
    {
        mReasonWeakUp = ParticipantGame::ReasonWeakUp::next_step;
        mEvent = std::make_unique<Event>(Event::Type::cancelMoved, id, from, invalidCoordinate, ReasonReject::empty);
        mWait.notify_all();
    }
}

void ParticipantGame::waitingForCell(std::uint32_t id, const Coordinate &from, const Coordinate &to) noexcept
{
    if (std::lock_guard lock(mMutex); id == mChessMan->getID() && mReasonWeakUp != ParticipantGame::ReasonWeakUp::stop)
    {
        mReasonWeakUp = ParticipantGame::ReasonWeakUp::next_step;
        mEvent = std::make_unique<Event>(Event::Type::waitingForCell, id, from, to, ReasonReject::empty);
        mWait.notify_all();
    }
}

void ParticipantGame::reject(std::uint32_t id, board::ReasonReject reason) noexcept
{
    if (std::lock_guard lock(mMutex); id == mChessMan->getID() && mReasonWeakUp != ParticipantGame::ReasonWeakUp::stop)
    {
        mReasonWeakUp = ParticipantGame::ReasonWeakUp::next_step;
        mEvent = std::make_unique<Event>(Event::Type::waitingForCell, id, invalidCoordinate, invalidCoordinate, reason);
        mWait.notify_all();
    }
}

/* ************************************************************
 * IMPL private
 * ************************************************************/
