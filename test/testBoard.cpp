#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "IChessBoard.h"
#include "IChessMan.h"
#include "ChessBoardImpl.h"

class MockIChessMan : public chessman::IChessMan {
public:
    ~MockIChessMan() override = default;

    MOCK_METHOD(chessman::ChessmanType, type, (), (const, override));
    MOCK_METHOD(std::int32_t, id, (), (const, override));
    MOCK_METHOD(const Coordinate &, getCurrentCoordinate, (), (const, override));
    MOCK_METHOD(void, setCurrentCoordinate, (const Coordinate &coordinate), (override));
};

class MockNotifier: public board::INotifier {
public:
    ~MockNotifier() override = default;

    MOCK_METHOD(void, moved, (std::int32_t id, const Coordinate &to), (override));
    MOCK_METHOD(void, placed, (std::int32_t id, const Coordinate &to), (override));
    MOCK_METHOD(void, removed, (std::int32_t id, const Coordinate &from), (override));
    MOCK_METHOD(void, waitingForCell, (std::int32_t id, const Coordinate &to), (override));
    MOCK_METHOD(void, reject, (std::int32_t id), (override));
};

class ChessBoardTest : public ::testing::Test
{
protected:
    void SetUp() override {
        Test::SetUp();
        mBoard = std::make_shared<ChessBoardImpl>();
        mockIChessMan = std::make_shared<MockIChessMan>();
        mockNotifier = std::make_shared<MockNotifier>();
        mBoard->start(nullptr);
    }

    void TearDown() override {
        Test::TearDown();
        mBoard->stop();
        mBoard.reset();
        mockIChessMan.reset();
        mockNotifier.reset();
    }

    std::shared_ptr<ChessBoardImpl> mBoard;
    std::shared_ptr<MockIChessMan> mockIChessMan;
    std::shared_ptr<MockNotifier> mockNotifier;
};

TEST_F(ChessBoardTest, placeFigure) {
/*
    Coordinate coordinate;
    EXPECT_CALL(*chessman, setCurrentCoordinate)
            .Times(testing::Exactly(2))
            .WillRepeatedly(testing::SaveArg<0>(&coordinate));
    EXPECT_CALL(*chessman, getCurrentCoordinate()).Times(testing::Exactly(0));
    EXPECT_CALL(*chessman, type())
            .Times(testing::Exactly(2))
            .WillOnce(testing::Return(chessman::ChessmanType::test))
            .WillOnce(testing::Return(chessman::ChessmanType::rook));
*/
    auto result = mBoard->placeFigure(chessman, {0, 0});
    EXPECT_EQ(result, board::ErrorCode::success);
    EXPECT_EQ(coordinate, Coordinate(0, 0));

    result = mBoard->placeFigure(chessman, {7, 7});
    EXPECT_EQ(result, board::ErrorCode::success);
    EXPECT_EQ(coordinate, Coordinate(7, 7));

    EXPECT_THROW(mBoard->placeFigure(chessman, {8, 7}), std::out_of_range);
    EXPECT_THROW(mBoard->placeFigure(chessman, {7, 8}), std::out_of_range);
    EXPECT_THROW(mBoard->placeFigure(chessman, {-1, 5}), std::out_of_range);
    EXPECT_THROW(mBoard->placeFigure(chessman, {3, -1}), std::out_of_range);

    result = mBoard->placeFigure(chessman, {0, 0});
    EXPECT_EQ(result, board::ErrorCode::occupied_place);*/
}

TEST_F(ChessBoardTest, moveFigure_SelfToEmpty) {/*
    const auto chessman = std::make_shared<MockIChessMan>();

    Coordinate coordinate;
    EXPECT_CALL(*chessman, setCurrentCoordinate)
            .Times(testing::Exactly(2))
            .WillRepeatedly(testing::SaveArg<0>(&coordinate));
    EXPECT_CALL(*chessman, getCurrentCoordinate())
            .Times(testing::Exactly(1))
            .WillOnce(testing::ReturnRefOfCopy(Coordinate{1,2}));
    EXPECT_CALL(*chessman, type())
            .Times(testing::Exactly(2))
            .WillRepeatedly(testing::Return(chessman::ChessmanType::rook));

    // place
    auto result = mBoard->placeFigure(chessman, {1, 2});
    EXPECT_EQ(result, board::ErrorCode::success);
    EXPECT_EQ(coordinate, Coordinate(1, 2));

    // moved
    result = mBoard->moveFigure(chessman, {5,5});
    EXPECT_EQ(result, board::ErrorCode::success);
    EXPECT_EQ(coordinate, Coordinate(5, 5));*/
}

