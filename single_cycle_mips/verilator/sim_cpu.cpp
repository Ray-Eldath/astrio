//
// Created by Ray Eldath on 2020/6/11 0011.
//
#include "CPU.h"
#include "CPU_Parameters.h"
#include "althas/althas.hpp"
#include "assembler.hpp"
#include "disassembler.hpp"
#include "verilated.h"
#include <vector>
#include <algorithm>
#include <iostream>

struct CPU_TestCase {
    const Instruction inst;
};

auto disassembler = new AstrioDisassembler(0x4);

class CPU_Tester : public TESTER<CPU, CPU_TestCase> {
public:
    CPU_Tester(const std::string &testname, const std::vector<CPU_TestCase> &testcases) : TESTER(testname, testcases) {}

    void beforeStart() override {
        DUT->ExpMipsCPU__DOT__loading_inst = 1;
        DUT->chip_select = 0;

        tick();
        for (const auto &testcase: d_testcases) {
            DUT->ExpMipsCPU__DOT__load_inst = testcase.inst.compiled;
            tick();

//            auto compiled = testcase.inst.compiled;
//            std::cout << "inst " << std::hex << compiled << " "
//                      << disassembler->disassemble(testcase.inst.addr, compiled) << " uploaded to DUT" << std::endl;
//            std::cout << "current pc: " << DUT->ExpMipsCPU__DOT__pc << std::endl;
        }

        printf("%s\n\n", colorize("all instructions uploaded. selecting chip & resetting DUT.",
                                  ForegroundColor::UNSPECIFIED, BackgroundColor::UNSPECIFIED, Effect::UNDERLINE));

        DUT->ExpMipsCPU__DOT__loading_inst = 0;
        DUT->chip_select = 1;
        reset();
        tick();
    }

    void onEach(CPU_TestCase testcase, TPRINTER *t) override {
        auto regs_p = DUT->ExpMipsCPU__DOT__registers_m__DOT__regs;
        uint32_t pc = DUT->ExpMipsCPU__DOT__pc, inst = DUT->ExpMipsCPU__DOT__inst;
        uint32_t regs[32];
        std::copy(regs_p, regs_p + 32, regs);

        t->printIndent();
        setColor(ForegroundColor::BLACK, BackgroundColor::BLUE);
        std::cout << " " << std::hex << pc << " ";
        resetColor();
        std::cout << " ";
        setColor(ForegroundColor::BLACK, BackgroundColor::UNSPECIFIED, Effect::BOLD);
        std::cout << std::left << std::setfill(' ') << std::setw(25) << disassembler->disassemble(pc, inst);
        resetColor();
        std::cout << ": " << std::right << std::setfill('0') << std::setw(8) << inst << " | ";
        setColor(ForegroundColor::CYAN);
        std::cout <<
                  "$gp=" << regs[28] << " $sp=" << regs[29] << " $fp=" << regs[30] << " $ra=" << regs[31] << std::endl;
        resetColor();

        t->increaseIndent();
        t->printIndent();
        for (int i = 1; i < 28; ++i) {
            if (regs[i] != 0)
                setColor(ForegroundColor::BLACK, BackgroundColor::UNSPECIFIED, Effect::BOLD);
            printf("%s=%d ", nameOf((Register) i).c_str(), regs[i]);
            resetColor();
        }
        t->decreaseIndent();

        printf("\n");
    }
};

std::vector<Instruction> buildInsts(AstrioAssembler *astrio) {
    astrio  // sum 1 to 5
            ->li(Register::$t1, 1)
            ->li(Register::$s1, 0)
            ->claim("loop")->add(Register::$s1, Register::$t1, Register::$s1)
            ->addi(Register::$t1, Register::$t1, 1)
            ->slti(Register::$t2, Register::$t1, 6)
            ->bne(Register::$t2, Register::$zero, "loop");

    astrio->nop()->nop()->nop();

//    astrio->addi(Register::$sp, Register::$sp, -4)
//            ->sw(Register::$s1, Register::$sp, 0)
//            ->li(Register::$s1, 0)
//            ->lw(Register::$s1, Register::$sp, 0);

    astrio
            ->li(Register::$a0, 1)
            ->li(Register::$a1, 1)
            ->li(Register::$a2, 5) // n-th of fib
            ->jal("fib")
            ->j("exit")
            ->claim("fib")->addi(Register::$sp, Register::$sp, -16)
            ->sw(Register::$a0, Register::$sp, 0)
            ->sw(Register::$a1, Register::$sp, 4)
            ->sw(Register::$a2, Register::$sp, 8)
            ->sw(Register::$ra, Register::$sp, 12)
            ->beq(Register::$a2, Register::$zero, "base")
            ->add(Register::$t1, Register::$a0, Register::$a1)
            ->move(Register::$a0, Register::$a1)
            ->move(Register::$a1, Register::$t1)
            ->addi(Register::$a2, Register::$a2, -1)
            ->jal("fib")
            ->lw(Register::$a0, Register::$sp, 0)
            ->lw(Register::$a1, Register::$sp, 4)
            ->lw(Register::$a2, Register::$sp, 8)
            ->lw(Register::$ra, Register::$sp, 12)
            ->addi(Register::$sp, Register::$sp, 16)
            ->jr(Register::$ra)
            ->claim("base")->add(Register::$v0, Register::$a0, Register::$a1)
            ->lw(Register::$a0, Register::$sp, 0)
            ->lw(Register::$a1, Register::$sp, 4)
            ->lw(Register::$ra, Register::$sp, 12)
            ->addi(Register::$sp, Register::$sp, 16)
            ->jr(Register::$ra)
            ->claim("exit")->nop();

    for (int i = 0; i < 100; i++)
        astrio->nop();

    return astrio->assemble();
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);

    std::vector<CPU_TestCase> testcases;
    for (const auto &inst: buildInsts(new AstrioAssembler(CPU_Parameters::InstStartFrom)))
        testcases.push_back(CPU_TestCase{inst});
    auto cpu = new CPU_Tester("astrio_scmips", testcases);
    cpu->run();
    delete cpu;

    exit(EXIT_SUCCESS);
}