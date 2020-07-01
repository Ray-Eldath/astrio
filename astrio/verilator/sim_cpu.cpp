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
        DUT->Astrio__DOT__loading_inst = 1;
        DUT->chip_select = 0;

        for (const auto &testcase: d_testcases) {
            DUT->Astrio__DOT__load_inst = testcase.compiled;
            tick();
            if (show_upload_status_) {
                auto compiled = testcase.compiled;
                cout << "inst " << hex << compiled << " "
                     << disassembler->disassemble(testcase.addr, compiled) << " uploaded to DUT" << endl;
                cout << "current pc: " << DUT->Astrio__DOT__pc << std::endl;
            }
        }

        printf("%s\n\n", colorize("all instructions uploaded. selecting chip & resetting DUT.",
                                  ForegroundColor::UNSPECIFIED, BackgroundColor::UNSPECIFIED, Effect::UNDERLINE));

        DUT->Astrio__DOT__loading_inst = 0;
        DUT->chip_select = 1;
        reset();
    }

    void onEach(Instruction testcase, TPRINTER *t) override {
        auto regs_p = DUT->Astrio__DOT__registers_m__DOT__regs;
        uint32_t pc = DUT->Astrio__DOT__pc, inst = DUT->Astrio__DOT__inst_pre;
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
    for (int i = 0; i <= spin_off; i++)
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
            ->claim("nonsense")->addi(Register::$t1, Register::$zero, 1)
            ->addi(Register::$t2, Register::$zero, 2)
            ->addi(Register::$t3, Register::$zero, 3)
            ->addi(Register::$t4, Register::$zero, 4)
            ->addi(Register::$t5, Register::$zero, 5)
            ->sw(Register::$t1, Register::$gp, 0)
            ->add(Register::$t1, Register::$t1, Register::$t1)
            ->nop()->nop()->nop()->nop()
            ->beq(Register::$t1, Register::$t2, "nonsense")
            ->jal("nonsense")
            ->lw(Register::$t6, Register::$gp, 0);

    return buildTestCase("debug", astrio, 10);
}

CPU_InstsTester *test_stall_lw() {
    auto astrio = new AstrioAssembler(CPU_Parameters::InstStartFrom);

    astrio
            ->li(Register::$t1, 213)
            ->sw(Register::$t1, Register::$sp, 0)
            ->lw(Register::$t2, Register::$sp, 0)
            ->move(Register::$s1, Register::$t2)
            ->li(Register::$s2, 312);

    return buildTestCase("stall_lw", astrio, 4);
}

CPU_InstsTester *test_bypassing_mem() {
    auto astrio = new AstrioAssembler(CPU_Parameters::InstStartFrom);

    astrio
            ->li(Register::$t1, 10)
            ->sw(Register::$t1, Register::$sp, 0)
            ->lw(Register::$s1, Register::$sp, 0)
            ->sw(Register::$s1, Register::$sp, -4)
            ->lw(Register::$s2, Register::$sp, -4);

    return buildTestCase("bypassing_mem", astrio, 5);
}

CPU_InstsTester *test_bypassing_ex() {
    auto astrio = new AstrioAssembler(CPU_Parameters::InstStartFrom);

    astrio
            ->li(Register::$t1, 10)
            ->li(Register::$t2, 5)
            ->add(Register::$t3, Register::$t1, Register::$t2) // 1b, 2a
            ->add(Register::$s1, Register::$t3, Register::$t1) // 1a
            ->add(Register::$s2, Register::$t1, Register::$t3); // 2b

    return buildTestCase("bypassing_ex", astrio, 5);
}

CPU_InstsTester *test_branching_stall1_bypassing_flush1() {
    auto astrio = new AstrioAssembler(CPU_Parameters::InstStartFrom);

    astrio
            ->li(Register::$s1, 10)
            ->nop()->nop() // manually stall ONE extra cc until bypassing logic is finished
            ->bne(Register::$s1, Register::$zero, "test")
            ->move(Register::$t1, Register::$s1) // should be flushed
            ->claim("test")->move(Register::$s2, Register::$s1);

    return buildTestCase("branching_stall1_bypassing_flush1", astrio, 5, true);
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
    auto bypassing_ex = test_bypassing_ex();
    bypassing_ex->run();
    auto bypassing_mem = test_bypassing_mem();
    bypassing_mem->run();
    auto stall_lw = test_stall_lw();
    stall_lw->run();
    auto branching1 = test_branching_stall1_bypassing_flush1();
    branching1->run();

//    delete loop_sum;
//    delete lw_sw;
//    delete recursive_fib;
    delete debug;
    delete bypassing_ex;
    delete bypassing_mem;
    delete stall_lw;
    delete branching1;

    exit(EXIT_SUCCESS);
}