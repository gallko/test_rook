#include "ChessManImpl.h"

ChessManImpl::ChessManImpl(uint32_t id, chessman::ChessmanType type)
    : chessman::IChessMan()
    , mId(id)
    , mType(type)
    , mCoordinate(-1,-1)
{

}

chessman::ChessmanType ChessManImpl::getType() const
{
    return mType;
}

std::uint32_t ChessManImpl::getID() const
{
    return mId;
}

const Coordinate &ChessManImpl::getCurrentCoordinate() const
{
    return mCoordinate;
}

void ChessManImpl::setCurrentCoordinate(const Coordinate &coordinate)
{
    mCoordinate = coordinate;
}
