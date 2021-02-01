#include "iostream"
#include "WaitForConfirmStep.h"
#include "StopState.h"
#include "NextStepState.h"
#include "IChessMan.h"
#include "../GameRules.h"
#include "WaitForCellStep.h"

WaitForConfirmStep::WaitForConfirmStep(std::shared_ptr<board::IChessBoard> board,
                                       std::shared_ptr<chessman::IChessMan> chessMan)
        : mBoard(std::move(board))
        , mChessMan(std::move(chessMan))
{

}


std::chrono::milliseconds WaitForConfirmStep::waitPeriod() const {
    return GameRules::generateDelayConfirm();
}

std::unique_ptr<ParticipantGame::IState> WaitForConfirmStep::doWork(std::unique_ptr<ParticipantGame::Event> ptr)
{
    std::unique_ptr<ParticipantGame::IState> result;
    if (ptr) {
        switch (ptr->mTypeEvent) {
            case ParticipantGame::Event::Type::stop:
                mBoard->removeFigure(*mChessMan);
                result = std::make_unique<StopState>();
                break;
            case ParticipantGame::Event::Type::placed:
            case ParticipantGame::Event::Type::moved:
                mChessMan->setCurrentCoordinate(ptr->mToCoordinate);
                result = std::make_unique<NextStepState>(mBoard, mChessMan);
                break;
            case ParticipantGame::Event::Type::cancelMoved:
                result = std::make_unique<WaitForConfirmStep>(mBoard, mChessMan);
                mBoard->moveFigure(*mChessMan, GameRules::generateStep(*mChessMan));
                break;
            case ParticipantGame::Event::Type::waitingForCell:
                result = std::make_unique<WaitForCellStep>(mBoard, mChessMan, ptr->mToCoordinate);
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
        }
    } else {
        std::cerr << "Performance issue.\n";
        result = std::make_unique<WaitForConfirmStep>(mBoard, mChessMan);
    }

    return result;
}

bool WaitForConfirmStep::stop() const {
    return false;
}
