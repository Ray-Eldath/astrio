//
// Created by Ray Eldath on 2020/6/11 0011.
//

#include "assembler.hpp"
#include "disassembler.hpp"
#include <string>
#include <iostream>
#include <iomanip>

using namespace std;

void print(const Instruction &inst, const std::string &disassembled) {
    cout << hex << right << setw(8) << setfill('0') << inst.addr << " - ";
    cout << hex << right << setw(8) << setfill('0') << inst.compiled << " # disassembled: " << disassembled << endl;
}

int main() {
    auto astrio = new AstrioAssembler(0x400000);

    astrio  // sum 1 to 5
            ->li(Register::$t1, 1)
            ->li(Register::$s1, 0)
            ->claim("loop")->add(Register::$s1, Register::$t1, Register::$s1)
            ->addi(Register::$t1, Register::$t1, 1)
            ->slti(Register::$t2, Register::$t1, 6)
            ->bne(Register::$t2, Register::$zero, "loop");

    astrio->nop()->nop()->nop();

    astrio
            ->li(Register::$a0, 5)
            ->jal("fact")
            ->j("exit")
            ->claim("fact")->addi(Register::$sp, Register::$sp, -8)
            ->sw(Register::$a0, Register::$sp, 0)
            ->sw(Register::$ra, Register::$sp, 4)
            ->slti(Register::$t0, Register::$a0, 1)
            ->beq(Register::$t0, Register::$zero, "recur")
            ->addi(Register::$v0, Register::$zero, 1)
            ->addi(Register::$sp, Register::$sp, 8)
            ->jr(Register::$ra)
            ->claim("recur")->addi(Register::$a0, Register::$a0, -1)
            ->jal("fact")
            ->lw(Register::$a0, Register::$sp, 0)
            ->lw(Register::$ra, Register::$sp, 4)
            ->addi(Register::$sp, Register::$sp, 8)
            ->add(Register::$v0, Register::$a0, Register::$v0)
            ->jr(Register::$ra)
            ->claim("exit")->nop();

    auto disassembler = new AstrioDisassembler(0x4);
    for (const auto &inst: astrio->assemble())
        print(inst, disassembler->disassemble(inst.addr, inst.compiled));
    return 0;
}
