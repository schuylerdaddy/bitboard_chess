cmake_minimum_required(VERSION 3.12) # version can be different

project(chess_ai) #name of your project

set(CMAKE_CXX_STANDARD 20)

FILE(GLOB_RECURSE BOARD src/board/*.cpp)
FILE(GLOB_RECURSE TEST src/test/*.cpp)

include_directories(src/include)

# engine_lib

add_library(chess_engine STATIC ${BOARD})
add_library(chess_engine_test STATIC ${BOARD})
target_compile_definitions(chess_engine_test PRIVATE UNITTEST=1)

# main exe
add_executable(chess_ai_main main.cpp) # add this executable
target_link_libraries(chess_ai_main chess_engine_test) # link google test to this executable
target_compile_definitions(chess_ai_main PRIVATE UNITTEST=1)

# test data
add_subdirectory(googletest) # add googletest subdirectory

include_directories(googletest/include) # this is so we can #include <gtest/gtest.h>

add_executable(tests ${TEST}) # add this executable

target_link_libraries(tests PRIVATE gtest chess_engine_test) # link google test to this executable
target_compile_definitions(tests PRIVATE UNITTEST=1)