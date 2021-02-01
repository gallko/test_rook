#include "StopState.h"

std::chrono::milliseconds StopState::waitPeriod() const
{
    return std::chrono::milliseconds();
}

std::unique_ptr<ParticipantGame::IState> StopState::doWork(std::unique_ptr<ParticipantGame::Event> ptr)
{
    return nullptr;
}

bool StopState::stop() const {
    return true;
}
