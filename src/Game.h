#pragma once

#include <functional>
#include <memory>
#include <vector>

namespace board {
    class IChessBoard;
}
class IGameElement;

class Game final
{
public:
    Game(size_t countParticipants, size_t countSteps);

    void startGame();
    void stopGame();
    void waitEnd();

private:
    std::vector<std::shared_ptr<IGameElement>> mGameElements;
    size_t mCountParticipants;
    size_t mCountSteps;
    bool mStartGame;
    std::shared_ptr<pthread_barrier_t> mStartBarrier, mEndBarrier;
};

