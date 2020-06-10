cmake_minimum_required(VERSION 3.12)
project(astrio)

set(CMAKE_CXX_STANDARD 17)

find_package(verilator REQUIRED HINTS $ENV{VERILATOR_ROOT} ${VERILATOR_ROOT})
if (NOT verilator_FOUND)
    message(FATAL_ERROR "Verilator was not found. Either install it, or set the VERILATOR_ROOT environment variable")
endif ()
set(verilator_INCLUDE_DIR ${VERILATOR_ROOT}/include)
find_package(althas REQUIRED)

add_subdirectory(single_cycle_mips)