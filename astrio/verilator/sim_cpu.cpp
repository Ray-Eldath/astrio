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

using namespace std;

auto disassembler = new AstrioDisassembler(0x4);

class CPU_InstsTester : public TESTER<CPU, Instruction> {
private:
    const bool show_upload_status_;
public:
    CPU_InstsTester(const std::string &testname,
                    const std::vector<Instruction> &insts,
                    const bool show_upload_status = false) :
            TESTER(testname, insts), show_upload_status_(show_upload_status) {}

    void beforeStart() override {
        DUT->ExpMipsCPU__DOT__loading_inst = 1;
        DUT->chip_select = 0;

        tick();
        for (const auto &testcase: d_testcases) {
            DUT->ExpMipsCPU__DOT__load_inst = testcase.compiled;
            tick();
            if (show_upload_status_) {
                auto compiled = testcase.compiled;
                cout << "inst " << hex << compiled << " "
                     << disassembler->disassemble(testcase.addr, compiled) << " uploaded to DUT" << endl;
                cout << "current pc: " << DUT->ExpMipsCPU__DOT__pc << std::endl;
            }
        }

        printf("%s\n\n", colorize("all instructions uploaded. selecting chip & resetting DUT.",
                                  ForegroundColor::UNSPECIFIED, BackgroundColor::UNSPECIFIED, Effect::UNDERLINE));

        DUT->ExpMipsCPU__DOT__loading_inst = 0;
        DUT->chip_select = 1;
        reset();
        tick();
    }

    void onEach(Instruction testcase, TPRINTER *t) override {
        auto regs_p = DUT->ExpMipsCPU__DOT__registers_m__DOT__regs;
        uint32_t pc = DUT->ExpMipsCPU__DOT__pc, inst = DUT->ExpMipsCPU__DOT__inst;
        uint32_t regs[32];
        copy(regs_p, regs_p + 32, regs);

        t->printIndent();
        setColor(ForegroundColor::BLACK, BackgroundColor::BLUE);
        cout << " " << std::hex << pc << " ";
        resetColor();
        cout << " ";
        setColor(ForegroundColor::BLACK, BackgroundColor::UNSPECIFIED, Effect::BOLD);
        cout << left << setfill(' ') << setw(25) << disassembler->disassemble(pc, inst);
        resetColor();
        cout << ": " << right << setfill('0') << setw(8) << inst << " | ";
        setColor(ForegroundColor::CYAN);
        cout << "$gp=" << regs[28] << " $sp=" << regs[29] << " $fp=" << regs[30] << " $ra=" << regs[31] << endl;
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

inline CPU_InstsTester *buildTestCase(const string &name,
                                      AstrioAssembler *insts,
                                      int spin_off = 0,
                                      bool show_upload_status = false) {
    for (int i = 0; i < spin_off; i++)
        insts->nop();
    return new CPU_InstsTester("astrio_" + name, insts->assemble(), show_upload_status);
}

CPU_InstsTester *test_loop_sum() {
    auto astrio = new AstrioAssembler(CPU_Parameters::InstStartFrom);

    astrio  // sum 1 to 5
            ->li(Register::$t1, 1)
            ->li(Register::$s1, 0)
            ->claim("loop")->add(Register::$s1, Register::$t1, Register::$s1)
            ->addi(Register::$t1, Register::$t1, 1)
            ->slti(Register::$t2, Register::$t1, 6)
            ->bne(Register::$t2, Register::$zero, "loop");

    return buildTestCase("loop_sum", astrio, 20);
}

CPU_InstsTester *test_lw_sw() {
    auto astrio = new AstrioAssembler(CPU_Parameters::InstStartFrom);

    astrio
            ->addi(Register::$sp, Register::$sp, -4)
            ->sw(Register::$s1, Register::$sp, 0)
            ->li(Register::$s1, 0)
            ->lw(Register::$s1, Register::$sp, 0);

    return buildTestCase("lw_sw", astrio);
}

CPU_InstsTester *test_recursive_fib() {
    auto astrio = new AstrioAssembler(CPU_Parameters::InstStartFrom);

    astrio  // fib
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

    return buildTestCase("recursive_fib", astrio, 75);
}

CPU_InstsTester *test_debug() {
    auto astrio = new AstrioAssembler(CPU_Parameters::InstStartFrom);

    astrio
            ->addi(Register::$t1, Register::$zero, 1)
            ->addi(Register::$t2, Register::$zero, 2)
            ->addi(Register::$t3, Register::$zero, 3)
            ->addi(Register::$t4, Register::$zero, 4)
            ->addi(Register::$t5, Register::$zero, 5)
            ->sw(Register::$t1, Register::$gp, 0)
            ->add(Register::$t1, Register::$t1, Register::$t1)
            ->nop()
            ->nop()
            ->nop()
            ->nop()
            ->nop()
            ->lw(Register::$t6, Register::$gp, 0);

    return buildTestCase("debug", astrio, 10);
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);

//    auto loop_sum = test_loop_sum();
//    loop_sum->run();
//    auto lw_sw = test_lw_sw();
//    lw_sw->run();
//    auto recursive_fib = test_recursive_fib();
//    recursive_fib->run();
    auto debug = test_debug();
    debug->run();

//    delete loop_sum;
//    delete lw_sw;
//    delete recursive_fib;
    delete debug;

    exit(EXIT_SUCCESS);
}