#include "ChessRookImpl.h"

ChessRookImpl::ChessRookImpl(std::string name)
    : chessman::IChessMan()
    , mName(std::move(name))
    , mType(chessman::ChessmanType::rook)
    , mCoordinate(-1,-1)
{

}

chessman::ChessmanType ChessRookImpl::type() const
{
    return mType;
}

const std::string &ChessRookImpl::name() const
{
    return mName;
}

const Coordinate &ChessRookImpl::getCurrentCoordinate() const
{
    return mCoordinate;
}

void ChessRookImpl::setCurrentCoordinate(const Coordinate &coordinate)
{
    mCoordinate = coordinate;
}
