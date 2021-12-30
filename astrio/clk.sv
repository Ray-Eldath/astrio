`timescale 1ns / 1ps
 
module ClockDivider #(
    parameter DIVISOR_WIDTH
)  (
    input clk,
    output clk_en
);
    
    logic [(DIVISOR_WIDTH - 1):0] cnt = 0;
    always_ff @(posedge clk) cnt <= cnt + 1;
    
    assign clk_en = cnt == 0;
endmodule