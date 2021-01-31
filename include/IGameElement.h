#pragma once

#include "RemoveCopyMove.h"

class IGameElement : public virtual RemoveCopyMove
{
public:
    virtual ~IGameElement() = default;

    virtual void startGame() = 0;
    virtual void stopGame() = 0;
};