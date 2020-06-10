# PC
add_executable(sim_pc verilator/sim_pc.cpp)
# ALU
add_executable(sim_alu verilator/sim_alu.cpp)
# Registers
add_executable(sim_registers verilator/sim_registers.cpp)
# Instruction Fetcher
add_executable(sim_fetcher verilator/sim_fetcher.cpp)
file(COPY inst.data
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
# Data Storage
add_executable(sim_storage verilator/sim_storage.cpp)

#
# verilate
#

set(VERILATOR_INCLUDES constants.sv types.sv)

verilate(sim_pc
        TRACE
        SOURCES ${VERILATOR_INCLUDES} pc_type.sv pc.sv
        INCLUDE_DIRS "."
        PREFIX PC)

verilate(sim_alu
        TRACE
        SOURCES ${VERILATOR_INCLUDES} alu_type.sv alu.sv
        INCLUDE_DIRS "."
        PREFIX ALU)

verilate(sim_registers
        TRACE
        SOURCES ${VERILATOR_INCLUDES} registers.sv
        INCLUDE_DIRS "."
        PREFIX Registers)

verilate(sim_fetcher
        TRACE
        SOURCES ${VERILATOR_INCLUDES} fetcher.sv
        INCLUDE_DIRS "."
        PREFIX Fetcher)

verilate(sim_storage
        TRACE
        SOURCES ${VERILATOR_INCLUDES} storage.sv
        INCLUDE_DIRS "."
        PREFIX Storage)