import Types::*;
import Parameters::*;

module Memory(
    input addr_t addr,
    input bit enable_write,
    input op_t write_data,
    input bit clk,
    output op_t read_out
);
    op_t mem [7:0];

    assign read_out = mem[addr >> 2];

    always_ff @(posedge clk)
        if (enable_write)
            mem[addr >> 2] <= write_data;

endmodule: Memory