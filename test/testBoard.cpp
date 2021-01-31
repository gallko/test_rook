#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "IChessBoard.h"
#include "IChessMan.h"
#include "ChessBoardImpl.h"

using namespace testing;

class MockIChessMan : public chessman::IChessMan {
public:
    ~MockIChessMan() override = default;

    MOCK_METHOD(chessman::ChessmanType, getType, (), (const, override));
    MOCK_METHOD(std::uint32_t, getID, (), (const, override));
    MOCK_METHOD(const Coordinate &, getCurrentCoordinate, (), (const, override));
    MOCK_METHOD(void, setCurrentCoordinate, (const Coordinate &coordinate), (override));
};

class MockNotifier: public board::INotifier {
public:
    ~MockNotifier() override = default;

    MOCK_METHOD(void, placed, (std::uint32_t id, const Coordinate &to), (override, noexcept));
    MOCK_METHOD(void, moved, (std::uint32_t id,  const Coordinate &from, const Coordinate &to), (override, noexcept));
    MOCK_METHOD(void, cancelMoved, (std::uint32_t id,  const Coordinate &from, const Coordinate &to), (override, noexcept));
    MOCK_METHOD(void, removed, (std::uint32_t id, const Coordinate &from), (override, noexcept));
    MOCK_METHOD(void, waitingForCell, (std::uint32_t id, const Coordinate &from, const Coordinate &to), (override, noexcept));
    MOCK_METHOD(void, reject, (std::uint32_t id, board::ReasonReject reason), (override, noexcept));
};

class ChessBoardTest : public Test
{
protected:
    void SetUp() override
    {
        mBoard = std::make_shared<ChessBoardImpl>(8);
        mockIChessMan = std::make_shared<MockIChessMan>();
        mockNotifier = std::make_shared<MockNotifier>();
        mBoard->startGame();
        mBoard->addNotifier(mockNotifier);
    }

    void TearDown() override
    {
        mBoard->removeNotifier(mockNotifier);
        mBoard->stopGame();
        mockIChessMan.reset();
        mockNotifier.reset();
        mBoard.reset();
    }

    void waitFinished()
    {
        std::unique_lock lock(mMutex);
        state = true;
        mWait.notify_all();
    }

    void waitForFinish()
    {
        using namespace std::chrono;
        std::unique_lock lock(mMutex);
        state = false;
        mWait.template wait_for(lock, 200ms, [this]() {
            return state;
        });
    }

    bool state{false};
    std::condition_variable mWait;
    std::mutex mMutex;

    std::shared_ptr<ChessBoardImpl> mBoard;
    std::shared_ptr<MockIChessMan> mockIChessMan;
    std::shared_ptr<MockNotifier> mockNotifier;
};

TEST_F(ChessBoardTest, placeFigure_IncorrectId)
{
    EXPECT_CALL(*mockNotifier,reject(Eq(board::IChessBoard::sEmptyCell), Eq(board::ReasonReject::incorrectId))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t, board::ReasonReject) {
                waitFinished();
            }));
    EXPECT_CALL(*mockIChessMan, getID).WillRepeatedly(Return(board::IChessBoard::sEmptyCell));
    mBoard->placeFigure(*mockIChessMan, {0, 0});
    waitForFinish();
}

TEST_F(ChessBoardTest, placeFigure_Normal)
{
    Coordinate coordinate;
    std::int32_t id;

    coordinate = {0, 0}; id = 10;
    EXPECT_CALL(*mockIChessMan, getID).WillRepeatedly(Return(id));
    EXPECT_CALL(*mockNotifier, placed(Eq(id), Eq(coordinate))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t id, const Coordinate &) {
                waitFinished();
            }));
    mBoard->placeFigure(*mockIChessMan, coordinate);
    waitForFinish();

    coordinate = {5, 3}; id = 20;
    EXPECT_CALL(*mockIChessMan, getID).WillRepeatedly(Return(id));
    EXPECT_CALL(*mockNotifier, placed(Eq(id), Eq(coordinate))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t id, const Coordinate &) {
                waitFinished();
            }));
    mBoard->placeFigure(*mockIChessMan, coordinate);
    waitForFinish();

    coordinate = {mBoard->sizeBoard() - 1, mBoard->sizeBoard() - 1}; id = 30;
    EXPECT_CALL(*mockIChessMan, getID).WillRepeatedly(Return(id));
    EXPECT_CALL(*mockNotifier, placed(Eq(id), Eq(coordinate))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t id, const Coordinate &) {
                waitFinished();
            }));
    mBoard->placeFigure(*mockIChessMan, coordinate);
    waitForFinish();
}

