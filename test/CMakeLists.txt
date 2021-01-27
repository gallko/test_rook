project(TestChessRook)

find_package(Threads REQUIRED)

file(GLOB TEST_SOURCES
        ./*.cpp
        ../src/ChessBoardImpl.cpp)

include_directories(
        ../include
        ../src
)

add_definitions(-DSIZE_BOARD=8 -DTEST_BUILD)
add_executable(${PROJECT_NAME} ${TEST_SOURCES})

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} Threads::Threads gtest gmock gmock_main)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)