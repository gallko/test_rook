#pragma once

#include <memory>
#include "RemoveCopyMove.h"

class IStateProvider;

class IState : public RemoveCopyMove
{
public:
    explicit IState(std::shared_ptr<IStateProvider> stateProvider)
        : mStateProvider(std::move(stateProvider))
    {

    }

    ~IState() override = default;

    virtual std::shared_ptr<IState> doWork() = 0;

protected:
    std::shared_ptr<IStateProvider> mStateProvider;
};
