#include <iostream>
#include <memory>
#include "Game.h"

int main(int argc, char **argv) {
    auto game = std::make_shared<Game>(4, 30);

    game->startGame();
    game->waitEnd();

    return EXIT_SUCCESS;
}