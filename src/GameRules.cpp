#include <atomic>
#include "GameRules.h"
#include "IChessMan.h"
#include "ChessManImpl.h"

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

Coordinate GameRules::generateStep(const chessman::IChessMan &chessMan)
{
    static thread_local std::mt19937 mGen{std::random_device()() };

    Coordinate result;
    if (chessMan.getType() == chessman::ChessmanType::rook)
    {
       std::uniform_int_distribution distribution_direction(0, 1);
       auto &coordinate = chessMan.getCurrentCoordinate();
       if (distribution_direction(mGen))
       {   // change x
           std::uniform_int_distribution<> distribution_step(0 - coordinate.first, sizeBoard() - coordinate.first);
           int32_t delta;
           do {
               delta = distribution_step(mGen);
           } while (!delta);
           result.first = coordinate.first + delta;
           result.second = coordinate.second;
       } else {
           // change y
           std::uniform_int_distribution<> distribution_step(0 - coordinate.second, sizeBoard() - coordinate.second);
           int32_t delta;
           do {
               delta = distribution_step(mGen);
           } while (!delta);
           result.first = coordinate.first;
           result.second = coordinate.second + delta;
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

