import Types::*;
import Parameters::*;

module Memory(
    input addr_t addr,
    input bit enable_write,
    input op_t write_data,
    input bit clk,
    output op_t read_out
);
    op_t static_mem [MemStaticStartFrom+MemStaticSpace:MemStaticStartFrom];
    op_t dynamic_mem [MemDynamicStartFrom+MemDynamicSpace:MemDynamicStartFrom];

    assign read_out = addr >= MemDynamicStartFrom ? dynamic_mem[addr]:static_mem[addr];

    always_ff @(posedge clk)
        if (enable_write)
            if (addr >= MemDynamicStartFrom)
                dynamic_mem[addr] <= write_data;
            else
                static_mem[addr] <= write_data;

endmodule: Memory