import Parameters::*;
import PCType::cmd_t;
import Types::addr_t;

module PC(
    input PCType::cmd_t cmd,
    input addr_t load_pc,
    input bit rst,
    input bit clk,
    output addr_t pc
);
    initial
        pc = InstStartFrom;

    addr_t next_pc;

    always_comb begin
        unique case (cmd)
            PCType::NONE:
                next_pc = pc;
            PCType::INC:
                next_pc = pc+4;
            PCType::LOAD:
                next_pc = load_pc;
            default:
                next_pc = pc;
        endcase

        if (rst == 1)
            next_pc = InstStartFrom;
    end

    always_ff @(posedge clk)
        pc <= next_pc;
endmodule : PC