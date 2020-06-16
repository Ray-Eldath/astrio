package ALUType;
    typedef enum bit [3:0]{
        NONE, AND, OR, ADD, SUB, LESS_THAN, EQUAL, XOR, SLL, SRL
    } alu_cmd_t /* verilator public */;
endpackage : ALUType