TEST_F(ChessBoardTest, placeFigure_IncorrectCoordinate)
{
    std::int32_t id = 10;
    ON_CALL(*mockIChessMan, getCurrentCoordinate).WillByDefault(ReturnRefOfCopy(invalidCoordinate));

    EXPECT_CALL(*mockIChessMan, getID).Times(Exactly(7)).WillRepeatedly(Return(id));
    EXPECT_CALL(*mockNotifier,reject(Eq(id), Eq(board::ReasonReject::incorrectCoordinate))).Times(Exactly(7))
            .WillRepeatedly(Invoke([&](std::int32_t, board::ReasonReject) {
                waitFinished();
            }));

    mBoard->placeFigure(*mockIChessMan, {-1, 5});
    waitForFinish();

    mBoard->placeFigure(*mockIChessMan, {6,-1});
    waitForFinish();

    mBoard->placeFigure(*mockIChessMan, {-1, -5});
    waitForFinish();

    mBoard->placeFigure(*mockIChessMan, {mBoard->sizeBoard(), mBoard->sizeBoard() - 1});
    waitForFinish();

    mBoard->placeFigure(*mockIChessMan, {mBoard->sizeBoard() - 1, mBoard->sizeBoard()});
    waitForFinish();

    mBoard->placeFigure(*mockIChessMan, {mBoard->sizeBoard(), mBoard->sizeBoard()});
    waitForFinish();

    mBoard->placeFigure(*mockIChessMan, {- 1, mBoard->sizeBoard()});
    waitForFinish();
}

TEST_F(ChessBoardTest, placeFigure_DuplicateId)
{
    std::uint32_t id = 5;
    Coordinate coordinate{5, 5};

    EXPECT_CALL(*mockIChessMan, getID).Times(Exactly(3)).WillRepeatedly(Return(id));

    EXPECT_CALL(*mockNotifier,placed(Eq(id), Eq(coordinate))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t, const Coordinate &) {
                waitFinished();
            }));
    mBoard->placeFigure(*mockIChessMan, coordinate);
    waitForFinish();

    EXPECT_CALL(*mockNotifier,reject(Eq(id), Eq(board::ReasonReject::duplicateId))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t, board::ReasonReject) {
                waitFinished();
            }));
    mBoard->placeFigure(*mockIChessMan, coordinate);
    waitForFinish();

    coordinate = {4, 0};
    EXPECT_CALL(*mockNotifier,reject(Eq(id), Eq(board::ReasonReject::duplicateId))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t, board::ReasonReject) {
                waitFinished();
            }));
    mBoard->placeFigure(*mockIChessMan, coordinate);
    waitForFinish();
}

TEST_F(ChessBoardTest, placeFigure_ToBusyCell)
{
    EXPECT_CALL(*mockIChessMan, getID)
            .Times(Exactly(2))
            .WillOnce(Return(10))
            .WillOnce(Return(20));

    // place 10 to {5, 5}
    EXPECT_CALL(*mockNotifier,placed(Eq(10), Eq(Coordinate{5,5}))).Times(Exactly(1))
            .WillOnce(Invoke([&](std::int32_t, const Coordinate &to) {
                waitFinished();
            }));
    mBoard->placeFigure(*mockIChessMan, {5, 5});
    waitForFinish();

    // place 20 to {5, 5}
    EXPECT_CALL(*mockNotifier,waitingForCell(Eq(20), invalidCoordinate, (Coordinate{5,5}))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t, Coordinate, Coordinate) {
                waitFinished();
            }));
    mBoard->placeFigure(*mockIChessMan, {5, 5});
    waitForFinish();

    EXPECT_CALL(*mockNotifier,reject(Eq(10), Eq(board::ReasonReject::boardStopped))).Times(Exactly(1))
            .WillOnce(Invoke([&](std::int32_t, board::ReasonReject) {
                waitFinished();
            }));
    EXPECT_CALL(*mockNotifier,reject(Eq(20), Eq(board::ReasonReject::boardStopped))).Times(Exactly(1))
            .WillOnce(Invoke([&](std::int32_t, board::ReasonReject) {
                waitFinished();
            }));
    mBoard->stopGame();
    waitForFinish();
}

