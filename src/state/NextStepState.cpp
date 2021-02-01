#include <iostream>
#include "../GameRules.h"
#include "NextStepState.h"
#include "WaitForConfirmStep.h"
#include "StopState.h"

NextStepState::NextStepState(std::shared_ptr<board::IChessBoard> board, std::shared_ptr<chessman::IChessMan> chessMan)
    : mBoard(std::move(board))
    , mChessMan(std::move(chessMan))
{

}

std::chrono::milliseconds NextStepState::waitPeriod() const
{
    return GameRules::generateDelayWaitNextStep();
}

std::unique_ptr<ParticipantGame::IState> NextStepState::doWork(std::unique_ptr<ParticipantGame::Event> ptr)
{
    std::unique_ptr<ParticipantGame::IState> result;
    if (!ptr) {
        result = std::make_unique<WaitForConfirmStep>(mBoard, mChessMan);
        mBoard->moveFigure(*mChessMan, GameRules::generateStep(*mChessMan));
    } else {
        if (ptr->mTypeEvent == ParticipantGame::Event::Type::stop ||
            ptr->mTypeEvent == ParticipantGame::Event::Type::reject && ptr->mReasonReject == board::ReasonReject::boardStopped)
        {
            result = std::make_unique<StopState>();
            mBoard->removeFigure(*mChessMan);
        } else {
            throw std::logic_error("Unexpected event in NextStep class");
        }
    }

    return result;
}

bool NextStepState::stop() const {
    return false;
}
