cmake_minimum_required(VERSION 3.10)

project(ChessRook
        LANGUAGES CXX)

find_package(Threads REQUIRED)

set(SOURCES
        src/main.cpp
        src/TreadBase.cpp
        src/ChessBoardImpl.cpp
        src/ChessManImpl.cpp
        src/GameRules.cpp
        src/ParticipantGame.cpp
        src/Logger.cpp
        src/Game.cpp
        src/state/NextStepState.cpp
        src/state/StopState.cpp
        src/state/WaitForCellStep.cpp
        src/state/WaitForConfirmStep.cpp
)

include_directories(include)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} Threads::Threads)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