TEST_F(ChessBoardTest, moveFigure_IncorrectId)
{
    EXPECT_CALL(*mockIChessMan, getCurrentCoordinate).Times(Exactly(1))
            .WillOnce(ReturnRefOfCopy(Coordinate{5, 5}));
    EXPECT_CALL(*mockNotifier,reject(Eq(board::IChessBoard::sEmptyCell), Eq(board::ReasonReject::incorrectId))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t, board::ReasonReject) {
                waitFinished();
            }));
    EXPECT_CALL(*mockIChessMan, getID).WillRepeatedly(Return(board::IChessBoard::sEmptyCell));

    mBoard->moveFigure(*mockIChessMan, {0, 0});
    waitForFinish();
}

TEST_F(ChessBoardTest, moveFigure_IdMismatch)
{
    EXPECT_CALL(*mockIChessMan, getCurrentCoordinate).Times(Exactly(1))
            .WillOnce(ReturnRefOfCopy(Coordinate{5, 5}));

    EXPECT_CALL(*mockNotifier,reject(Eq(50), Eq(board::ReasonReject::idMismatch))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t, board::ReasonReject) {
                waitFinished();
            }));
    EXPECT_CALL(*mockIChessMan, getID).WillRepeatedly(Return(50));

    mBoard->moveFigure(*mockIChessMan, {0, 0});
    waitForFinish();
}

TEST_F(ChessBoardTest, moveFigure_IncorrectCoordinate)
{
    EXPECT_CALL(*mockNotifier,reject(Eq(5), Eq(board::ReasonReject::incorrectCoordinate))).Times(Exactly(6))
            .WillRepeatedly(Invoke([&](std::int32_t, board::ReasonReject) {
                waitFinished();
            }));
    EXPECT_CALL(*mockIChessMan, getID).WillRepeatedly(Return(5));
    EXPECT_CALL(*mockIChessMan, getCurrentCoordinate).Times(Exactly(3))
            .WillRepeatedly(ReturnRefOfCopy(Coordinate{0, 0}));

    mBoard->moveFigure(*mockIChessMan, {5, -1});
    waitForFinish();

    mBoard->moveFigure(*mockIChessMan, {-5, 0});
    waitForFinish();

    mBoard->moveFigure(*mockIChessMan, {-5, -3});
    waitForFinish();

    EXPECT_CALL(*mockIChessMan, getCurrentCoordinate).Times(Exactly(3))
            .WillOnce(ReturnRefOfCopy(Coordinate{-1, 0}))
            .WillOnce(ReturnRefOfCopy(Coordinate{0, -5}))
            .WillOnce(ReturnRefOfCopy(Coordinate{-2, -6}));

    mBoard->moveFigure(*mockIChessMan, {0, 0});
    waitForFinish();

    mBoard->moveFigure(*mockIChessMan, {0, 0});
    waitForFinish();

    mBoard->moveFigure(*mockIChessMan, {0, 0});
    waitForFinish();
}

TEST_F(ChessBoardTest, moveFigure_Normal)
{
    EXPECT_CALL(*mockIChessMan, getID).Times(Exactly(3))
        .WillOnce(Return(10))
        .WillOnce(Return(10))
        .WillOnce(Return(500));
    EXPECT_CALL(*mockIChessMan, getCurrentCoordinate)
        .Times(Exactly(1))
        .WillOnce(ReturnRefOfCopy(Coordinate{0, 0}));

    // place
    EXPECT_CALL(*mockNotifier, placed(Eq(10), Eq(Coordinate{0, 0}))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t id, const Coordinate &) {
                waitFinished();
            }));
    mBoard->placeFigure(*mockIChessMan, Coordinate{0, 0});
    waitForFinish();

    // move
    EXPECT_CALL(*mockNotifier,moved(Eq(10), Eq(Coordinate{0, 0}), Eq(Coordinate{5, 5}))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t id, const Coordinate &, const Coordinate &) {
                waitFinished();
            }));
    mBoard->moveFigure(*mockIChessMan, {5, 5});
    waitForFinish();

    // place to the vacant cell
    EXPECT_CALL(*mockNotifier, placed(Eq(500), Eq(Coordinate{0, 0}))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t, const Coordinate &to) {
                waitFinished();
            }));
    mBoard->placeFigure(*mockIChessMan, Coordinate{0, 0});
    waitForFinish();
}

