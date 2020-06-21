import Mux3Type::cmd_t;
import Types::op_t;

module OpMux3(
    input op_t default_line,
    input op_t top_line,
    input op_t bottom_line,
    input cmd_t cmd,
    output op_t out
);

    always_comb
        unique case (cmd)
            Mux3Type::ZERO:
                out = 0;
            Mux3Type::DEFAULT:
                out = default_line;
            Mux3Type::LEFT:
                out = top_line;
            Mux3Type::RIGHT:
                out = bottom_line;
            default:
                out = default_line;
        endcase
endmodule : OpMux3