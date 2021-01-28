#pragma once

class IState {
public:
    std::shared_ptr<IState> doWork();
    bool isEnd();
};
