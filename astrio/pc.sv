import Parameters::*;
import PCType::pc_cmd_t;
import Types::addr_t;

module PC(
    input PCType::pc_cmd_t cmd,
    input addr_t load_pc,
    input bit rst,
    input bit clk,
    output addr_t pc,
    output addr_t inc_pc
);
    initial pc = InstStartFrom;

    addr_t current_pc;

    assign inc_pc = pc+4;
    always_comb begin
        unique case (cmd)
            PCType::HOLD:
                current_pc = pc;
            PCType::INC:
                current_pc = inc_pc;
            PCType::INC_OFFSET:
                current_pc = inc_pc+load_pc;
            PCType::LOAD:
                current_pc = load_pc;
            default:
                current_pc = pc;
        endcase

        if (rst == 1)
            current_pc = InstStartFrom;
    end

    always_ff @(posedge clk)
        pc <= current_pc;
endmodule : PC