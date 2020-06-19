import Types::*;
import ALUType::alu_cmd_t;

module ALU(
    input ALUType::alu_cmd_t cmd,
    input op_t a,
    input op_t b,
    output op_t out,
    output bit overflow
);
    logic signed [32:0] out_e;

    always_comb begin
        overflow = 0;

        unique case (cmd)
            ALUType::AND:
                out = a & b;
            ALUType::OR:
                out = a | b;
            ALUType::ADD: begin
                out_e = {a[31], a}+{b[31], b};
                out = out_e[31:0];
                overflow = out_e[32] ^ out_e[31];
            end
            ALUType::SUB: begin
                out_e = {a[31], a}-{b[31], b};
                out = out_e[31:0];
                overflow = out_e[32] ^ out_e[31];
            end
            ALUType::LESS_THAN: // a < b -> 1
                out = {{31{1'b0}}, a < b};
            ALUType::EQUAL: // a == b -> 1
                out = {{31{1'b0}}, a == b};
            ALUType::XOR:
                out = ~(a | b);
            ALUType::SLL:
                out = a << b;
            ALUType::SRL:
                out = a >> b;
            default:
                out = 0;
        endcase
    end

endmodule: ALU