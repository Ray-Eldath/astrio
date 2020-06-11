//
// Created by Ray Eldath on 2020/6/11 0011.
//
#include "CPU.h"
#include "CPU_Parameters.h"
#include "althas/althas.hpp"
#include "verilated.h"
#include "assembler.hpp"
#include <vector>
#include <algorithm>
#include <iostream>

struct CPU_TestCase {
    const Instruction inst;
};

class CPU_Tester : public TESTER<CPU, CPU_TestCase> {
public:
    CPU_Tester(const std::string &testname, const std::vector<CPU_TestCase> &testcases) : TESTER(testname, testcases) {}

    void beforeTraceStarts() override {
        DUT->ExpMipsCPU__DOT__loading_inst = 1;
        DUT->chip_select = 0;

        tick();
        for (const auto &testcase: d_testcases) {
            DUT->ExpMipsCPU__DOT__load_inst = testcase.inst.compiled;
            tick();

            std::cout << "inst " << std::hex <<
                      testcase.inst.asm_code << " " << testcase.inst.compiled << " uploaded to DUT" << std::endl;
            std::cout << "current pc: " << DUT->ExpMipsCPU__DOT__pc << std::endl;
        }

        printf("%s\n\n", colorize("all instructions uploaded.",
                                  ForegroundColor::UNSPECIFIED, BackgroundColor::UNSPECIFIED, Effect::UNDERLINE));

        DUT->ExpMipsCPU__DOT__loading_inst = 0;
        DUT->chip_select = 1;
        reset();
    }

    void onEach(CPU_TestCase testcase, TPRINTER *t) override {
        auto regs_p = DUT->ExpMipsCPU__DOT__registers_m__DOT__regs;
        uint32_t regs[32];
        std::copy(regs_p, regs_p + 32, regs);

        t->printIndent();
        setColor(ForegroundColor::BLACK, BackgroundColor::BLUE);
        std::cout << " " << testcase.inst.asm_code << " ";
        resetColor();
        std::cout << " " << std::hex << DUT->ExpMipsCPU__DOT__pc << std::endl;

        t->increaseIndent();
        t->printIndent();
        for (int i = 0; i < 32; ++i)
            printf("%s=%d ", nameOf((Register) i).c_str(), regs[i]);
        t->decreaseIndent();

        printf("\n");
    }
};

std::vector<CPU_TestCase> testcases = {
        CPU_TestCase{addi(Register::$t1, Register::$zero, 13)},
        CPU_TestCase{add(Register::$t0, Register::$t1, Register::$t2)},
        CPU_TestCase{nop()}
};

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);

    auto cpu = new CPU_Tester("astrio_scmips", testcases);
    cpu->run();
    delete cpu;

    exit(EXIT_SUCCESS);
}