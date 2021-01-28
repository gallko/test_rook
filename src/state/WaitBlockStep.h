#pragma once

#include "IState.h"

class WaitBlockStep : public IState
{
public:
    WaitBlockStep(std::shared_ptr<IStateProvider> stateProvider);
    ~WaitBlockStep() override = default;

    std::shared_ptr<IState> doWork() override;
};
