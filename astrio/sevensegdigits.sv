`timescale 1ns / 1ps

package DigitType;
    typedef logic unsigned [3:0] digit_t;
endpackage

import DigitType::*;

module SevenSegDigits(
    input digit_t [7:0] digits,
    input bit [7:0] en = 8'hff,
    input slow_clk, // 5MHz
    output CA, CB, CC, CD, CE, CF, CG, DP, AN0, AN1, AN2, AN3, AN4, AN5, AN6, AN7
);
    logic current_en, clock_en;
    digit_t current_digit, i = 7;
    assign current_digit = digits[i];
    assign current_en = en[i];
    logic [7:0] cathodes;
    assign cathodes = 1 << i; 
    
    Digit d(.digit(current_digit), .en(current_en), .CA(CA), .CB(CB), .CC(CC), .CD(CD), .CE(CE), .CF(CF), .CG(CG), .DP(DP));
    ClockDivider #(16) cd(.clk(slow_clk), .clk_en(clk_en));
    
    always @(posedge slow_clk) if (clk_en) i <= (i > 0) ? i - 1 : 7;

    assign {AN7, AN6, AN5, AN4, AN3, AN2, AN1, AN0} = ~cathodes;
endmodule

module Digit(
    input digit_t digit,
    input bit en = 1,
    output CA, CB, CC, CD, CE, CF, CG, DP
);
    logic [7:0] SevenSeg;

    always_comb begin
        if (en)
            unique case (digit)
                4'h0: SevenSeg = 8'b00000011;
                4'h1: SevenSeg = 8'b10011111;
                4'h2: SevenSeg = 8'b00100101;
                4'h3: SevenSeg = 8'b00001101;
                4'h4: SevenSeg = 8'b10011001;
                4'h5: SevenSeg = 8'b01001001;
                4'h6: SevenSeg = 8'b01000001;
                4'h7: SevenSeg = 8'b00011111;
                4'h8: SevenSeg = 8'b00000001;
                4'h9: SevenSeg = 8'b00001001;
                default: SevenSeg = 8'hff;
            endcase
        else SevenSeg = 8'hff;
    end
    
    assign {CA, CB, CC, CD, CE, CF, CG, DP} = SevenSeg;
endmodule