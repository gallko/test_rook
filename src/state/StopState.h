#pragma once

#include "../ParticipantGame.h"

class StopState : public ParticipantGame::IState
{
public:
    StopState() = default;
    ~StopState() override = default;

    std::chrono::milliseconds waitPeriod() const override;
    std::unique_ptr<ParticipantGame::IState> doWork(std::unique_ptr<ParticipantGame::Event> ptr) override;
    bool stop() const override;

private:
};

