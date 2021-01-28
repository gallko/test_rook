#include "WaitNextStepState.h"
#include "ISystemResource.h"
#include "IStateProvider.h"


WaitNextStepState::WaitNextStepState(std::shared_ptr<IStateProvider> stateProvider,
                                     std::shared_ptr<resource::ISystemResource> systemResource)
    : IState(std::move(stateProvider))
    , mSystemResource(std::move(systemResource))
{

}

std::shared_ptr<IState> WaitNextStepState::doWork()
{
    std::shared_ptr<IState> next_state;
    auto time = mSystemResource->random(200, 300);

    switch (mSystemResource->sleep(time))
    {
        case resource::ReasonWakeUp::externalWakeUp: // behavior is undefined
        case resource::ReasonWakeUp::exit:
            next_state = mStateProvider->makeExit();
            break;
        case resource::ReasonWakeUp::timeOut:
            next_state = mStateProvider->makeDoStepState();
            break;
    }

    return next_state;
}
