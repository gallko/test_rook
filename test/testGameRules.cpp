#include <gtest/gtest.h>

#include "ChessRookImpl.h"
#include "GameRulesImpl.h"

class GameRulesTest : public ::testing::Test
{
protected:
    void SetUp() override {
        Test::SetUp();
        mGameRules = std::make_shared<GameRulesImpl>(20);
    }

    void TearDown() override {
        Test::TearDown();
        mGameRules.reset();
    }

    std::shared_ptr<IGameRules> mGameRules;
};

TEST_F(GameRulesTest, checkStepRook) {
    auto mChess = std::make_shared<ChessRookImpl>("test");
    mChess->setCurrentCoordinate({0, 0});

    EXPECT_TRUE(mGameRules->checkStep(mChess, {5, 0}));
    EXPECT_TRUE(mGameRules->checkStep(mChess, {0, 2}));
    EXPECT_FALSE(mGameRules->checkStep(mChess, {0, 0}));
    EXPECT_FALSE(mGameRules->checkStep(mChess, {1, 2}));
    EXPECT_FALSE(mGameRules->checkStep(mChess, {6, 6}));
}

TEST_F(GameRulesTest, generateStepStepRook) {
    auto mChess = std::make_shared<ChessRookImpl>("test");
    mChess->setCurrentCoordinate({10, 7});

    for (size_t i = 0; i < 1000; i++) {
        auto coordinate = mGameRules->generateStep(mChess);
        EXPECT_TRUE(mGameRules->checkStep(mChess, coordinate));
    }
}
