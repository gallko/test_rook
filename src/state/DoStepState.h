#pragma once

#include "IState.h"

class IStateProvider;
class IGameRules;
namespace board {
    class IChessBoard;
}
namespace chessman {
    class IChessMan;
}

class DoStepState : public IState
{
public:
    DoStepState(std::shared_ptr<IStateProvider> stateProvider,
                std::shared_ptr<board::IChessBoard> board,
                std::shared_ptr<IGameRules> gameRules,
                std::shared_ptr<chessman::IChessMan> chessMan);
    ~DoStepState() override = default;

    std::shared_ptr<IState> doWork() override;

private:
    std::shared_ptr<board::IChessBoard> mBoard;
    std::shared_ptr<IGameRules> mGameRules;
    std::shared_ptr<chessman::IChessMan> mChessMan;
};
