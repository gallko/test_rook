#include <atomic>
#include "GameRules.h"
#include "IChessMan.h"
#include "ChessManImpl.h"

using namespace board;

static thread_local std::mt19937 mGen{std::random_device()() };

bool GameRules::checkStep(const std::shared_ptr<chessman::IChessMan> &chessMan, const Coordinate &coordinate)
{
    auto result = false;
    auto &ch_crd = chessMan->getCurrentCoordinate();
    if (chessMan->getType() == chessman::ChessmanType::rook)
    {
        result = ch_crd != coordinate // ch_crd is not same coordinate
               && coordinate >= 0 && coordinate <= sizeBoard() // located in board
               && (ch_crd.first == coordinate.first || ch_crd.second == coordinate.second);
    }
    return result;
}

board::Coordinate GameRules::generateFirstStep() {
    std::uniform_int_distribution<> distribution_step(0, sizeBoard() - 1);
    return board::Coordinate(distribution_step(mGen), distribution_step(mGen));
}


Coordinate GameRules::generateStep(const chessman::IChessMan &chessMan)
{
    Coordinate result;
    {
       std::uniform_int_distribution distribution_direction(0, 1);
       auto &coordinate = chessMan.getCurrentCoordinate();
       if (distribution_direction(mGen))
       {   // change x
           std::uniform_int_distribution<> distribution_step(0 - coordinate.first, sizeBoard() - 1 - coordinate.first);
           Coordinate::first_type delta;
           do {
               delta = static_cast<Coordinate::first_type>(distribution_step(mGen));
           } while (!delta);
           result.first = static_cast<Coordinate::first_type>(coordinate.first + delta);
           result.second = coordinate.second;
       } else {
           // change y
           std::uniform_int_distribution<> distribution_step(0 - coordinate.second, sizeBoard() - 1 - coordinate.second);
           Coordinate::second_type delta;
           do {
               delta = static_cast<Coordinate::second_type>(distribution_step(mGen));
           } while (!delta);
           result.first = coordinate.first;
           result.second = static_cast<Coordinate::second_type>(coordinate.second + delta);
       }
    }

    return result;
}

std::uint32_t GameRules::generateId()
{
    static std::atomic<uint32_t> id{0};
    return ++id;
}

std::shared_ptr<chessman::IChessMan> GameRules::makeChessMan(chessman::ChessmanType type)
{
    return std::make_shared<ChessManImpl>(generateId(), type);
}

std::chrono::milliseconds GameRules::generateDelayWaitNextStep()
{
    std::uniform_int_distribution distribution_direction(200, 300);
    return std::chrono::milliseconds(distribution_direction(mGen));
}

std::chrono::milliseconds GameRules::generateDelayWaitForCell()
{
    return std::chrono::milliseconds(5000);
}

std::chrono::milliseconds GameRules::generateDelayConfirm()
{
    return std::chrono::milliseconds(100);
}

