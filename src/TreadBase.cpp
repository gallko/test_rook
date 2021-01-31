#include "TreadBase.h"

TreadBase::TreadBase(std::string name)
    : mTread(nullptr)
    , mName(std::move(name))
{

}

void TreadBase::start()
{
    if (!mTread)
    {
        mTread = std::make_unique<std::thread>(&TreadBase::mainLoop, this);
        pthread_setname_np(mTread->native_handle(), mName.c_str());
    }
}

void TreadBase::cancel()
{
    pthread_cancel(mTread->native_handle());
}


void TreadBase::mainLoop()
{
    onStart();
    loop();
    onStop();
}

void TreadBase::join()
{
    mTread->join();
}