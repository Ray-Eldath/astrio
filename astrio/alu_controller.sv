import CPUType::*;
import ALUType::*;

module ALUController(
    input opcode_t opcode,
    input funct_t funct,
    output ALUType::alu_cmd_t alu_cmd_out
);

    always_comb
        unique case (opcode)
            6'b0: begin // R
                unique case (funct)
                    6'b10_0000: // add
                        alu_cmd_out = ALUType::ADD;
                    6'b10_0010: // sub
                        alu_cmd_out = ALUType::SUB;
                    6'b10_0100: // and
                        alu_cmd_out = ALUType::AND;
                    6'b10_0101: // or
                        alu_cmd_out = ALUType::OR;
                    6'b0: // sll
                        alu_cmd_out = ALUType::SLL;
                    6'b00_0010:  // srl
                        alu_cmd_out = ALUType::SRL;
                    6'b10_1010: // slt
                        alu_cmd_out = ALUType::LESS_THAN;
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
            6'b00_1010:   // I: slti
                alu_cmd_out = ALUType::LESS_THAN;
            6'b00_0100, 6'b00_0101:  // I: beq, bne
                alu_cmd_out = ALUType::EQUAL;
            6'b10_0011, 6'b10_1011:  // I: lw, sw
                alu_cmd_out = ALUType::ADD;
            default:
                alu_cmd_out = ALUType::NONE;
        endcase

endmodule : ALUController