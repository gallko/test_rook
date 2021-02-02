#include <iostream>

#include "Game.h"
#include "IChessBoard.h"
#include "ChessBoardImpl.h"
#include "GameRules.h"
#include "Logger.h"
#include "ParticipantGame.h"

Game::Game(size_t countParticipants, size_t countSteps)
    : mGameElements()
    , mCountParticipants(countParticipants)
    , mCountSteps(countSteps)
    , mStartGame(false)
    , mStartBarrier()
    , mEndBarrier()
{

}

void Game::startGame()
{
    if (!mStartGame)
    {
        mStartGame = true;
        auto deleter = [](pthread_barrier_t *barrier) {
            pthread_barrier_destroy(barrier);
            delete barrier;
        };
        mStartBarrier = std::shared_ptr<pthread_barrier_t>(new pthread_barrier_t(), deleter);
        mEndBarrier = std::shared_ptr<pthread_barrier_t>(new pthread_barrier_t(), deleter);

        pthread_barrier_init(mStartBarrier.get(), nullptr, mCountParticipants);
        pthread_barrier_init(mEndBarrier.get(), nullptr, mCountParticipants + 1);

        auto board = std::make_shared<ChessBoardImpl>(GameRules::sizeBoard());
        auto logger = std::make_shared<Logger>(std::cout);
        board->startGame();
        logger->startGame();
        board->addNotifier(logger);

        for (size_t i = 0; i < mCountParticipants; ++i)
        {
            auto participant = std::make_shared<ParticipantGame>(board, mCountSteps, mStartBarrier, mEndBarrier);
            participant->startGame();
            mGameElements.push_back(participant);
        }

        mGameElements.push_back(logger);
        mGameElements.push_back(board);
    }
}

void Game::stopGame() {
    if (mStartGame)
    {
        mStartGame = false;
        for (auto &element: mGameElements)
        {
            element->stopGame();
        }
        mGameElements.clear();
        pthread_barrier_wait(mEndBarrier.get());
    }
}

void Game::waitEnd()
{
    pthread_barrier_wait(mEndBarrier.get());
    mEndBarrier.reset();
}
