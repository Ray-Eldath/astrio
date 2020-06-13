//
// Created by Ray Eldath on 2020/6/10 0010.
//
#include "Fetcher.h"
#include "Fetcher_Parameters.h"
#include "verilated.h"
#include <vector>
#include <iostream>
#include <cstdlib>
#include "althas/config.h"

#undef RESET_LINE_ENABLED

#include "althas/althas.hpp"
#include "assembler.hpp"

struct Fetcher_TestCase {
    std::string name;
    uint8_t cs;
    uint32_t addr;
    uint32_t inst;
};

class Fetcher_Tester : public TESTER<Fetcher, Fetcher_TestCase> {
public:
    Fetcher_Tester(const std::string &testname, const std::vector<Fetcher_TestCase> &testcases) :
            TESTER(testname, testcases) {}

    void beforeAll() override {
        DUT->chip_select = 0;
        DUT->load = 1;

        for (const auto &testcase: d_testcases) {
            DUT->addr = testcase.addr;
            DUT->load_inst = testcase.inst;
            tick();
        }

        DUT->chip_select = 1;
        DUT->load = 0;
        tick();
    }

    void onEach(Fetcher_TestCase testcase, TPRINTER *t) override {
        DUT->chip_select = testcase.cs;
        DUT->addr = testcase.addr;
    }

    void afterEach(Fetcher_TestCase testcase, TPRINTER *t) override {
        auto name = colorize(lrpad(testcase.name.c_str()), ForegroundColor::BLACK, BackgroundColor::BLUE);
        TASSERTF_INFO("%#-10x", DUT->inst, testcase.inst,
                      "%s  addr=%#x cs=%d", name, testcase.addr, testcase.cs);
    }
};

std::vector<Fetcher_TestCase> testcases;
const uint32_t inst_start_from = Fetcher_Parameters::InstStartFrom;

inline Fetcher_TestCase testcase(Instruction &inst, uint32_t addr) {
    auto name = inst.asm_code;
    return Fetcher_TestCase{name, 1, addr, inst.compiled};
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);

    auto astrio = new AstrioAssembler(inst_start_from);
    astrio->addi(Register::$t1, Register::$zero, 13)
            ->add(Register::$t0, Register::$t1, Register::$t2);

    for (auto inst: astrio->assemble())
        testcases.push_back(testcase(inst, inst.addr));
    testcases.push_back(Fetcher_TestCase{"cs0", 0, inst_start_from + 12, 0});
    testcases.push_back(Fetcher_TestCase{"cs0", 0, inst_start_from + 16, 0});

    auto fetcher = new Fetcher_Tester("fetcher", testcases);
    fetcher->run();
    delete fetcher;

    exit(EXIT_SUCCESS);
}