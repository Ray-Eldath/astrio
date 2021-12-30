import Types::*;
import CPUType::*;


module Guardian(
    input bit clk,
    input bit rst_PB_in,
    output logic CA, CB, CC, CD, CE, CF, CG, DP, AN0, AN1, AN2, AN3, AN4, AN5, AN6, AN7
);
    bit clk_out1, rst, chip_select, rst_PB_state;
    PCType::pc_cmd_t pc_cmd;
    reg_id_t read1, read2;
    op_t read1_out, read2_out;

    clk_wiz_0 clk_0 (
        .clk_out1(clk_out1),     // output clk_out1
        .clk_in1(clk)        // input clk_in1
    );
    
//    Debouncer rst_db (.clk(clk), .PB(rst_PB_in), .PB_state(rst_PB_state));

    (* dont_touch = "true" *) Astrio cpu(
        .clk(clk),
        .rst(rst_PB_in),
        .chip_select(!rst_PB_in),
        .read1(read1),
        .read2(read2),
        .read1_out(read1_out),
        .read2_out(read2_out),
        .CA(CA), .CB(CB), .CC(CC), .CD(CD), .CE(CE), .CF(CF), .CG(CG), .DP(DP), .AN0(AN0), .AN1(AN1), .AN2(AN2), .AN3(AN3), .AN4(AN4), .AN5(AN5), .AN6(AN6), .AN7(AN7)
    );
endmodule
