//
// Created by Ray Eldath on 2020/6/15 0015.
//

#ifndef ASTRIO_TRANSLATOR_HPP
#define ASTRIO_TRANSLATOR_HPP

#include <string>
#include <sstream>

typedef struct R {
    std::string rs, rt, rd, opcode, funct;
    uint16_t shamt;
} R_inst;

typedef struct I {
    std::string rs, rt, opcode;
    int16_t immi;
} I_inst;

typedef struct J {
    std::string opcode;
    uint32_t addr;
} J_inst;

class AstrioDisassembler {
private:
    uint16_t pc_inc;

    static inline std::string lpadAddr(uint32_t addr) {
        std::stringstream ss;
        ss << "0x" << std::hex << addr;
        return ss.str();
    }

    static inline I_inst parse_I(const std::string &bin) {
        return I_inst{
                nameOf((Register) std::stoi(bin.substr(6, 5), nullptr, 2)),
                nameOf((Register) std::stoi(bin.substr(11, 5), nullptr, 2)),
                bin.substr(0, 6),
                (int16_t) std::stol(bin.substr(16, 16), nullptr, 2)
        };
    }

    [[nodiscard]] inline std::string disassemble_I(uint32_t addr, const I_inst &inst) const {
        std::stringstream ss;
        if (inst.opcode == "001000")
            ss << "addi " << inst.rt << ", " << inst.rs << ", " << inst.immi;
        else if (inst.opcode == "001010")
            ss << "slti " << inst.rt << ", " << inst.rs << ", " << inst.immi;
        else if (inst.opcode == "001100")
            ss << "andi " << inst.rt << ", " << inst.rs << ", " << inst.immi;
        else if (inst.opcode == "001101")
            ss << "ori " << inst.rt << ", " << inst.rs << ", " << inst.immi;
        else if (inst.opcode == "000100")
            ss << "beq " << inst.rs << ", " << inst.rt << ", " << lpadAddr(addr + (inst.immi << 2) + pc_inc);
        else if (inst.opcode == "000101")
            ss << "bne " << inst.rs << ", " << inst.rt << ", " << lpadAddr(addr + (inst.immi << 2) + pc_inc);
        else if (inst.opcode == "100011")
            ss << "lw " << inst.rt << ", " << inst.immi << "(" << inst.rs << ")";
        else if (inst.opcode == "101011")
            ss << "sw " << inst.rt << ", " << inst.immi << "(" << inst.rs << ")";

        return ss.str();
    }

    static inline R_inst parse_R(const std::string &bin) {
        return R_inst{
                nameOf((Register) std::stoi(bin.substr(6, 5), nullptr, 2)),
                nameOf((Register) std::stoi(bin.substr(11, 5), nullptr, 2)),
                nameOf((Register) std::stoi(bin.substr(16, 5), nullptr, 2)),
                bin.substr(0, 6),
                bin.substr(26, 6),
                (uint16_t) std::stoi(bin.substr(21, 5), nullptr, 2)
        };
    }

    static inline std::string disassemble_R(const R_inst &inst) {
        std::stringstream ss;
        if (inst.funct == "100000")
            ss << "add " << inst.rd << ", " << inst.rs << ", " << inst.rt;
        else if (inst.funct == "100010")
            ss << "sub " << inst.rd << ", " << inst.rs << ", " << inst.rt;
        else if (inst.funct == "100100")
            ss << "and " << inst.rd << ", " << inst.rs << ", " << inst.rt;
        else if (inst.funct == "100101")
            ss << "or " << inst.rd << ", " << inst.rs << ", " << inst.rt;
        else if (inst.funct == "001000")
            ss << "jr " << inst.rs;
        else if (inst.funct == "000000") {
            if (inst.rs == "$zero" && inst.rt == "$zero" && inst.rd == "$zero" && inst.shamt == 0)
                ss << "nop";
            else ss << "sll " << inst.rt << ", " << inst.shamt;
        } else if (inst.funct == "000010")
            ss << "srl " << inst.rt << ", " << inst.shamt;
        else if (inst.funct == "101010")
            ss << "slt " << inst.rd << ", " << inst.rs << ", " << inst.rt;

        return ss.str();
    }

    static inline J_inst parse_J(const std::string &bin) {
        return J_inst{
                bin.substr(0, 6),
                (uint32_t) std::stol(bin.substr(6, 26), nullptr, 2)
        };
    }

    static inline std::string disassemble_J(const J_inst &inst) {
        std::stringstream ss;
        if (inst.opcode == "000010")
            ss << "j " << lpadAddr(inst.addr << 2);
        else if (inst.opcode == "000011")
            ss << "jal " << lpadAddr(inst.addr << 2);

        return ss.str();
    }

public:
    explicit AstrioDisassembler(uint16_t pcInc) : pc_inc(pcInc) {}

    std::string disassemble(uint32_t addr, uint32_t inst) {
        std::string bin = std::bitset<32>(inst).to_string();
        std::string opcode = bin.substr(0, 6);
        if (opcode == "000000")
            return disassemble_R(parse_R(bin));
        else {
            std::string i = disassemble_I(addr, parse_I(bin));
            return i.empty() ? disassemble_J(parse_J(bin)) : i;
        }
    }
};

#endif //ASTRIO_TRANSLATOR_HPP
