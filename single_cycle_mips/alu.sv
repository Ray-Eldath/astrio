import Types::*;
import ALUType::cmd_t;

module ALU(
    input ALUType::cmd_t cmd,
    input reg_t a,
    input reg_t b,
    input bit clk,
    output reg_t out,
    output bit overflow,
    output bit zero
);
    logic signed [32:0] out_e;
    reg_t next_out;

    assign zero = out == 0;

    always_comb begin
        overflow = 0;

        unique case (cmd)
            ALUType::AND:
                next_out = a & b;
            ALUType::OR:
                next_out = a | b;
            ALUType::ADD: begin
                out_e = {a[31], a}+{b[31], b};
                next_out = out_e[31:0];
                overflow = out_e[32] ^ out_e[31];
            end
            ALUType::SUB: begin
                out_e = {a[31], a}-{b[31], b};
                next_out = out_e[31:0];
                overflow = out_e[32] ^ out_e[31];
            end
            ALUType::LESS_THAN:
                next_out = {{31{1'b0}}, a < b};
            ALUType::XOR:
                next_out = ~(a | b);
            default:
                next_out = 0;
        endcase
    end

    always_ff @(posedge clk)
        out <= next_out;

endmodule: ALU