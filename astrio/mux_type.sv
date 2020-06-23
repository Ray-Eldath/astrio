package Mux3Type;
    typedef enum bit [1:0]{
        DEFAULT, TOP, BOTTOM, ZERO
    } cmd_t;
endpackage : Mux3Type

package Mux2Type;
    typedef enum bit [1:0]{
        THIS, THAT, ZERO
    } cmd_t;
endpackage : Mux2Type