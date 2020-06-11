//
// Created by Ray Eldath on 2020/6/11 0011.
//

#include "assembler.hpp"
#include <string>
#include <iostream>

using namespace std;

int main() {
    auto add_ = add(Register::$t0, Register::$t1, Register::$t2);
    cout << add_.asm_code << endl << add_.compiled << endl;
    auto addi_ = addi(Register::$zero, Register::$t1, 13);
    cout << addi_.asm_code << endl << addi_.compiled << endl;

    return 0;
}