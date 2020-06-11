import Types::*;
import Parameters::*;

module Memory(
    input addr_t addr,
    input bit enable_write,
    input op_t write_data,
    input bit clk,
    output op_t read_out
);
    op_reg_t mem [MemStartFrom+MemSpace:MemStartFrom];

    always_ff @(posedge clk) begin
        if (enable_write)
            mem[addr] <= write_data;
        else
            read_out <= mem[addr];
    end
endmodule : Memory