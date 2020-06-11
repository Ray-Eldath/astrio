package CPUType;
    typedef enum bit [1:0]{
        R, I, J
    } inst_type;

    typedef logic unsigned [31:26] opcode_t;
    typedef logic unsigned [5:0] funct_t;
endpackage : CPUType