TEST_F(ChessBoardTest, moveFigure_ToBusyCell)
{
    EXPECT_CALL(*mockIChessMan, getID)
            .Times(Exactly(4))
            .WillOnce(Return(10))
            .WillOnce(Return(20))
            .WillOnce(Return(20))
            .WillOnce(Return(10));
    EXPECT_CALL(*mockIChessMan, getCurrentCoordinate)
            .Times(Exactly(2))
            .WillOnce(ReturnRefOfCopy(Coordinate{0, 0}))
            .WillOnce(ReturnRefOfCopy(Coordinate{5, 5}));

    // place id:10 to {5,5}
    EXPECT_CALL(*mockNotifier, placed(Eq(10), Eq(Coordinate{5,5}))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t id, const Coordinate &to) {
                waitFinished();
            }));
    mBoard->placeFigure(*mockIChessMan, {5, 5});
    waitForFinish();

    // place id:20 to {0,0}
    EXPECT_CALL(*mockNotifier, placed(Eq(20), Eq(Coordinate{0,0}))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t id, const Coordinate &) {
                waitFinished();
            }));
    mBoard->placeFigure(*mockIChessMan, {0, 0});
    waitForFinish();

    // move from id:20 {0,0} to {5,5}
    EXPECT_CALL(*mockNotifier, waitingForCell(Eq(20), Eq(Coordinate{0, 0}), Eq(Coordinate{5, 5}))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t id, const Coordinate &, const Coordinate &) {
                waitFinished();
            }));
    mBoard->moveFigure(*mockIChessMan, {5,5});
    waitForFinish();

    // move from id:10 5,5 to 4,4
    // move from id:20 0,0 to 5,5
    EXPECT_CALL(*mockNotifier,moved(Eq(10), Eq(Coordinate{5,5}), Eq(Coordinate{4,4}))).Times(Exactly(1));
    EXPECT_CALL(*mockNotifier,moved(Eq(20), Eq(Coordinate{0,0}), Eq(Coordinate{5,5}))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t id, const Coordinate &, const Coordinate &) {
                waitFinished();
            }));
    mBoard->moveFigure(*mockIChessMan, {4, 4});
    waitForFinish();
}

TEST_F(ChessBoardTest, removeFigure_IncorrectId)
{
    EXPECT_CALL(*mockIChessMan, setCurrentCoordinate).Times(Exactly(0));

    EXPECT_CALL(*mockNotifier,placed(_, _)).Times(Exactly(0));
    EXPECT_CALL(*mockNotifier,moved(_, _, _)).Times(Exactly(0));
    EXPECT_CALL(*mockNotifier,removed(_, _)).Times(Exactly(0));
    EXPECT_CALL(*mockNotifier,waitingForCell(_, _, _)).Times(Exactly(0));

    EXPECT_CALL(*mockIChessMan, getCurrentCoordinate).Times(Exactly(1))
            .WillOnce(ReturnRefOfCopy(Coordinate{5, 5}));
    EXPECT_CALL(*mockNotifier,reject(Eq(board::IChessBoard::sEmptyCell), Eq(board::ReasonReject::incorrectId)))
            .Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t, board::ReasonReject) {
                waitFinished();
            }));
    EXPECT_CALL(*mockIChessMan, getID).WillRepeatedly(Return(board::IChessBoard::sEmptyCell));
    mBoard->removeFigure(*mockIChessMan);
    waitForFinish();
}

TEST_F(ChessBoardTest, removeFigure_IncorrectCoordinate)
{
    EXPECT_CALL(*mockIChessMan, setCurrentCoordinate).Times(Exactly(0));

    EXPECT_CALL(*mockNotifier,placed(_, _)).Times(Exactly(0));
    EXPECT_CALL(*mockNotifier,moved(_, _, _)).Times(Exactly(0));
    EXPECT_CALL(*mockNotifier,removed(_, _)).Times(Exactly(0));
    EXPECT_CALL(*mockNotifier,waitingForCell(_, _, _)).Times(Exactly(0));

    EXPECT_CALL(*mockIChessMan, getCurrentCoordinate).Times(Exactly(1))
            .WillOnce(ReturnRefOfCopy(Coordinate{-6, 6}));
    EXPECT_CALL(*mockNotifier,reject(Eq(5), Eq(board::ReasonReject::incorrectCoordinate)))
            .Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t, board::ReasonReject) {
                waitFinished();
            }));
    EXPECT_CALL(*mockIChessMan, getID).WillRepeatedly(Return(5));
    mBoard->removeFigure(*mockIChessMan);
    waitForFinish();
}

