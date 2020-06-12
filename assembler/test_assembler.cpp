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

    astrio->add(Register::$t0, Register::$t1, Register::$t2)
            ->claim("abc");
    astrio->li(Register::$t1, 13); // $t1 = 13
    astrio->add(Register::$t0, Register::$t1, Register::$t2); // $t0 = 13
    astrio->addi(Register::$t1, Register::$t1, 11); // $t1 = 24
    astrio->andi(Register::$t3, Register::$t1, 0xcdef); // $t3 = 8
    astrio->sll(Register::$t3, Register::$t3, 4); // $t3 = 128
    astrio->j("dead_loop");
    astrio->srl(Register::$t3, Register::$t3, 2); // $t3 = 32
    astrio->srl(Register::$t4, Register::$t4, 2)
            ->claim("dead_loop"); // $t4 = 0
    astrio->j("abc");

    for (const auto &inst: astrio->assemble())
        print(inst);
    return 0;
}
