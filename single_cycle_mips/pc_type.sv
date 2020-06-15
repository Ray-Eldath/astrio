package PCType;
    typedef enum bit [1:0]{
        NONE, INC, INC_OFFSET, LOAD
    } pc_cmd_t /* verilator public */;
endpackage : PCType