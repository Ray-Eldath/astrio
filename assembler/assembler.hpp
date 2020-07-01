//
// Created by Ray Eldath on 2020/6/11 0011.
//

#ifndef ASTRIO_ASSEMBLER_HPP
#define ASTRIO_ASSEMBLER_HPP

#include <bitset>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <iterator>

enum class Register {
    $zero = 0,
    $at = 1,
    $v0 = 2, $v1 = 3,
    $a0 = 4, $a1 = 5, $a2 = 6, $a3 = 7,
    $t0 = 8, $t1 = 9, $t2 = 10, $t3 = 11, $t4 = 12, $t5 = 13, $t6 = 14, $t7 = 15,
    $s0 = 16, $s1 = 17, $s2 = 18, $s3 = 19, $s4 = 20, $s5 = 21, $s6 = 22, $s7 = 23,
    $t8 = 24, $t9 = 25,
    $k0 = 26, $k1 = 27,
    $gp = 28,
    $sp = 29,
    $fp = 30,
    $ra = 31
};

static std::string nameOf(Register reg) {
    const int id = static_cast<int>(reg);

    std::stringstream ss;

    if (id == 0)
        return "$zero";
    else if (id == 1)
        return "$at";
    else if (id == 2 || id == 3)
        ss << "$v" << id - 2;
    else if (id >= 4 && id <= 7)
        ss << "$a" << id - 4;
    else if (id >= 8 && id <= 15)
        ss << "$t" << id - 8;
    else if (id >= 16 && id <= 23)
        ss << "$s" << id - 16;
    else if (id == 24 || id == 25)
        ss << "$t" << id - 16;
    else if (id == 26 || id == 27)
        ss << "$v" << id - 26;
    else if (id == 28)
        return "$gp";
    else if (id == 29)
        return "$sp";
    else if (id == 30)
        return "$fp";
    else if (id == 31)
        return "$ra";

    return ss.str();
}

enum class ResolvableInstruction {
    J, JAL, BEQ, BNE
};

struct Instruction {
    uint32_t addr;
    uint32_t compiled;
};

struct PendingInstruction {
    ResolvableInstruction type;
    std::string pending_identifier;
    uint32_t addr;
    Register rs = Register::$zero, rt = Register::$zero;
};

class AstrioAssembler {
private:
    uint32_t pc;
    const uint32_t pc_start, pc_inc;
    std::map<std::string, uint32_t> identifier_addr;
    std::vector<Instruction> insts;
    std::vector<PendingInstruction> pending_insts;

    inline uint32_t compile_j(uint32_t addr) { return J(0x2, addr >> 2); }

    inline uint32_t compile_jal(uint32_t addr) { return J(0x3, (addr + pc_inc) >> 2); }

    inline uint32_t compile_beq(Register rs, Register rt, int32_t addr, int32_t that_pc) {
        return I(0x4, rs, rt, (addr - pc_inc - that_pc) >> 2);
    }

    inline uint32_t compile_bne(Register rs, Register rt, int32_t addr, int32_t that_pc) {
        printf("\n%#x\n", addr);
        return I(0x5, rs, rt, (addr - pc_inc - that_pc) >> 2);
    }

    template<ResolvableInstruction inst_type>
    inline void resolve_j(const PendingInstruction &inst) {
        uint32_t compiled;
        uint32_t addr = identifier_addr[inst.pending_identifier];

        if constexpr(inst_type == ResolvableInstruction::J)
            compiled = compile_j(addr);
        else if constexpr(inst_type == ResolvableInstruction::JAL)
            compiled = compile_jal(addr);
        else if constexpr(inst_type == ResolvableInstruction::BEQ)
            compiled = compile_beq(inst.rs, inst.rt, addr, inst.addr);
        else if constexpr(inst_type == ResolvableInstruction::BNE)
            compiled = compile_bne(inst.rs, inst.rt, addr, inst.addr);

        insts.insert(insts.begin() + (inst.addr - pc_start) / pc_inc - 1, Instruction{inst.addr, compiled});
    }

