//
// Created by Ray Eldath on 2020/6/11 0011.
//

#include "assembler.hpp"
#include <string>
#include <iostream>
#include <iomanip>

using namespace std;

void print(const Instruction &inst) {
    cout << hex << right << setw(8) << setfill('0') << inst.addr << " ";
    cout << left << setw(25) << setfill(' ') << inst.asm_code << " # compiled to " << inst.compiled << endl;
}

int main() {
    auto astrio = new AstrioAssembler(0x400000 - 0x4);

    astrio  // sum 1 to 5
            ->li(Register::$t1, 1)
            ->li(Register::$s1, 0)
            ->claim_next("loop")
            ->add(Register::$s1, Register::$t1, Register::$s1)
            ->addi(Register::$t1, Register::$t1, 1)
            ->slti(Register::$t2, Register::$t1, 6)
            ->bne(Register::$t2, Register::$zero, "loop");

    for (const auto &inst: astrio->assemble())
        print(inst);
    return 0;
}
