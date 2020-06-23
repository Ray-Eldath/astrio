# PC
add_executable(sim_pc verilator/sim_pc.cpp)
# ALU
add_executable(sim_alu verilator/sim_alu.cpp)
# Registers
add_executable(sim_registers verilator/sim_registers.cpp)
# Instruction Fetcher
add_executable(sim_fetcher verilator/sim_fetcher.cpp)
target_include_directories(sim_fetcher PRIVATE ../assembler)
# Data Storage
add_executable(sim_memory verilator/sim_memory.cpp)
# CPU
add_executable(sim_cpu verilator/sim_cpu.cpp)
target_include_directories(sim_cpu PRIVATE ../assembler)

#
# verilate
#

set(VERILATOR_INCLUDES constants.sv types.sv)
set(VERILATOR_INCLUDE_TYPES pc_type.sv alu_type.sv mux_type.sv cpu_type.sv)

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

verilate(sim_memory
        TRACE
        SOURCES ${VERILATOR_INCLUDES} memory.sv
        INCLUDE_DIRS "."
        PREFIX Memory)

verilate(sim_cpu
        TRACE
        SOURCES ${VERILATOR_INCLUDES} ${VERILATOR_INCLUDE_TYPES} pc.sv fetcher.sv registers.sv memory.sv alu.sv alu_controller.sv op_mux.sv cpu.sv
        INCLUDE_DIRS "."
        PREFIX CPU)