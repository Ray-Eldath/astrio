import Types::*;
import CPUType::*;
import ALUType::*;
import PCType::*;

module ExpMipsCPU(
    input bit clk,
    input bit rst,
    input bit chip_select
);


    // IF/ID
    inst_t inst;
    addr_t inc_pc;

    // PC
    PCType::pc_cmd_t pc_cmd;
    addr_t pc, load_pc;
    inst_t load_inst;
    bit loading_inst;

    // instrucion decoding
    funct_t funct, funct_s2;
    opcode_t opcode, opcode_s2, opcode_s3, opcode_s4;

    // ALU
    ALUType::alu_cmd_t alu_cmd;
    op_t alu_a, alu_a__, alu_a_s2;
    op_t alu_b, alu_b__, alu_b_s2;
    op_t alu_out, alu_out__;
    bit alu_overflow;

    // reg
    bit reg_write_enable, reg_write_enable__, reg_write_enable_s2, reg_write_enable_s3, reg_write_enable_s4;
    op_t reg_write_data, reg_write_data__, reg_write_data_s3, reg_write_data_s4;
    reg_id_t reg_write_id, reg_write_id__, reg_write_id_s2, reg_write_id_s3, reg_write_id_s4;

    op_t read1_out, read2_out;
    reg_id_t read1, read2;

    // mem
    addr_t mem_addr, mem_addr__, mem_addr_s3;
    bit mem_write_enable, mem_write_enable__, mem_write_enable_s2, mem_write_enable_s3;
    op_t mem_write_data, mem_write_data__, mem_write_data_s2, mem_write_data_s3;
    op_t mem_read_out;


    PC pc_m(.cmd(pc_cmd), .load_pc(load_pc), .rst(rst), .inc_pc(inc_pc), .pc(pc), .clk(clk));

    Fetcher fetcher_m(.addr(pc), .load_inst(load_inst), .load(loading_inst), .chip_select(chip_select), .clk(clk), .inst(inst));

    Registers registers_m(
        .read1(read1), .read2(read2),
        .enable_write(reg_write_enable__), .write_id(reg_write_id__), .write_data(reg_write_data__),
        .read1_out(read1_out), .read2_out(read2_out),
        .clk(clk));

    ALUController alu_controller_m(.opcode(opcode_s2), .funct(funct_s2), .alu_cmd_out(alu_cmd));

    ALU alu_m(.cmd(alu_cmd), .a(alu_a__), .b(alu_b__), .out(alu_out), .overflow(alu_overflow));
    Memory mem_m(.addr(mem_addr__), .enable_write(mem_write_enable__), .write_data(mem_write_data__), .read_out(mem_read_out), .clk(clk));


    logic unsigned [5:0] shamt;

    always_comb begin
        opcode = inst[31:26];
        funct = inst[5:0];
    end

    // s1: ID
    always_comb begin
        alu_a = 0;
        alu_b = 0;
        read1 = 0;
        read2 = 0;
        reg_write_enable = 0;
        reg_write_id = 0;
        reg_write_data = 0;
        mem_write_enable = 0;
        mem_write_data = 0;
        pc_cmd = PCType::INC;
        load_pc = 0;

        unique casez (opcode)
            6'b001_???: begin // I: i
                read1 = inst[25:21]; // rs

                unique case (opcode)
                    6'b00_1100, 6'b00_1101: begin // andi, ori
                        alu_a = read1_out;
                        alu_b = {{16{1'b0}}, inst[15:0]};
                    end
                    default: begin // addi, slti
                        alu_a = read1_out;
                        alu_b = {{16{inst[15]}}, inst[15:0]};
                    end
                endcase

                reg_write_enable = 1;
                reg_write_id = inst[20:16]; // rt
            end
            6'b000_???: begin
                unique case (opcode)
                    // 6'b00_0010, 6'b00_0011: begin // j, jal
                    //     pc_cmd = PCType::LOAD;
                    //     alu_a = pc;
                    //     alu_b = 2;
                    //     load_pc = {alu_out[31:28], inst[25:0], 2'b0};
                    //
                    //     if (opcode == 6'b00_0011) begin // jal
                    //         reg_write_enable = 1;
                    //         reg_write_id = 5'd31; // $ra
                    //         reg_write_data = inc_pc;
                    //     end
                    // end
                    // 6'b00_0100, 6'b00_0101: begin // beq, bne
                    //     read1 = inst[25:21]; // rs
                    //     read2 = inst[20:16]; // rt
                    //     alu_a = read1_out;
                    //     alu_b = read2_out;
                    //
                    //     if (opcode == 6'b00_0100 && alu_out == 1) begin // beq
                    //         pc_cmd = PCType::INC_OFFSET;
                    //         load_pc = {{14{inst[15]}}, inst[15:0], 2'b0};
                    //     end else if (opcode == 6'b00_0101 && alu_out == 0) begin // bne
                    //         pc_cmd = PCType::INC_OFFSET;
                    //         load_pc = {{14{inst[15]}}, inst[15:0], 2'b0};
                    //     end
                    // end
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
                                // TODO: clear
                            end
                            default: begin // add, sub, and, or, slt
                                alu_a = read1_out;
                                alu_b = read2_out;
                            end
                        endcase

                        if (alu_cmd != ALUType::NONE) begin
                            reg_write_enable = 1;
                            reg_write_id = inst[15:11]; // rd
                        end
                    end
                    default: begin end
                endcase
            end
            6'b10?_???: begin // lX, sX
                read1 = inst[25:21]; // rs
                alu_a = read1_out;
                alu_b = {{16{inst[15]}}, inst[15:0]};

                unique case (opcode)
                    6'b10_0011: begin // lw
                        reg_write_enable = 1;
                        reg_write_id = inst[20:16]; // rt
                    end
                    6'b10_1011: begin // sw
                        read2 = inst[20:16]; // rt
                        mem_write_enable = 1;
                        mem_write_data = read2_out;
                    end
                    default: begin end
                endcase
            end
            default: begin end
        endcase
    end

    always_ff @(posedge clk) begin
        // s1 -> s2
        opcode_s2 <= opcode;
        funct_s2 <= funct;

        alu_a_s2 <= alu_a;
        alu_b_s2 <= alu_b;
        reg_write_enable_s2 <= reg_write_enable;
        reg_write_id_s2 <= reg_write_id;
        mem_write_enable_s2 <= mem_write_enable;
        mem_write_data_s2 <= mem_write_data;
    end

    // s2: EX
    always_comb begin
        mem_addr = 0;
        reg_write_data = 0;

        alu_a__ = alu_a_s2;
        alu_b__ = alu_b_s2;

        unique casez (opcode_s2)
            6'b001_???: // I: i
                reg_write_data = alu_out;
            6'b000_???:
                if (opcode_s2 == 6'b0) // R
                    reg_write_data = alu_out;
            6'b10?_???: // lX, sX
                mem_addr = alu_out;
            default: begin end
        endcase
    end

    always_ff @(posedge clk) begin
        reg_write_data_s3 <= reg_write_data;
        mem_addr_s3 <= mem_addr;

        // s2 -> s3
        opcode_s3 <= opcode_s2;

        reg_write_enable_s3 <= reg_write_enable_s2;
        reg_write_id_s3 <= reg_write_id_s2;

        mem_write_enable_s3 <= mem_write_enable_s2;
        mem_write_data_s3 <= mem_write_data_s2;
    end

    // s3: MEM
    always_comb begin
        mem_addr__ = mem_addr_s3;
        mem_write_enable__ = mem_write_enable_s3;
        mem_write_data__ = mem_write_data_s3;
    end

    always_ff @(posedge clk) begin
        reg_write_data_s4 <= opcode_s3[31:29] == 3'b100 ? mem_read_out:reg_write_data_s3;

        // s3 -> s4
        opcode_s4 <= opcode_s3;
        reg_write_enable_s4 <= reg_write_enable_s3;
        reg_write_id_s4 <= reg_write_id_s3;
    end

    // s4: WB
    always_ff @(posedge clk) begin
        reg_write_enable__ <= reg_write_enable_s4;
        reg_write_id__ <= reg_write_id_s4;
        reg_write_data__ <= reg_write_data_s4;
    end
endmodule : ExpMipsCPU