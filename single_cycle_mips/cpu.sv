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
    addr_t pc, load_pc, inc_pc;
    PCType::pc_cmd_t pc_cmd;

    PC pc_m(.cmd(pc_cmd), .load_pc(load_pc), .rst(rst), .clk(clk), .inc_pc(inc_pc), .pc(pc));
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

    logic unsigned [2:0] opcode_lead3;
    logic unsigned [5:0] shamt;

    assign opcode = inst[31:26];
    assign funct = inst[5:0];
    assign opcode_lead3 = opcode[31:29];

    always_comb begin
        alu_a = 0;
        alu_b = 0;
        read1 = 0;
        read2 = 0;
        reg_write = 0;
        reg_write_id = 0;
        reg_write_data = 0;
        pc_cmd = PCType::INC;
        load_pc = 0;

        unique case (opcode_lead3)
            3'b001: begin // I: i
                read1 = inst[25:21]; // rs

                unique case (funct)
                    6'b00_1100, 6'b00_1101: begin // andi, ori
                        alu_a = read1_out;
                        alu_b = {{16{1'b0}}, inst[15:0]};
                    end
                    default: begin // addi, andi, ori, slti
                        alu_a = read1_out;
                        alu_b = {{16{inst[15]}}, inst[15:0]};
                    end
                endcase

                reg_write = 1;
                reg_write_id = inst[20:16]; // rt
                reg_write_data = alu_out;
            end
            3'b000: begin
                unique case (opcode)
                    6'b00_0010, 6'b00_0011: begin // j, jal
                        pc_cmd = PCType::LOAD;
                        alu_a = pc;
                        alu_b = 2;
                        load_pc = {alu_out[31:28], inst[25:0], 2'b0};

                        if (opcode == 6'b00_0011) begin // jal
                            reg_write = 1;
                            reg_write_id = 5'd31; // $ra
                            reg_write_data = inc_pc;
                        end
                    end
                    6'b00_0100, 6'b00_0101: begin // beq, bne
                        read1 = inst[25:21]; // rs
                        read2 = inst[20:16]; // rt
                        alu_a = read1_out;
                        alu_b = read2_out;

                        if (opcode == 6'b00_0100) begin // beq
                            if (alu_out == 1) begin
                                pc_cmd = PCType::LOAD;
                                load_pc = inc_pc+{{14{inst[15]}}, inst[15:0], 2'b0};
                            end
                        end
                        else if (opcode == 6'b00_0101) begin // bne
                            if (alu_out == 0) begin
                                pc_cmd = PCType::LOAD;
                                load_pc = inc_pc+{{14{inst[15]}}, inst[15:0], 2'b0};
                            end
                        end
                    end
                    6'b0: begin // R
                        read1 = inst[25:21]; // rs
                        read2 = inst[20:16]; // rt

                        unique case (funct)
                            6'b0, 6'b00_0010: begin // sll, srl
                                alu_a = read2_out;
                                alu_b = {{27{1'b0}}, inst[10:6]};
                            end
                            6'b00_1000: begin // jr
                                pc_cmd = PCType::LOAD;
                                load_pc = read1_out; // rs
                            end
                            default: begin // add, sub, and, or, slt
                                alu_a = read1_out;
                                alu_b = read2_out;
                            end
                        endcase

                        if (alu_cmd != ALUType::NONE) begin
                            reg_write = 1;
                            reg_write_id = inst[15:11]; // rd
                            reg_write_data = alu_out;
                        end
                    end
                    default: begin

                    end
                endcase
            end
            default: begin

            end
        endcase
    end

endmodule : ExpMipsCPU