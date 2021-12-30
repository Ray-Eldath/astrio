`timescale 1ns / 1ps

import Types::*;
import Parameters::*;
import DigitType::*;

module Registers(
    input reg_id_t read1,
    input reg_id_t read2,
    input bit enable_write,
    input reg_id_t write_id,
    input op_t write_data,
    input bit clk,
    output op_t read1_out,
    output op_t read2_out,
    output logic CA, CB, CC, CD, CE, CF, CG, DP, AN0, AN1, AN2, AN3, AN4, AN5, AN6, AN7
);
    bit out_write_data_pre;
    op_t regs [31:0], reg_k0;

    var int i;
    initial begin
        for (i = 0; i < 32; i++)
            regs[i] = 32'b0;  
    end

    always_ff @(posedge clk)
        if (enable_write == 1 && write_id != 0)
            regs[write_id] <= write_data;

    assign out_write_data_pre = enable_write && write_id != 0;

    assign read1_out = out_write_data_pre && write_id == read1 ? write_data:regs[read1];
    assign read2_out = out_write_data_pre && write_id == read2 ? write_data:regs[read2];
    
    // SevenSegDigits
    assign reg_k0 = regs[26];
    logic [7:0] en;
    logic [31:0] cnt = 0;
    digit_t [7:0] digits;
    SevenSegDigits ds(.digits(digits), .en(en), .slow_clk(clk), .CA(CA), .CB(CB), .CC(CC), .CD(CD), .CE(CE), .CF(CF), .CG(CG), .DP(DP), .AN0(AN0), .AN1(AN1), .AN2(AN2), .AN3(AN3), .AN4(AN4), .AN5(AN5), .AN6(AN6), .AN7(AN7));
    
    assign digits[0] = reg_k0 % 10;
    assign digits[1] = reg_k0 / 10 % 10;
    assign digits[2] = reg_k0 / 100 % 10;
    assign digits[3] = reg_k0 / 1000;

    always_comb
        if (reg_k0 < 10)
            en <= 8'h01;
        else if (reg_k0 < 100)
            en <= 8'h03;
        else if (reg_k0 < 1000)
            en <= 8'h07;
        else
            en <= 8'h0f;

endmodule : Registers