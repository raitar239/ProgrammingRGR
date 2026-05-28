cmake_minimum_required(VERSION 3.15)
project(TexasHoldem CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(poker
    main.cpp
    core/utils.cpp
    core/logger.cpp
    core/config.cpp
    engine/poker_engine.cpp
    engine/ai_player.cpp
    engine/texas_holdem.cpp
    ui/leaderboard.cpp
    ui/menu.cpp
)

target_include_directories(poker PRIVATE ${CMAKE_SOURCE_DIR})

# Tests
find_package(GTest QUIET)
add_executable(run_tests tests/test_poker.cpp engine/poker_engine.cpp)
target_include_directories(run_tests PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(run_tests GTest::gtest GTest::gtest_main)
