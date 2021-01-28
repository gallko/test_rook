#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "ChessRookImpl.h"

class ChessRookTest : public ::testing::Test {
protected:
    static constexpr auto nameChess = "ChessTest";
    static constexpr auto nameWrong = "ChessTest2";

    void SetUp() override {
        Test::SetUp();
        mChess = std::make_shared<ChessRookImpl>(nameChess);
    }

    void TearDown() override {
        Test::TearDown();
        mChess.reset();
    }

    std::shared_ptr<chessman::IChessMan> mChess;
};

TEST_F(ChessRookTest, name) {
    EXPECT_EQ(mChess->name(), nameChess);
    EXPECT_NE(mChess->name(), nameWrong);
}

TEST_F(ChessRookTest, type) {
    EXPECT_EQ(mChess->type(), chessman::ChessmanType::rook);
    EXPECT_NE(mChess->type(), chessman::ChessmanType::test);
}

TEST_F(ChessRookTest, defaultCoordinate) {
    EXPECT_EQ(mChess->getCurrentCoordinate(), Coordinate(-1, -1));
    EXPECT_NE(mChess->getCurrentCoordinate(), Coordinate(0, 0));
}

TEST_F(ChessRookTest, set_getCoordinate) {
    mChess->setCurrentCoordinate({5, 4});
    EXPECT_EQ(mChess->getCurrentCoordinate(), Coordinate(5, 4));
    EXPECT_NE(mChess->getCurrentCoordinate(), Coordinate(0, 0));
}