TEST_F(ChessBoardTest, removeFigure_IdMismatch)
{
    EXPECT_CALL(*mockIChessMan, setCurrentCoordinate).Times(Exactly(0));

    EXPECT_CALL(*mockNotifier,placed(_, _)).Times(Exactly(0));
    EXPECT_CALL(*mockNotifier,moved(_, _, _)).Times(Exactly(0));
    EXPECT_CALL(*mockNotifier,removed(_, _)).Times(Exactly(0));
    EXPECT_CALL(*mockNotifier,waitingForCell(_, _, _)).Times(Exactly(0));

    EXPECT_CALL(*mockIChessMan, getCurrentCoordinate).Times(Exactly(1))
            .WillOnce(ReturnRefOfCopy(Coordinate{5, 5}));

    EXPECT_CALL(*mockNotifier,reject(Eq(50), Eq(board::ReasonReject::idMismatch))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t, board::ReasonReject) {
                waitFinished();
            }));
    EXPECT_CALL(*mockIChessMan, getID).WillRepeatedly(Return(50));

    mBoard->removeFigure(*mockIChessMan);
    waitForFinish();
}

TEST_F(ChessBoardTest, removeFigure_Normal)
{
    EXPECT_CALL(*mockIChessMan, getID)
            .Times(Exactly(3))
            .WillOnce(Return(40))
            .WillOnce(Return(40))
            .WillOnce(Return(60));
    EXPECT_CALL(*mockIChessMan, getCurrentCoordinate)
            .Times(Exactly(1))
            .WillOnce(ReturnRefOfCopy(Coordinate{5, 5}));
    EXPECT_CALL(*mockIChessMan, setCurrentCoordinate).Times(Exactly(0));

    EXPECT_CALL(*mockNotifier,removed(_, _)).Times(Exactly(0));
    EXPECT_CALL(*mockNotifier,waitingForCell(_, _, _)).Times(Exactly(0));

    // place id:40{5,5}
    EXPECT_CALL(*mockNotifier, placed(Eq(40), Eq(Coordinate{5,5}))).Times(Exactly(1))
            .WillOnce(Invoke([&](std::int32_t id, const Coordinate &to) {
                waitFinished();
            }));
    mBoard->placeFigure(*mockIChessMan, {5, 5});
    waitForFinish();

    // remove id:40{5,5}
    EXPECT_CALL(*mockNotifier, removed(Eq(40), Eq(Coordinate{5,5}))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t id, const Coordinate &to) {
                waitFinished();
            }));
    mBoard->removeFigure(*mockIChessMan);
    waitForFinish();

    // place id:60{5,5}
    EXPECT_CALL(*mockNotifier, placed(Eq(60), Eq(Coordinate{5,5}))).Times(Exactly(1))
            .WillOnce(Invoke([&](std::int32_t id, const Coordinate &to) {
                waitFinished();
            }));
    mBoard->placeFigure(*mockIChessMan, {5, 5});
    waitForFinish();
}

TEST_F(ChessBoardTest, removeFigure_NormalWithWaiting)
{
    EXPECT_CALL(*mockIChessMan, setCurrentCoordinate).Times(Exactly(0));
    EXPECT_CALL(*mockIChessMan, getCurrentCoordinate).Times(Exactly(0));
    EXPECT_CALL(*mockIChessMan, getID).Times(Exactly(2))
            .WillOnce(Return(50))
            .WillOnce(Return(60));

    EXPECT_CALL(*mockNotifier,moved(_, _, _)).Times(Exactly(0));
    EXPECT_CALL(*mockNotifier,removed(_, _)).Times(Exactly(0));

    // place id:50 {5,5}
    EXPECT_CALL(*mockNotifier,placed(Eq(50), Eq(Coordinate{5,5}))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t, const Coordinate &to) {
                waitFinished();
            }));
    mBoard->placeFigure(*mockIChessMan, {5,5});
    waitForFinish();

    // place id:60 {5,5}
    EXPECT_CALL(*mockNotifier,waitingForCell(Eq(60), invalidCoordinate, Eq(Coordinate{5,5}))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t, Coordinate, const Coordinate) {
                waitFinished();
            }));
    mBoard->placeFigure(*mockIChessMan, {5,5});
    waitForFinish();

}

