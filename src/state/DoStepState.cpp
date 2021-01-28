#include "DoStepState.h"
#include "IState.h"
#include "IStateProvider.h"
#include "IChessBoard.h"
#include "IGameRules.h"
#include "IChessMan.h"

DoStepState::DoStepState(std::shared_ptr<IStateProvider> stateProvider,
                         std::shared_ptr<board::IChessBoard> board,
                         std::shared_ptr<IGameRules> gameRules,
                         std::shared_ptr<chessman::IChessMan> chessMan)
    : IState(std::move(stateProvider))
    , mBoard(std::move(board))
    , mGameRules(std::move(gameRules))
    , mChessMan(std::move(chessMan))
{

}

std::shared_ptr<IState> DoStepState::doWork()
{
    std::shared_ptr<IState> next_state;
    auto coordinate = mGameRules->generateStep(mChessMan);

    switch (mBoard->moveFigure(mChessMan, coordinate)) {
        case board::ErrorCode::error:
            next_state = mStateProvider->makeExit();
            break;
        case board::ErrorCode::success:
            next_state = mStateProvider->makeWaitNextStepState();
            break;
        case board::ErrorCode::occupied_place:
            next_state = mStateProvider->makeWaitBlock();
            break;
    }

    return next_state;
}

