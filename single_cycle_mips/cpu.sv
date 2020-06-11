import Types::*;
import CPUType::*;
import ALUType::*;
import PCType::*;

module ExpMipsCPU(
    input bit clk,
    input bit rst,
    input bit chip_select
);

    inst_t inst;
    opcode_t opcode;
    funct_t funct;

    inst_t load_inst;
    bit loading_inst;
    addr_t pc, pc_load_pc;
    PCType::pc_cmd_t pc_cmd;

    PC pc_m(.cmd(pc_cmd), .load_pc(pc_load_pc), .rst(rst), .clk(clk), .pc(pc));
    Fetcher fetcher_m(.addr(pc), .load_inst(load_inst), .load(loading_inst), .chip_select(chip_select), .clk(clk), .inst(inst));

    reg_id_t read1, read2, reg_write_id;
    bit reg_write;
    op_t reg_write_data, read1_out, read2_out;

    Registers registers_m(
        .read1(read1), .read2(read2),
        .enable_write(reg_write), .write_id(reg_write_id), .write_data(reg_write_data),
        .read1_out(read1_out), .read2_out(read2_out),
        .clk(clk));

    ALUType::alu_cmd_t alu_cmd;
    ALUController alu_controller_m(.opcode(opcode), .funct(funct), .alu_cmd_out(alu_cmd));

    op_t alu_a, alu_b, alu_out;
    bit alu_overflow, alu_zero;
    ALU alu_m(.cmd(alu_cmd), .a(alu_a), .b(alu_b), .out(alu_out), .overflow(alu_overflow), .zero(alu_zero));

    addr_t mem_addr;
    bit mem_write;
    op_t mem_write_data, mem_read_out;
    Memory mem_m(.addr(mem_addr), .enable_write(mem_write), .write_data(mem_write_data), .clk(clk), .read_out(mem_read_out));

    assign opcode = inst[31:26];
    assign funct = inst[5:0];

    always_ff @(posedge clk) begin
        pc_cmd <= PCType::INC;
    end

    always_comb begin
        alu_a = 0;
        alu_b = 0;
        read1 = 0;
        read2 = 0;
        reg_write = 0;
        reg_write_id = 0;
        reg_write_data = 0;

        unique case (opcode)
            6'b0: begin // R
                read1 = inst[25:21]; // rs
                read2 = inst[20:16]; // rt

                alu_a = read1_out;
                alu_b = read2_out;

                reg_write = 1;
                reg_write_id = inst[15:11]; // rd
                reg_write_data = alu_out;
            end

            6'b001000: begin // I: addi
                read1 = inst[25:21]; // rs

                alu_a = {{16{inst[15]}}, inst[15:0]};
                alu_b = read1_out;

                reg_write = 1;
                reg_write_id = inst[20:16]; // rt
                reg_write_data = alu_out;
            end

            default: begin

            end
        endcase
    end

endmodule : ExpMipsCPU