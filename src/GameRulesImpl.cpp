#include "GameRulesImpl.h"
#include "IChessMan.h"

GameRulesImpl::GameRulesImpl(std::int32_t sizeBoard)
    : IGameRules()
    , mGen(std::random_device()())
    , mSizeBoard(sizeBoard - 1)
{

}

bool GameRulesImpl::checkStep(const std::shared_ptr<chessman::IChessMan> &chessMan, const Coordinate &coordinate) const
{
    auto result = false;
    auto &ch_crd = chessMan->getCurrentCoordinate();
    if (chessMan->type() == chessman::ChessmanType::rook)
    {
        result = ch_crd != coordinate // ch_crd is not same coordinate
               && coordinate >= 0 && coordinate <= mSizeBoard // located in board
               && (ch_crd.first == coordinate.first || ch_crd.second == coordinate.second);
    }
    return result;
}

Coordinate GameRulesImpl::generateStep(const std::shared_ptr<chessman::IChessMan> &chessMan) const
{
    Coordinate result;
    if (chessMan->type() == chessman::ChessmanType::rook)
    {
       std::uniform_int_distribution distribution_direction(0, 1);
       auto &coordinate = chessMan->getCurrentCoordinate();
       if (distribution_direction(mGen))
       {   // change x
           std::uniform_int_distribution<> distribution_step(0 - coordinate.first, mSizeBoard - coordinate.first);
           int32_t delta;
           do {
               delta = distribution_step(mGen);
           } while (!delta);
           result.first = coordinate.first + delta;
           result.second = coordinate.second;
       } else {
           // change y
           std::uniform_int_distribution<> distribution_step(0 - coordinate.second, mSizeBoard - coordinate.second);
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

