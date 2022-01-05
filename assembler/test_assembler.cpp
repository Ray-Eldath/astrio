//
// Created by Ray Eldath on 2020/6/11 0011.
//

#include "assembler.hpp"
#include "disassembler.hpp"
#include <string>
#include <iostream>
#include <iomanip>

using namespace std;

template<bool PRINT_ADDR = true, bool PRINT_DIS = true>
void print(const Instruction &inst, const std::string &disassembled) {
    if constexpr(PRINT_ADDR)
        cout << hex << right << setw(8) << setfill('0') << inst.addr << " - ";
    cout << hex << right << setw(8) << setfill('0') << inst.compiled;
    if constexpr(PRINT_DIS)
        cout << " // disassembled: " << disassembled;
    cout << endl;
}

unique_ptr<AstrioAssembler> build_assembler() { return std::make_unique<AstrioAssembler>(0x0); }

unique_ptr<AstrioDisassembler> build_disassembler() { return std::make_unique<AstrioDisassembler>(0x4); }

void print_disassemble(unique_ptr<AstrioAssembler> &astrio) {
    auto disassembler = build_disassembler();
    for (const auto &inst: astrio->assemble())
        print<false, false>(inst, disassembler->disassemble(inst.addr, inst.compiled));
    for (const auto &inst: astrio->assemble())
        print<true, true>(inst, disassembler->disassemble(inst.addr, inst.compiled));
}

void bubble_sort() {
    cout << endl << "=====================================" << endl;
    cout << "     MIPS CPU TEST - BUBBLE SORT     " << endl;
    cout << "=====================================" << endl;
/*
   1 #include "stdio.h"
   2
   3 int x[5] = {1, 5, 3, 2, 4};
   4 int main() {
   5     for (int i = 0; i < 5; i++)
   6         for (int j = i + 1; j < 5; j++)
   7             if (x[i] < x[j]) {
   8                 int t = x[i];
   9                 x[i] = x[j];
  10                 x[j] = t;
  11             }
  12
  13     for (int i = 0; i < 5; i++)
  14         printf("%d ", x[i]);
  15 }
  */
    auto astrio = build_assembler();

    astrio->li(Register::$sp, 0)
            ->li(Register::$a0, 8)
            ->sw(Register::$a0, Register::$sp, 4)
            ->li(Register::$a0, 2)
            ->sw(Register::$a0, Register::$sp, 8)
            ->li(Register::$a0, 5)
            ->sw(Register::$a0, Register::$sp, 12)
            ->li(Register::$a0, 3)
            ->sw(Register::$a0, Register::$sp, 16)
            ->li(Register::$a0, 1)
            ->sw(Register::$a0, Register::$sp, 20)
            ->li(Register::$a0, 0)
            ->sw(Register::$a0, Register::$sp, 24)
            ->li(Register::$a0, 6)
            ->sw(Register::$a0, Register::$sp, 28)
            ->li(Register::$a0, 4)
            ->sw(Register::$a0, Register::$sp, 32)
            ->li(Register::$a0, 7)
            ->sw(Register::$a0, Register::$sp, 36)
            ->nop()->nop()->nop()->nop()
            ->li(Register::$s0, 0)
            ->claim("outer")->slti(Register::$t0, Register::$s0, 32) // s0: i
            ->beq(Register::$t0, Register::$zero, "end")
            ->addi(Register::$s1, Register::$s0, 4) // s1: j
            ->addi(Register::$s0, Register::$s0, 4)
            ->claim("inner")->slti(Register::$t1, Register::$s1, 32)
            ->beq(Register::$t1, Register::$zero, "outer")
            ->addi(Register::$s1, Register::$s1, 4)
            ->lw(Register::$s3, Register::$s0, 0) // s3 <- [s0]; s3 <- x[i]
            ->lw(Register::$s4, Register::$s1, 0) // s4 <- [s1]; s4 <- x[j]
            ->slt(Register::$t3, Register::$s3, Register::$s4)
            ->beq(Register::$t3, Register::$zero, "inner")
            ->sw(Register::$s4, Register::$s0, 0) // swap
            ->sw(Register::$s3, Register::$s1, 0)
            ->j("inner")
            ->claim("end")->nop()->nop()->nop()->nop();

    print_disassemble(astrio);
}

void sum_1_to_5() {
    cout << endl << "=====================================" << endl;
    cout << "      MIPS CPU TEST - SUM 1 TO 5     " << endl;
    cout << "=====================================" << endl;
    auto astrio = build_assembler();

    astrio  // sum 1 to 5
            ->li(Register::$t1, 1)
            ->li(Register::$s1, 0)
            ->claim("loop")->add(Register::$s1, Register::$t1, Register::$s1)
            ->addi(Register::$t1, Register::$t1, 1)
            ->slti(Register::$t2, Register::$t1, 6)
            ->bne(Register::$t2, Register::$zero, "loop")
            ->move(Register::$k0, Register::$s1)
            ->claim("end")->nop()
            ->j(0x00000020);

    print_disassemble(astrio);
}

void factorial_recur() {
    cout << endl << "=====================================" << endl;
    cout << "      MIPS CPU TEST - FACT RECUR     " << endl;
    cout << "=====================================" << endl;
    auto astrio = build_assembler();

    astrio
            ->li(Register::$sp, 32)
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

    print_disassemble(astrio);
}

void bypassing() {
    cout << endl << "=====================================" << endl;
    cout << "      MIPS CPU TEST - BYPASSING      " << endl;
    cout << "=====================================" << endl;
    auto astrio = build_assembler();

    astrio
            ->li(Register::$t1, 10)
            ->li(Register::$t2, 5)
            ->add(Register::$t3, Register::$t1, Register::$t2) // 1b, 2a
            ->add(Register::$s1, Register::$t3, Register::$t1) // 1a
            ->add(Register::$s2, Register::$t1, Register::$t3) // 2b
            ->move(Register::$k0, Register::$s2);

    print_disassemble(astrio);
}

int main() {
    bubble_sort();
    sum_1_to_5();
    factorial_recur();
    bypassing();
    return 0;
}
