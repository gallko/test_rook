#pragma once

#include "../ParticipantGame.h"

class WaitForConfirmStep : public ParticipantGame::IState
{
public:
    WaitForConfirmStep(std::shared_ptr<board::IChessBoard> board,
                       std::shared_ptr<chessman::IChessMan> chessMan);
    ~WaitForConfirmStep() override = default;

    std::chrono::milliseconds waitPeriod() const override;
    std::unique_ptr<ParticipantGame::IState> doWork(std::unique_ptr<ParticipantGame::Event> ptr) override;

    bool stop() const override;

private:
    std::shared_ptr<board::IChessBoard> mBoard;
    std::shared_ptr<chessman::IChessMan> mChessMan;
};
