import Types::*;
import Parameters::*;

module Registers(
    input reg_id_t read1,
    input reg_id_t read2,
    input bit enable_write,
    input reg_id_t write_id,
    input op_t write_data,
    input bit clk,
    output op_t read1_out,
    output op_t read2_out
);
    op_reg_t regs [31:0];
    op_t next_read1_out, next_read2_out;

    initial begin
        regs[28] = GPAt;
        regs[29] = SPAt;
    end

    assign next_read1_out = regs[read1];
    assign next_read2_out = regs[read2];

    always_ff @(posedge clk) begin
        read1_out <= next_read1_out;
        read2_out <= next_read2_out;
    end

    always_ff @(negedge clk)
        if (enable_write == 1)
            regs[write_id] <= write_data;
endmodule : Registers