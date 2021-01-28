cmake_minimum_required(VERSION 3.10)

project(ChessRook
        LANGUAGES CXX)

find_package(Threads REQUIRED)

find_package(GTest)
#find_package(GMock)
if (GTEST_FOUND)
    enable_testing()
    add_subdirectory(test)
endif ()

set(SOURCES
        src/main.cpp
        src/ChessBoardImpl.cpp
        src/ChessRookImpl.cpp
        src/GameRulesImpl.cpp
        src/ParticipantGame.cpp
)

add_definitions(-DSIZE_BOARD=8)

include_directories(include)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} Threads::Threads)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
