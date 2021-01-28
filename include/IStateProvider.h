#pragma once

#include <memory>

class IStateProvider {
public:
    virtual ~IStateProvider() = default;
    virtual std::shared_ptr<IState> makeExit() = 0;
    virtual std::shared_ptr<IState> makeDoStepState() = 0;
    virtual std::shared_ptr<IState> makeWaitNextStepState() = 0;
    virtual std::shared_ptr<IState> makeWaitBlock() = 0;
};
