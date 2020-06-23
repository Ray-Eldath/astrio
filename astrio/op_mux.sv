import Mux3Type::cmd_t;
import Mux2Type::cmd_t;
import Types::op_t;

module OpMux3(
    input op_t default_line,
    input op_t top_line,
    input op_t bottom_line,
    input Mux3Type::cmd_t cmd,
    output op_t out
);

    always_comb
        unique case (cmd)
            Mux3Type::ZERO:
                out = 0;
            Mux3Type::DEFAULT:
                out = default_line;
            Mux3Type::TOP:
                out = top_line;
            Mux3Type::BOTTOM:
                out = bottom_line;
            default:
                out = default_line;
        endcase
endmodule : OpMux3

module OpMux2(
    input op_t this_line,
    input op_t that_line,
    input Mux2Type::cmd_t cmd,
    output op_t out
);

    always_comb
        unique case (cmd)
            Mux2Type::ZERO:
                out = 0;
            Mux2Type::THIS:
                out = this_line;
            Mux2Type::THAT:
                out = that_line;
            default:
                out = this_line;
        endcase
endmodule : OpMux2