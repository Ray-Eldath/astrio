import CPUType::*;
import ALUType::*;

module ALUController(
    input opcode_t opcode,
    input funct_t funct,
    output ALUType::alu_cmd_t alu_cmd_out
);

    always_comb
        unique case (opcode)
            6'b0: begin
                unique case (funct)
                    6'b10_0000:  // add
                        alu_cmd_out = ALUType::ADD;
                    default:
                        alu_cmd_out = ALUType::NONE;
                endcase
            end
            6'b00_1000: begin   // I: addi
                alu_cmd_out = ALUType::ADD;
            end
            default:
                alu_cmd_out = ALUType::NONE;
        endcase

endmodule : ALUController