    // region private
    inline void incPC(uint32_t inst_count = 1) { pc += inst_count * pc_inc; }

    template<size_t size, typename T>
    inline std::string toBinary(const T &t) {
        return std::bitset<size>(t).to_string();
    }

    uint32_t R(uint8_t opcode, Register rs, Register rt, Register rd, uint8_t shamt, uint8_t funct) {
        std::stringstream binary;
        binary << toBinary<6>(opcode);
        binary << toBinary<5>((uint32_t) rs);
        binary << toBinary<5>((uint32_t) rt);
        binary << toBinary<5>((uint32_t) rd);
        binary << toBinary<5>(shamt);
        binary << toBinary<6>(funct);

        return std::stol(binary.str(), nullptr, 2);
    }

    uint32_t I(uint8_t opcode, Register rs, Register rt, int32_t immi) {
        std::stringstream binary;

        binary << toBinary<6>(opcode);
        binary << toBinary<5>((uint32_t) rs);
        binary << toBinary<5>((uint32_t) rt);
        binary << toBinary<16>(immi);

        return std::stol(binary.str(), nullptr, 2);
    }

    uint32_t J(uint8_t opcode, uint32_t addr) {
        std::stringstream binary;

        binary << toBinary<6>(opcode);
        binary << toBinary<26>(addr);

        return std::stol(binary.str(), nullptr, 2);
    }
    // endregion

public:
    explicit AstrioAssembler(uint32_t inst_start, const uint32_t inst_inc = 0x4) :
            pc(inst_start), pc_start(inst_start), pc_inc(inst_inc) {}

    std::vector<Instruction> assemble() {
        for (const auto &pending_inst : pending_insts) {
            std::string s = pending_inst.pending_identifier;
            if (identifier_addr.find(s) == identifier_addr.end()) {
                fprintf(stderr, "unexpected EOL but %s still undefined.", s.c_str());
                abort();
            }

            if (pending_inst.type == ResolvableInstruction::J)
                resolve_j<ResolvableInstruction::J>(pending_inst);
            else if (pending_inst.type == ResolvableInstruction::JAL)
                resolve_j<ResolvableInstruction::JAL>(pending_inst);
            else if (pending_inst.type == ResolvableInstruction::BEQ)
                resolve_j<ResolvableInstruction::BEQ>(pending_inst);
            else if (pending_inst.type == ResolvableInstruction::BNE)
                resolve_j<ResolvableInstruction::BNE>(pending_inst);
        }

        return insts;
    }

    AstrioAssembler *claim(const std::string &identifier) {
        identifier_addr.insert(std::pair(identifier, pc));
        return this;
    }

    AstrioAssembler *add(Register rd, Register rs, Register rt) {
        incPC();
        insts.push_back(Instruction{pc, R(0, rs, rt, rd, 0, 0x20)});
        return this;
    }

    AstrioAssembler *addi(Register rt, Register rs, uint16_t immi) {
        incPC();
        insts.push_back(Instruction{pc, I(0x8, rs, rt, immi)});
        return this;
    }

    AstrioAssembler *sub(Register rd, Register rs, Register rt) {
        incPC();
        insts.push_back(Instruction{pc, R(0, rs, rt, rd, 0, 0x22)});
        return this;
    }

    AstrioAssembler *and_(Register rd, Register rs, Register rt) {
        incPC();
        insts.push_back(Instruction{pc, R(0, rs, rt, rd, 0, 0x24)});
        return this;
    }

    AstrioAssembler *andi(Register rt, Register rs, uint16_t immi) {
        incPC();
        insts.push_back(Instruction{pc, I(0xc, rs, rt, immi)});
        return this;
    }

    AstrioAssembler *or_(Register rd, Register rs, Register rt) {
        incPC();
        insts.push_back(Instruction{pc, R(0, rs, rt, rd, 0, 0x25)});
        return this;
    }

    AstrioAssembler *ori(Register rt, Register rs, uint16_t immi) {
        incPC();
        insts.push_back(Instruction{pc, I(0xd, rs, rt, immi)});
        return this;
    }

