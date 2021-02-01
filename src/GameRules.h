#pragma once

#include <random>
#include <memory>
#include <chrono>
#include "IChessMan.h"

class GameRules
{
public:
    static bool checkStep(const std::shared_ptr<chessman::IChessMan> &chessMan, const board::Coordinate &coordinate);
    static board::Coordinate generateFirstStep();
    static board::Coordinate generateStep(const chessman::IChessMan &chessMan);
    static constexpr std::int32_t sizeBoard();
    static std::uint32_t generateId();
    static std::shared_ptr<chessman::IChessMan> makeChessMan(chessman::ChessmanType type);

    static std::chrono::milliseconds generateDelayWaitNextStep();
    static std::chrono::milliseconds generateDelayWaitForCell();
    static std::chrono::milliseconds generateDelayConfirm();
};

inline constexpr std::int32_t GameRules::sizeBoard()
{
    return 8;
}
