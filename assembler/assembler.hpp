//
// Created by Ray Eldath on 2020/6/11 0011.
//

#ifndef ASTRIO_ASSEMBLER_HPP
#define ASTRIO_ASSEMBLER_HPP

#include <bitset>
#include <sstream>

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

struct Instruction {
    std::string asm_code;
    uint32_t compiled;
};

std::string nameOf(Register reg) {
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

Instruction add(Register rs, Register rt, Register rd) {
    std::stringstream ss;
    ss << "add " << nameOf(rd) << ", " << nameOf(rs) << ", " << nameOf(rd);
    return Instruction{ss.str(), R(0, rs, rt, rd, 0, 0x20)};
}

inline Instruction addi(Register rs, Register rt, uint16_t immi) {
    std::stringstream ss;
    ss << "addi " << nameOf(rt) << ", " << nameOf(rs) << ", " << immi;
    return Instruction{ss.str(), I(0x8, rs, rt, immi)};
}

#endif //ASTRIO_ASSEMBLER_HPP