    AstrioAssembler *move(Register rt, Register rs) { return ori(rt, rs, 0); }

    AstrioAssembler *li(Register rt, uint16_t immi) { return ori(rt, Register::$zero, immi); }

    AstrioAssembler *slt(Register rd, Register rs, Register rt) {
        incPC();
        insts.push_back(Instruction{pc, R(0, rs, rt, rd, 0, 0x2a)});
        return this;
    }

    AstrioAssembler *slti(Register rt, Register rs, uint16_t immi) {
        incPC();
        insts.push_back(Instruction{pc, I(0xa, rs, rt, immi)});
        return this;
    }

    AstrioAssembler *jr(Register rs) {
        incPC();
        insts.push_back(Instruction{pc, R(0, rs, Register::$zero, Register::$zero, 0, 0x8)});
        return this;
    }

    AstrioAssembler *nop() {
        incPC();
        insts.push_back(Instruction{pc, 0});
        return this;
    }

    AstrioAssembler *sll(Register rd, Register rt, uint16_t shamt) {
        incPC();
        insts.push_back(Instruction{pc, R(0x0, Register::$zero, rt, rd, shamt, 0x0)});
        return this;
    }

    AstrioAssembler *srl(Register rd, Register rt, uint16_t shamt) {
        incPC();
        insts.push_back(Instruction{pc, R(0x0, Register::$zero, rt, rd, shamt, 0x2)});
        return this;
    }

    AstrioAssembler *j(const std::string &identifier) {
        if (identifier_addr.find(identifier) == identifier_addr.end()) {
            incPC();
            pending_insts.push_back(PendingInstruction{ResolvableInstruction::J, identifier, pc});
            return this;
        } else
            return j(identifier_addr[identifier]);
    }

    AstrioAssembler *j(uint32_t addr) {
        incPC();
        insts.push_back(Instruction{pc, compile_j(addr)});
        return this;
    }

    AstrioAssembler *jal(const std::string &identifier) {
        if (identifier_addr.find(identifier) == identifier_addr.end()) {
            incPC();
            pending_insts.push_back(PendingInstruction{ResolvableInstruction::JAL, identifier, pc});
            return this;
        } else
            return jal(identifier_addr[identifier]);
    }

    AstrioAssembler *jal(uint32_t addr) {
        incPC();
        insts.push_back(Instruction{pc, compile_jal(addr)});
        return this;
    }

    AstrioAssembler *beq(Register rs, Register rt, const std::string &identifier) {
        if (identifier_addr.find(identifier) == identifier_addr.end()) {
            incPC();
            pending_insts.push_back(PendingInstruction{ResolvableInstruction::BEQ, identifier, pc, rs, rt});
            return this;
        } else
            return beq(rs, rt, identifier_addr[identifier]);
    }

    AstrioAssembler *beq(Register rs, Register rt, uint32_t addr) {
        incPC();
        insts.push_back(Instruction{pc, compile_beq(rs, rt, addr, pc)});
        return this;
    }

    AstrioAssembler *bne(Register rs, Register rt, const std::string &identifier) {
        if (identifier_addr.find(identifier) == identifier_addr.end()) {
            incPC();
            pending_insts.push_back(PendingInstruction{ResolvableInstruction::BNE, identifier, pc, rs, rt});
            return this;
        } else
            return bne(rs, rt, identifier_addr[identifier]);
    }

    AstrioAssembler *bne(Register rs, Register rt, uint32_t addr) {
        incPC();
        insts.push_back(Instruction{pc, compile_bne(rs, rt, addr, pc)});
        return this;
    }

    AstrioAssembler *lw(Register rt, Register rs, uint16_t offset) {
        incPC();
        insts.push_back(Instruction{pc, I(0x23, rs, rt, offset)});
        return this;
    }

    AstrioAssembler *sw(Register rt, Register rs, uint16_t offset) {
        incPC();
        insts.push_back(Instruction{pc, I(0x2b, rs, rt, offset)});
        return this;
    }
};

#endif //ASTRIO_ASSEMBLER_HPP
