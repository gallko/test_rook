#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "IChessBoard.h"
#include "IChessman.h"
#include "ChessBoardImpl.h"

class ChessBoardTest : public ::testing::Test
{
protected:
    void SetUp() override {
        Test::SetUp();
        mBoard = std::make_shared<ChessBoardImpl>();
    }

    void TearDown() override {
        Test::TearDown();
        mBoard.reset();
    }

    std::shared_ptr<board::IChessBoard> mBoard;
};

class MockIChessman : public chessman::IChessman {
public:
    MOCK_METHOD(chessman::ChessmanType, type, (), (const, override));
    MOCK_METHOD(std::string, name, (), (const, override));
    MOCK_METHOD(Coordinate, getCurrentCoordinate, (), (const, override));
    MOCK_METHOD(void, setCurrentCoordinate, (const Coordinate &coordinate), (override));
};

TEST_F(ChessBoardTest, placeFigure) {
    const auto chessman = std::make_shared<MockIChessman>();

    EXPECT_CALL(*chessman, setCurrentCoordinate(testing::_))
            .Times(testing::Exactly(2));
    EXPECT_CALL(*chessman, getCurrentCoordinate()).Times(testing::Exactly(0));
    EXPECT_CALL(*chessman, type())
            .Times(testing::Exactly(2))
            .WillOnce(testing::Return(chessman::ChessmanType::test))
            .WillOnce(testing::Return(chessman::ChessmanType::rook));


    auto result = mBoard->placeFigure(chessman, {0, 0});
    EXPECT_EQ(result, board::ErrorCode::success);

    result = mBoard->placeFigure(chessman, {7, 7});
    EXPECT_EQ(result, board::ErrorCode::success);

    EXPECT_THROW(mBoard->placeFigure(chessman, {8, 7}), std::out_of_range);
    EXPECT_THROW(mBoard->placeFigure(chessman, {7, 8}), std::out_of_range);
    EXPECT_THROW(mBoard->placeFigure(chessman, {-1, 5}), std::out_of_range);
    EXPECT_THROW(mBoard->placeFigure(chessman, {3, -1}), std::out_of_range);

    result = mBoard->placeFigure(chessman, {0, 0});
    EXPECT_EQ(result, board::ErrorCode::occupied_place);
}

TEST_F(ChessBoardTest, moveFigure_SelfToEmpty) {
    const auto chessman = std::make_shared<MockIChessman>();

    EXPECT_CALL(*chessman, setCurrentCoordinate(testing::_)).Times(testing::Exactly(0));
    EXPECT_CALL(*chessman, getCurrentCoordinate())
            .Times(testing::Exactly(1))
            .WillOnce(testing::Return(Coordinate{1,2}));
    EXPECT_CALL(*chessman, type())
            .Times(testing::Exactly(2))
            .WillRepeatedly(testing::Return(chessman::ChessmanType::rook));

    // place
    auto result = mBoard->placeFigure(chessman, {1, 2});
    EXPECT_EQ(result, board::ErrorCode::success);

    // moved
    result = mBoard->moveFigure(chessman, {5,5});
    EXPECT_EQ(result, board::ErrorCode::success);
}

TEST_F(ChessBoardTest, moveFigure_SelfWrongToEnyWere) {
    const auto chessman = std::make_shared<MockIChessman>();

    EXPECT_CALL(*chessman, setCurrentCoordinate(testing::_)).Times(testing::Exactly(0));
    EXPECT_CALL(*chessman, type())
            .Times(testing::Exactly(1))
            .WillOnce(testing::Return(chessman::ChessmanType::rook));
    EXPECT_CALL(*chessman, getCurrentCoordinate())
            .Times(testing::Exactly(1))
            .WillOnce(testing::Return(Coordinate{3,7}));

    auto result = mBoard->moveFigure(chessman, {5,5});
    EXPECT_EQ(result, board::ErrorCode::error);
}

TEST_F(ChessBoardTest, moveFigure_WrongSelfToAnyWhere) {
    const auto chessman = std::make_shared<MockIChessman>();

    EXPECT_CALL(*chessman, setCurrentCoordinate(testing::_)).Times(testing::Exactly(0));
    EXPECT_CALL(*chessman, getCurrentCoordinate())
            .Times(testing::Exactly(1))
            .WillOnce(testing::Return(Coordinate{1,2}));
    EXPECT_CALL(*chessman, type())
            .Times(testing::Exactly(2))
            .WillOnce(testing::Return(chessman::ChessmanType::rook))
            .WillOnce(testing::Return(chessman::ChessmanType::test));

    // place
    auto result = mBoard->placeFigure(chessman, {1, 2});
    EXPECT_EQ(result, board::ErrorCode::success);

    // moved
    result = mBoard->moveFigure(chessman, {5,5});
    EXPECT_EQ(result, board::ErrorCode::error);

}

TEST_F(ChessBoardTest, moveFigure_SelfToOccupied) {
    const auto chessman = std::make_shared<MockIChessman>();

    EXPECT_CALL(*chessman, setCurrentCoordinate(testing::_)).Times(testing::Exactly(0));
    EXPECT_CALL(*chessman, getCurrentCoordinate()).Times(testing::Exactly(0));
    EXPECT_CALL(*chessman, type())
            .Times(testing::Exactly(2))
            .WillRepeatedly(testing::Return(chessman::ChessmanType::rook));

    // place first
    auto result = mBoard->placeFigure(chessman, {1, 2});
    EXPECT_EQ(result, board::ErrorCode::success);

    // place second
    result = mBoard->placeFigure(chessman, {5, 5});
    EXPECT_EQ(result, board::ErrorCode::success);

    // moved
    result = mBoard->moveFigure(chessman, {5,5});
    EXPECT_EQ(result, board::ErrorCode::occupied_place);
}