#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "ChessManImpl.h"

using namespace board;

TEST(ChessMenTest, type) {
    auto mChess = std::make_shared<ChessManImpl>(50, chessman::ChessmanType::rook);
    EXPECT_EQ(mChess->getType(), chessman::ChessmanType::rook);
}

TEST(ChessMenTest, defaultCoordinate) {
    auto mChess = std::make_shared<ChessManImpl>(50, chessman::ChessmanType::rook);
    EXPECT_EQ(mChess->getCurrentCoordinate(), Coordinate(-1, -1));
    EXPECT_NE(mChess->getCurrentCoordinate(), Coordinate(0, 0));
}

TEST(ChessMenTest, set_getCoordinate) {
    auto mChess = std::make_shared<ChessManImpl>(50, chessman::ChessmanType::rook);
    mChess->setCurrentCoordinate({5, 4});
    EXPECT_EQ(mChess->getCurrentCoordinate(), Coordinate(5, 4));
    EXPECT_NE(mChess->getCurrentCoordinate(), Coordinate(0, 0));
}
