#include <gtest/gtest.h>

#include "ChessManImpl.h"
#include "GameRules.h"


TEST(GameRulesTest, checkStepRook) {
    auto mChess = std::make_shared<ChessManImpl>(50, chessman::ChessmanType::rook);
    mChess->setCurrentCoordinate({0, 0});

    EXPECT_TRUE(GameRules::checkStep(mChess, {5, 0}));
    EXPECT_TRUE(GameRules::checkStep(mChess, {0, 2}));
    EXPECT_FALSE(GameRules::checkStep(mChess, {0, 0}));
    EXPECT_FALSE(GameRules::checkStep(mChess, {1, 2}));
    EXPECT_FALSE(GameRules::checkStep(mChess, {6, 6}));
}

TEST(GameRulesTest, generateStepStepRook)
{
    auto mChess = std::make_shared<ChessManImpl>(30, chessman::ChessmanType::rook);
    mChess->setCurrentCoordinate({10, 7});

    for (size_t i = 0; i < 1000; i++) {
        auto coordinate = GameRules::generateStep(*mChess);
        EXPECT_TRUE(GameRules::checkStep(mChess, coordinate));
    }
}
