#pragma once

#include <random>
#include "IGameRules.h"

class GameRulesImpl : public IGameRules {
public:
    explicit GameRulesImpl(std::int32_t sizeBoard);
    ~GameRulesImpl() override = default;

    bool checkStep(const std::shared_ptr<chessman::IChessMan> &chessMan, const Coordinate &coordinate) const override;
    Coordinate generateStep(const std::shared_ptr<chessman::IChessMan> &chessMan) const override;

private:
    mutable std::mt19937 mGen;
    std::int32_t mSizeBoard;
};
