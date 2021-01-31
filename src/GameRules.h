#pragma once

#include <random>
#include <memory>
#include "IChessMan.h"

class GameRules
{
public:
    static bool checkStep(const std::shared_ptr<chessman::IChessMan> &chessMan, const Coordinate &coordinate);
    static Coordinate generateStep(const chessman::IChessMan &chessMan);
    static constexpr std::uint32_t sizeBoard();
    static std::uint32_t generateId();
    static std::shared_ptr<chessman::IChessMan> makeChessMan(chessman::ChessmanType type);
};

inline constexpr std::uint32_t GameRules::sizeBoard()
{
    return 8;
}
