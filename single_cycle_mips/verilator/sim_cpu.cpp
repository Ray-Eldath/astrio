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

        printf("%s\n\n", colorize("all instructions uploaded. selecting chip & resetting DUT.",
                                  ForegroundColor::UNSPECIFIED, BackgroundColor::UNSPECIFIED, Effect::UNDERLINE));

        DUT->ExpMipsCPU__DOT__loading_inst = 0;
        DUT->chip_select = 1;
        reset();
        tick();
    }

    void onEach(CPU_TestCase testcase, TPRINTER *t) override {
        auto regs_p = DUT->ExpMipsCPU__DOT__registers_m__DOT__regs;
        uint32_t regs[32];
        std::copy(regs_p, regs_p + 32, regs);

        t->printIndent();
        setColor(ForegroundColor::BLACK, BackgroundColor::BLUE);
        std::cout << " " << std::hex << DUT->ExpMipsCPU__DOT__pc << " ";
        resetColor();
        std::cout << " ";
        setColor(ForegroundColor::BLACK, BackgroundColor::UNSPECIFIED, Effect::BOLD);
        std::cout << std::left << std::setfill(' ') << std::setw(25) << testcase.inst.asm_code;
        resetColor();
        std::cout << ": " << std::right << std::setfill('0') << std::setw(8) << testcase.inst.compiled << " | ";
        setColor(ForegroundColor::CYAN);
        std::cout <<
                  "$gp=" << regs[28] << " $sp=" << regs[29] << " $fp=" << regs[30] << " $ra=" << regs[31] << std::endl;
        resetColor();

        t->increaseIndent();
        t->printIndent();
        for (int i = 1; i < 28; ++i) {
            if (regs[i] != 0)
                setColor(ForegroundColor::BLACK, BackgroundColor::UNSPECIFIED, Effect::BOLD);
            printf("%s=%d ", AstrioAssembler::nameOf((Register) i).c_str(), regs[i]);
            resetColor();
        }
        t->decreaseIndent();

        printf("\n");
    }
};

std::vector<Instruction> buildInsts(AstrioAssembler *astrio) {
    astrio->li(Register::$t1, 13);
    astrio->add(Register::$t0, Register::$t1, Register::$t2)
            ->claim("dead_loop");
    astrio->addi(Register::$t1, Register::$t1, 11);
    astrio->andi(Register::$t3, Register::$t1, 0xcdef);
    astrio->sll(Register::$t3, Register::$t3, 4);
    astrio->srl(Register::$t3, Register::$t3, 2);
    astrio->srl(Register::$t4, Register::$t4, 2);
    astrio->j("dead_loop");
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