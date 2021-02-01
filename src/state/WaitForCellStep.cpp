#include "WaitForCellStep.h"
#include "WaitForConfirmStep.h"
#include "StopState.h"
#include "IChessMan.h"
#include "NextStepState.h"
#include "../GameRules.h"

WaitForCellStep::WaitForCellStep(std::shared_ptr<board::IChessBoard> board,
                                 std::shared_ptr<chessman::IChessMan> chessMan,
                                 board::Coordinate toCoordinate)
        : mBoard(std::move(board))
        , mChessMan(std::move(chessMan))
        , mToCoordinate(std::move(toCoordinate))
{

}

std::chrono::milliseconds WaitForCellStep::waitPeriod() const
{
    return GameRules::generateDelayWaitForCell();
}


std::unique_ptr<ParticipantGame::IState> WaitForCellStep::doWork(std::unique_ptr<ParticipantGame::Event> ptr)
{
    std::unique_ptr<ParticipantGame::IState> result;
    if (ptr)
    {
        switch (ptr->mTypeEvent) {
            case ParticipantGame::Event::Type::stop:
                result = std::make_unique<StopState>();
                mBoard->cancelMoveFigure(*mChessMan, mToCoordinate);
                mBoard->removeFigure(*mChessMan);
                break;
            case ParticipantGame::Event::Type::placed:
            case ParticipantGame::Event::Type::moved:
                mChessMan->setCurrentCoordinate(ptr->mToCoordinate);
                result = std::make_unique<NextStepState>(mBoard, mChessMan);
                break;
            case ParticipantGame::Event::Type::remove:
                result = std::make_unique<StopState>();
                break;
            case ParticipantGame::Event::Type::reject:
                switch (ptr->mReasonReject) {
                    case board::ReasonReject::boardStopped:
                        result = std::make_unique<StopState>();
                        break;
                    case board::ReasonReject::incorrectCoordinate:
                    case board::ReasonReject::idMismatch:
                    case board::ReasonReject::incorrectId:
                    case board::ReasonReject::duplicateId:
                    case board::ReasonReject::waiterNotFound:
                    case board::ReasonReject::empty:
                        throw std::logic_error("Unexpected ReasonReject in WaitForCellStep class");
                }
                break;
            case ParticipantGame::Event::Type::cancelMoved:
            case ParticipantGame::Event::Type::waitingForCell:
                throw std::logic_error("Unexpected event in WaitForCellStep class");
        }
    } else {
        result = std::make_unique<WaitForConfirmStep>(mBoard, mChessMan);
        mBoard->cancelMoveFigure(*mChessMan, mToCoordinate);
    }

    return result;
}

bool WaitForCellStep::stop() const
{
    return false;
}
