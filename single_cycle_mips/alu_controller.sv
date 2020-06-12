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
                    6'b10_0000: // add
                        alu_cmd_out = ALUType::ADD;
                    6'b10_0010: // sub
                        alu_cmd_out = ALUType::SUB;
                    6'b10_0100: // and
                        alu_cmd_out = ALUType::AND;
                    6'b10_0101: // or
                        alu_cmd_out = ALUType::OR;
                    default:
                        alu_cmd_out = ALUType::NONE;
                endcase
            end
            6'b00_1000:   // I: addi
                alu_cmd_out = ALUType::ADD;
            6'b00_1100:   // I: andi
                alu_cmd_out = ALUType::AND;
            6'b00_1101:   // I: ori
                alu_cmd_out = ALUType::OR;
            default:
                alu_cmd_out = ALUType::NONE;
        endcase

endmodule : ALUController