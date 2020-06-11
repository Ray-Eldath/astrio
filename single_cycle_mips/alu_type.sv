package ALUType;
    typedef enum bit [2:0]{
        NONE, AND, OR, ADD, SUB, LESS_THAN, XOR
    } alu_cmd_t /* verilator public */;
endpackage : ALUType