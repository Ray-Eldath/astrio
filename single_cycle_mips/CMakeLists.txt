add_executable(sim_pc verilator/sim_pc.cpp)
add_executable(sim_alu verilator/sim_alu.cpp)
add_executable(sim_registers verilator/sim_registers.cpp)
add_executable(sim_fetcher verilator/sim_fetcher.cpp)

set(VERILATOR_DEFAULT_IMPORT constants.sv types.sv)

verilate(sim_pc
        TRACE
        SOURCES ${VERILATOR_DEFAULT_IMPORT} pc_type.sv pc.sv
        INCLUDE_DIRS "."
        PREFIX PC)

verilate(sim_alu
        TRACE
        SOURCES ${VERILATOR_DEFAULT_IMPORT} alu_type.sv alu.sv
        INCLUDE_DIRS "."
        PREFIX ALU)

verilate(sim_registers
        TRACE
        SOURCES ${VERILATOR_DEFAULT_IMPORT} registers.sv
        INCLUDE_DIRS "."
        PREFIX Registers)

verilate(sim_fetcher
        TRACE
        SOURCES ${VERILATOR_DEFAULT_IMPORT} fetcher.sv
        INCLUDE_DIRS "."
        PREFIX Fetcher)