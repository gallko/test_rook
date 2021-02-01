#include <iostream>
#include <vector>
#include <memory>
#include "ChessBoardImpl.h"
#include "GameRules.h"
#include "ParticipantGame.h"
#include "Coordinate.h"
#include "ChessManImpl.h"
#include "Logger.h"

int main(int argc, char **argv) {

    auto logger = std::make_shared<Logger>(std::cout);
    logger->startGame();

    auto board = std::make_shared<ChessBoardImpl>(GameRules::sizeBoard());
    board->addNotifier(logger);
    board->startGame();

    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, nullptr, 4);

    auto participantGame = std::make_shared<ParticipantGame>(board, &barrier);
    auto participantGame2 = std::make_shared<ParticipantGame>(board, &barrier);
    auto participantGame3 = std::make_shared<ParticipantGame>(board, &barrier);
    auto participantGame4 = std::make_shared<ParticipantGame>(board, &barrier);

    participantGame->startGame();
    participantGame2->startGame();
    participantGame3->startGame();
    participantGame4->startGame();

    using namespace std::chrono;
    std::this_thread::sleep_for(20s);
    std::cout << "exit\n";
    return EXIT_SUCCESS;
}