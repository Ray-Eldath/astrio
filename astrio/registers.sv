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
    op_t regs [31:0];

    initial begin
        regs[28] = GPAt;
        regs[29] = SPAt;
    end

    always_ff @(posedge clk)
        if (enable_write == 1 && write_id != 0)
            regs[write_id] <= write_data;

    assign read1_out = (enable_write && write_id == read1 && write_id != 0) ? write_data:regs[read1];
    assign read2_out = (enable_write && write_id == read2 && write_id != 0) ? write_data:regs[read2];
endmodule : Registers