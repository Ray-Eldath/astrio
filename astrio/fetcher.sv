import Types::*;
import Parameters::*;

module Fetcher(
    input addr_t addr,
    input inst_t load_inst,
    input bit load,
    input bit chip_select,
    input bit clk,
    output inst_t inst
);
    inst_t insts [(InstStartFrom+InstSpace-1)>>2:InstStartFrom>>2];

    addr_t addr_shifted;
    assign addr_shifted = addr >> 2;
    assign inst = chip_select == 0 ? 32'b0:insts[addr_shifted];

    // instruction loading falicity is designed for loading instructions
    // directly programmably, i.e. infill instructions in C++. this facility
    // may be deprecated in actual on-board experiment since instruction loading
    // is handled by UART, PS-PL interconnection or other peripherals.
    always_ff @(posedge clk)
        if (load == 1)
            insts[addr_shifted] <= load_inst;

endmodule : Fetcher