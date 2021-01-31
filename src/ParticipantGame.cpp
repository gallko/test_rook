#include "ParticipantGame.h"
#include "GameRules.h"


ParticipantGame::ParticipantGame(std::shared_ptr<board::IChessBoard> board, pthread_barrier_t *barrier)
        : TreadBase("ParticipantGame")
        , mBarrier(barrier)
        , mBoard(std::move(board))
        , mChessMan(nullptr)
        , mMutex()
        , mWait()
        , mReasonWeakUp(ParticipantGame::ReasonWeakUp::next_step)
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
    TreadBase::start();
    mChessMan = GameRules::makeChessMan(chessman::ChessmanType::rook);
    mBoard->addNotifier(shared_from_this());
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
    mBoard->placeFigure(*mChessMan, GameRules::generateStep(*mChessMan));
}

void ParticipantGame::loop()
{

}

void ParticipantGame::onStop()
{

}

/* ************************************************************
 * IMPL board::INotifier
 * ************************************************************/
void ParticipantGame::placed(std::uint32_t id, const Coordinate &to) noexcept
{

}

void ParticipantGame::moved(std::uint32_t id, const Coordinate &from, const Coordinate &to) noexcept
{

}

void ParticipantGame::cancelMoved(std::uint32_t id, const Coordinate &from, const Coordinate &to) noexcept
{

}

void ParticipantGame::removed(std::uint32_t id, const Coordinate &from) noexcept
{

}

void ParticipantGame::waitingForCell(std::uint32_t id, const Coordinate &from, const Coordinate &to) noexcept
{

}

void ParticipantGame::reject(std::uint32_t id, board::ReasonReject reason) noexcept
{

}

/* ************************************************************
 * IMPL private
 * ************************************************************/
