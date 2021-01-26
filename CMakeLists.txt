cmake_minimum_required(VERSION 3.10)

project(test_rook
        LANGUAGES CXX)

find_package(Threads REQUIRED)

set(SOURCES
        src/main.cpp
)

include_directories(include)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} Threads::Threads)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
