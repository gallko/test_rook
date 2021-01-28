#pragma once

#include "IState.h"

class IStateProvider;
namespace resource {
    class ISystemResource;
}

class WaitNextStepState : public IState
{
public:
    WaitNextStepState(std::shared_ptr<IStateProvider> stateProvider,
                      std::shared_ptr<resource::ISystemResource> systemResource);
    ~WaitNextStepState() override = default;

    std::shared_ptr<IState> doWork() override;

private:
    std::shared_ptr<resource::ISystemResource> mSystemResource;
};
