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
    inst_reg_t insts [(InstStartFrom+InstSpace-1)>>2:InstStartFrom>>2];

    addr_t addr_shifted;
    assign addr_shifted = addr >> 2;

    always_ff @(posedge clk) begin
        inst <= chip_select == 0 ? 32'b0:insts[addr_shifted];

        if (load == 1)
            insts[addr_shifted] <= load_inst;
    end
endmodule : Fetcher