TEST_F(ChessBoardTest, cancelMoveFigure_normal)
{
    EXPECT_CALL(*mockIChessMan, getID)
            .Times(Exactly(5))
            .WillOnce(Return(10))
            .WillOnce(Return(20))
            .WillOnce(Return(20))
            .WillOnce(Return(10))
            .WillOnce(Return(20));
    EXPECT_CALL(*mockIChessMan, getCurrentCoordinate)
            .Times(Exactly(3))
            .WillOnce(ReturnRefOfCopy(Coordinate{0, 0}))
            .WillOnce(ReturnRefOfCopy(Coordinate{5, 5}))
            .WillOnce(ReturnRefOfCopy(Coordinate{0, 0}));

    // place id:10 to {5,5}
    EXPECT_CALL(*mockNotifier, placed(Eq(10), Eq(Coordinate{5,5}))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t id, const Coordinate &to) {
                waitFinished();
            }));
    mBoard->placeFigure(*mockIChessMan, {5, 5});
    waitForFinish();

    // place id:20 to {0,0}
    EXPECT_CALL(*mockNotifier, placed(Eq(20), Eq(Coordinate{0,0}))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t id, const Coordinate &) {
                waitFinished();
            }));
    mBoard->placeFigure(*mockIChessMan, {0, 0});
    waitForFinish();

    // move from id:20 {0,0} to {5,5}
    EXPECT_CALL(*mockNotifier, waitingForCell(Eq(20), Eq(Coordinate{0, 0}), Eq(Coordinate{5, 5}))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t id, const Coordinate &, const Coordinate &) {
                waitFinished();
            }));
    mBoard->moveFigure(*mockIChessMan, {5,5});
    waitForFinish();

    // move from id:10 5,5 to 0,0
    EXPECT_CALL(*mockNotifier, waitingForCell(Eq(10), Eq(Coordinate{5, 5}), Eq(Coordinate{0, 0}))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t id, const Coordinate &, const Coordinate &) {
                waitFinished();
            }));
    mBoard->moveFigure(*mockIChessMan, {0, 0});
    waitForFinish();

    // cancel from id:20 {0,0} to {5,5}
    EXPECT_CALL(*mockNotifier, cancelMoved(Eq(20), Eq(Coordinate{0, 0}), Eq(Coordinate{5, 5}))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t id, const Coordinate &, const Coordinate &) {
                waitFinished();
            }));
    mBoard->cancelMoveFigure(*mockIChessMan, {5, 5});
    waitForFinish();
}

TEST_F(ChessBoardTest, cancelMoveFigure_waiterNotFound)
{
    EXPECT_CALL(*mockIChessMan, getID)
            .Times(Exactly(3))
            .WillOnce(Return(10))
            .WillOnce(Return(20))
            .WillOnce(Return(20));
    EXPECT_CALL(*mockIChessMan, getCurrentCoordinate)
            .Times(Exactly(1))
            .WillOnce(ReturnRefOfCopy(Coordinate{0, 0}));

    // place id:10 to {5,5}
    EXPECT_CALL(*mockNotifier, placed(Eq(10), Eq(Coordinate{5,5}))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t id, const Coordinate &to) {
                waitFinished();
            }));
    mBoard->placeFigure(*mockIChessMan, {5, 5});
    waitForFinish();

    // place id:20 to {0,0}
    EXPECT_CALL(*mockNotifier, placed(Eq(20), Eq(Coordinate{0,0}))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t id, const Coordinate &) {
                waitFinished();
            }));
    mBoard->placeFigure(*mockIChessMan, {0, 0});
    waitForFinish();

    // cancel from id:20 {0,0} to {5,5}
    EXPECT_CALL(*mockNotifier, reject(Eq(20), Eq(board::ReasonReject::waiterNotFound))).Times(Exactly(1))
            .WillRepeatedly(Invoke([&](std::int32_t id, board::ReasonReject) {
                waitFinished();
            }));
    mBoard->cancelMoveFigure(*mockIChessMan, {5, 5});
    waitForFinish();
}
