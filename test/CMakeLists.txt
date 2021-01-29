project(TestChessRook)

find_package(Threads REQUIRED)

file(GLOB TEST_SOURCES
        ./testBoard.cpp
        ../src/TreadBase.cpp
        ../src/ChessBoardImpl.cpp
#        ../src/ChessRookImpl.cpp
#        ../src/GameRulesImpl.cpp
#        ../src/ParticipantGame.cpp
#        ../src/state/DoStepState.cpp
#        ../src/state/WaitNextStepState.cpp
#        ../src/state/WaitBlockStep.h
)

include_directories(
        ../include
        ../src
)

add_definitions(-DSIZE_BOARD=8 -DTEST_BUILD)
add_executable(${PROJECT_NAME} ${TEST_SOURCES})

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} Threads::Threads gtest gmock gmock_main)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)