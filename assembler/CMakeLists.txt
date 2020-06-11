cmake_minimum_required(VERSION 3.12)
project(assembler)

set(CMAKE_CXX_STANDARD 17)

add_executable(test_assembler test_assembler.cpp)
target_include_directories(test_assembler PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})