#include <iostream>
#include <vector>
#include <memory>
#include "ChessBoardImpl.h"
#include "GameRules.h"
#include "ParticipantGame.h"
#include "Coordinate.h"
#include "ChessManImpl.h"

#include "utils_log.h"

int main(int argc, char **argv) {

    print_help();

    auto board = std::make_shared<ChessBoardImpl>(GameRules::sizeBoard());
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
    std::this_thread::sleep_for(10s);
    participantGame->stopGame();
    participantGame2->stopGame();
    participantGame3->stopGame();
    participantGame4->stopGame();
    std::this_thread::sleep_for(1s);
    pthread_barrier_destroy(&barrier);
    return EXIT_SUCCESS;
}