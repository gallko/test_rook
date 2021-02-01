#pragma once

#include "IChessMan.h"

class ChessManImpl : public chessman::IChessMan
{
public:
    explicit ChessManImpl(uint32_t id, chessman::ChessmanType type);
    ~ChessManImpl() override = default;

    std::uint32_t getID() const override;
    chessman::ChessmanType getType() const override;
    const board::Coordinate &getCurrentCoordinate() const override;
    void setCurrentCoordinate(const board::Coordinate &coordinate) override;

private:
    const uint32_t mId;
    const chessman::ChessmanType mType;
    board::Coordinate mCoordinate;
};
