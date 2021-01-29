#pragma once
#include <thread>
#include <string>

#include "RemoveCopyMove.h"


class TreadBase : public RemoveCopyMove
{
public:
    explicit TreadBase(std::string name);
    ~TreadBase() override;

    void start();
    void cancel();

protected:
    virtual void onStart() {};
    virtual void loop() = 0;
    virtual void onStop() {};

private:
    void mainLoop();

    std::unique_ptr<std::thread> mTread;
    const std::string mName;
};
