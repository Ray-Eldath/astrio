import Types::*;
import Parameters::*;

module Fetcher(
    input addr_t addr,
    input bit clk,
    output inst_t inst
);
    inst_reg_t insts [InstStartFrom+InstSpace:InstStartFrom];
    initial $readmemh("inst.data", insts);

    always_ff @(posedge clk)
        inst <= insts[addr];
endmodule : Fetcher