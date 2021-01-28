#pragma once

#include "IChessMan.h"

class ChessRookImpl : public chessman::IChessMan
{
public:
    explicit ChessRookImpl(std::string name);
    ~ChessRookImpl() override = default;

    chessman::ChessmanType type() const override;
    const std::string &name() const override;
    const Coordinate &getCurrentCoordinate() const override;
    void setCurrentCoordinate(const Coordinate &coordinate) override;

private:
    const std::string mName;
    const chessman::ChessmanType mType;
    Coordinate mCoordinate;
};
