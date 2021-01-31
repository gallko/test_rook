#pragma once
#include <thread>
#include <string>

#include "RemoveCopyMove.h"


class TreadBase : public virtual RemoveCopyMove
{
public:
    explicit TreadBase(std::string name);
    ~TreadBase() override = default;

protected:
    void start();
    void cancel();

    virtual void onStart() {};
    virtual void loop() = 0;
    virtual void onStop() {};

    void join();
private:
    void mainLoop();

    std::unique_ptr<std::thread> mTread;
    const std::string mName;
};
