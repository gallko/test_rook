project(TestChessRook)

find_package(Threads REQUIRED)

file(GLOB TEST_SOURCES
        ./testBoard.cpp
        ./testGameRules.cpp
        testChess.cpp

        ../src/TreadBase.cpp
        ../src/ChessBoardImpl.cpp
        ../src/ChessManImpl.cpp
        ../src/GameRules.cpp
        ../src/ParticipantGame.cpp
#        ../src/state/DoStepState.cpp
#        ../src/state/WaitNextStepState.cpp
#        ../src/state/WaitBlockStep.h
)

include_directories(
        ../include
        ../src
)

add_definitions(-DTEST_BUILD)
add_executable(${PROJECT_NAME} ${TEST_SOURCES})

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} Threads::Threads gtest gmock gmock_main)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)