//
// Created by Ray Eldath on 2020/6/11 0011.
//

#include "assembler.hpp"
#include <string>
#include <iostream>

using namespace std;

void print(const Instruction &inst) {
    cout << inst.asm_code << hex << " # compiled to " << inst.compiled << endl;
}

int main() {
    print(li(Register::$t1, 13)); // $t1 = 13
    print(add(Register::$t0, Register::$t1, Register::$t2)); // $t0 = 13
    print(addi(Register::$t1, Register::$t1, 11)); // $t1 = 24
    print(andi(Register::$t3, Register::$t1, 0xcdef)); // $t3 = 8

    return 0;
}
