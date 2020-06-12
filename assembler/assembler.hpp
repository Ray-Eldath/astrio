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

enum class ResolvableInstruction {
    J, JAL
};

struct Instruction {
    std::string asm_code;
    uint32_t addr;
    uint32_t compiled;
};

struct PendingInstruction {
    ResolvableInstruction type;
    std::string asm_code, pending_identifier;
    uint32_t addr;
};

class AstrioAssembler {
private:
    uint32_t pc, pc_start;
    const uint32_t pc_inc;
    const Instruction NOP = Instruction{"nop", pc, I(0, Register::$zero, Register::$zero, 0)};
    std::map<std::string, uint32_t> identifier_addr;
    std::vector<Instruction> insts;
    std::vector<PendingInstruction> pending_insts;

    inline uint32_t compile_j(uint32_t addr) { return J(0x2, addr); }

    inline void resolve_j(const PendingInstruction &inst) {
        insts.insert(insts.begin() + (inst.addr - pc_start) / pc_inc + 1,
                     Instruction{inst.asm_code, inst.addr, compile_j(identifier_addr[inst.pending_identifier])});
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

        return std::stoi(binary.str(), nullptr, 2);
    }

    uint32_t I(uint8_t opcode, Register rs, Register rt, uint16_t immi) {
        std::stringstream binary;

        binary << toBinary<6>(opcode);
        binary << toBinary<5>((uint32_t) rs);
        binary << toBinary<5>((uint32_t) rt);
        binary << toBinary<16>(immi);

        return std::stoi(binary.str(), nullptr, 2);
    }

    uint32_t J(uint8_t opcode, uint32_t addr) {
        std::stringstream binary;

        binary << toBinary<6>(opcode);
        binary << toBinary<26>(addr);

        return std::stoi(binary.str(), nullptr, 2);
    }
    // endregion

public:
    explicit AstrioAssembler(uint32_t inst_start, uint32_t inst_inc = 4) :
            pc(inst_start), pc_start(inst_start), pc_inc(inst_inc) {}

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
            ss << "$a" << id - 16;
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

    std::vector<Instruction> assemble() {
        for (const auto &pending_inst : pending_insts) {
            std::string s = pending_inst.pending_identifier;
            if (identifier_addr.find(s) == identifier_addr.end()) {
                fprintf(stderr, "unexpected EOL but %s still undefined.", s.c_str());
                abort();
            }

            if (pending_inst.type == ResolvableInstruction::J)
                resolve_j(pending_inst);
        }

        return insts;
    }

    AstrioAssembler *claim(const std::string &identifier) {
        identifier_addr.insert(std::pair(identifier, pc));
        return this;
    }

    AstrioAssembler *add(Register rd, Register rs, Register rt) {
        incPC();
        std::stringstream ss;
        ss << "add " << nameOf(rd) << ", " << nameOf(rs) << ", " << nameOf(rt);
        insts.push_back(Instruction{ss.str(), pc, R(0, rs, rt, rd, 0, 0x20)});
        return this;
    }

    AstrioAssembler *addi(Register rt, Register rs, uint16_t immi) {
        incPC();
        std::stringstream ss;
        ss << "addi " << nameOf(rt) << ", " << nameOf(rs) << ", " << immi;
        insts.push_back(Instruction{ss.str(), pc, I(0x8, rs, rt, immi)});
        return this;
    }

    AstrioAssembler *sub(Register rd, Register rs, Register rt) {
        incPC();
        std::stringstream ss;
        ss << "sub " << nameOf(rd) << ", " << nameOf(rs) << ", " << nameOf(rd);
        insts.push_back(Instruction{ss.str(), pc, R(0, rs, rt, rd, 0, 0x22)});
        return this;
    }

    AstrioAssembler *and_(Register rd, Register rs, Register rt) {
        incPC();
        std::stringstream ss;
        ss << "and " << nameOf(rd) << ", " << nameOf(rs) << ", " << nameOf(rd);
        insts.push_back(Instruction{ss.str(), pc, R(0, rs, rt, rd, 0, 0x24)});
        return this;
    }

    AstrioAssembler *andi(Register rt, Register rs, uint16_t immi) {
        incPC();
        std::stringstream ss;
        ss << "andi " << nameOf(rt) << ", " << nameOf(rs) << ", " << immi;
        insts.push_back(Instruction{ss.str(), pc, I(0xc, rs, rt, immi)});
        return this;
    }

    AstrioAssembler *or_(Register rd, Register rs, Register rt) {
        incPC();
        std::stringstream ss;
        ss << "or " << nameOf(rd) << ", " << nameOf(rs) << ", " << nameOf(rd);
        insts.push_back(Instruction{ss.str(), pc, R(0, rs, rt, rd, 0, 0x25)});
        return this;
    }

    AstrioAssembler *ori(Register rt, Register rs, uint16_t immi) {
        incPC();
        std::stringstream ss;
        ss << "ori " << nameOf(rt) << ", " << nameOf(rs) << ", " << immi;
        insts.push_back(Instruction{ss.str(), pc, I(0xd, rs, rt, immi)});
        return this;
    }

    AstrioAssembler *move(Register rt, Register rs) { return ori(rt, rs, 0); }

    AstrioAssembler *li(Register rt, uint16_t immi) { return ori(rt, Register::$zero, immi); }

    AstrioAssembler *nop() {
        incPC();
        insts.push_back(NOP);
        return this;
    }

    AstrioAssembler *sll(Register rd, Register rt, uint16_t shamt) {
        incPC();
        std::stringstream ss;
        ss << "sll " << nameOf(rd) << ", " << nameOf(rt) << ", " << shamt;
        insts.push_back(Instruction{ss.str(), pc, R(0x0, Register::$zero, rt, rd, shamt, 0x0)});
        return this;
    }

    AstrioAssembler *srl(Register rd, Register rt, uint16_t shamt) {
        incPC();
        std::stringstream ss;
        ss << "srl " << nameOf(rd) << ", " << nameOf(rt) << ", " << shamt;
        insts.push_back(Instruction{ss.str(), pc, R(0x0, Register::$zero, rt, rd, shamt, 0x2)});
        return this;
    }

    AstrioAssembler *j(const std::string &identifier) {
        if (identifier_addr.find(identifier) == identifier_addr.end()) {
            incPC();
            pending_insts.push_back(PendingInstruction{ResolvableInstruction::J, "j " + identifier, identifier, pc});
            return this;
        } else
            return j(identifier_addr[identifier]);
    }

    AstrioAssembler *j(uint32_t addr) {
        incPC();
        std::stringstream ss;
        ss << "j " << std::hex << addr;
        insts.push_back(Instruction{ss.str(), pc, compile_j(addr)});
        return this;
    }
};

#endif //ASTRIO_ASSEMBLER_HPP