TEST_F(ChessBoardTest, moveFigure_SelfWrongToEnyWere) {/*
    const auto chessman = std::make_shared<MockIChessMan>();

    EXPECT_CALL(*chessman, setCurrentCoordinate(testing::_)).Times(testing::Exactly(0));
    EXPECT_CALL(*chessman, type())
            .Times(testing::Exactly(1))
            .WillOnce(testing::Return(chessman::ChessmanType::rook));
    EXPECT_CALL(*chessman, getCurrentCoordinate())
            .Times(testing::Exactly(1))
            .WillOnce(testing::ReturnRefOfCopy(Coordinate{3,7}));

    auto result = mBoard->moveFigure(chessman, {5,5});
    EXPECT_EQ(result, board::ErrorCode::error);*/
}

TEST_F(ChessBoardTest, moveFigure_WrongSelfToAnyWhere) {/*
    const auto chessman = std::make_shared<MockIChessMan>();

    Coordinate coordinate;
    EXPECT_CALL(*chessman, setCurrentCoordinate)
            .Times(testing::Exactly(1))
            .WillRepeatedly(testing::SaveArg<0>(&coordinate));
    EXPECT_CALL(*chessman, getCurrentCoordinate())
            .Times(testing::Exactly(1))
            .WillOnce(testing::ReturnRefOfCopy(Coordinate{1,2}));
    EXPECT_CALL(*chessman, type())
            .Times(testing::Exactly(2))
            .WillOnce(testing::Return(chessman::ChessmanType::rook))
            .WillOnce(testing::Return(chessman::ChessmanType::test));

    // place
    auto result = mBoard->placeFigure(chessman, {1, 2});
    EXPECT_EQ(result, board::ErrorCode::success);
    EXPECT_EQ(coordinate, Coordinate(1, 2));

    // moved
    result = mBoard->moveFigure(chessman, {5,5});
    EXPECT_EQ(result, board::ErrorCode::error);*/

}

TEST_F(ChessBoardTest, moveFigure_SelfToOccupied) {/*
    const auto chessman = std::make_shared<MockIChessMan>();

    Coordinate coordinate;
    EXPECT_CALL(*chessman, setCurrentCoordinate)
            .Times(testing::Exactly(2))
            .WillRepeatedly(testing::SaveArg<0>(&coordinate));
    EXPECT_CALL(*chessman, getCurrentCoordinate())
            .Times(testing::Exactly(1))
            .WillOnce(testing::ReturnRefOfCopy(Coordinate(1, 2)));
    EXPECT_CALL(*chessman, type())
            .Times(testing::Exactly(2))
            .WillRepeatedly(testing::Return(chessman::ChessmanType::rook));

    // place first
    auto result = mBoard->placeFigure(chessman, {1, 2});
    EXPECT_EQ(result, board::ErrorCode::success);
    EXPECT_EQ(coordinate, Coordinate(1, 2));

    // place second
    result = mBoard->placeFigure(chessman, {5, 5});
    EXPECT_EQ(result, board::ErrorCode::success);
    EXPECT_EQ(coordinate, Coordinate(5, 5));

    // moved
    result = mBoard->moveFigure(chessman, {5,5});
    EXPECT_EQ(result, board::ErrorCode::occupied_place);*/
}

TEST_F(ChessBoardTest, moveFigure_SelfToSelf) {/*
    const auto chessman = std::make_shared<MockIChessMan>();

    EXPECT_CALL(*chessman, setCurrentCoordinate).Times(testing::Exactly(0));
    EXPECT_CALL(*chessman, getCurrentCoordinate()).Times(testing::Exactly(0));
    EXPECT_CALL(*chessman, type()).Times(testing::Exactly(0));*/
}