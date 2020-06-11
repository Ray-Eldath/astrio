//
// Created by Ray Eldath on 2020/6/11 0011.
//

#include "assembler.hpp"
#include <string>
#include <iostream>

using namespace std;

int main() {
    auto addi_ = addi(Register::$t1, Register::$zero, 13);
    cout << addi_.asm_code << endl << hex << addi_.compiled << endl;
    auto add_ = add(Register::$t0, Register::$t1, Register::$t2);
    cout << add_.asm_code << endl << add_.compiled << endl;

    return 0;
}