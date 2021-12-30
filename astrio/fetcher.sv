`timescale 1ns / 1ps

import Types::*;
import Parameters::*;

module Fetcher(
    input addr_t addr,
    input inst_t load_inst,
    input bit load,
    input bit chip_select,
    input bit rst,
    input bit clk,
    output inst_t inst
);
    addr_t addr_shifted, addra;
    assign addr_shifted = addr >> 2;

    inst_t bram_inst;
//    blk_mem_gen_0 bram_insts (
//        .clka(clk),    // input wire clka
//        .ena(chip_select),      // input wire ena
//        .addra(addr_shifted),  // input wire [9 : 0] addra
//        .douta(bram_inst)  // output wire [31 : 0] douta
//    );

    inst_t insts [(InstStartFrom+InstSpace-1)>>2:InstStartFrom>>2];
    assign inst = (rst == 1 || chip_select == 0) ? 32'b0: insts[addr_shifted];

//    assign inst = chip_select == 0 ? 32'b0 : bram_inst;

    var int i;
    initial begin
        $readmemh("insts.mem", insts);
        for (i = 0; i < 10; i++)
            $display("%h", insts[i]);
    end
/*    always_ff @(posedge clk)
        if (load == 1)
            insts[addr_shifted] <= load_inst;
*/
endmodule : Fetcher