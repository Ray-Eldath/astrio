//
// Created by Ray Eldath on 2020/6/8 0008.
//
#include "PC.h"
#include "PC_PCType.h"
#include "PC_Parameters.h"
#include "althas/althas.hpp"
#include <vector>

struct PC_TestCase {
    char name[20];
    uint8_t cmd;
    uint8_t rst;
    uint32_t load_pc;
    uint32_t pc_expected;
};

uint32_t inst_start_from = PC_Parameters::_IDataInstStartFrom::InstStartFrom;
typedef PC_PCType::pc_cmd_t cmd;
std::vector<PC_TestCase> PC_testcases = {
        PC_TestCase{"inc", cmd::INC, 0, 0, inst_start_from + 0x4},
        PC_TestCase{"none", cmd::NONE, 0, 0, inst_start_from + 0x4},
        PC_TestCase{"none", cmd::NONE, 0, 0, inst_start_from + 0x4},
        PC_TestCase{"load", cmd::LOAD, 0, 0x1, 0x1},
        PC_TestCase{"rst", cmd::NONE, 1, 0, inst_start_from}
};

class PC_Tester : public TESTER<PC, PC_TestCase> {
private:
    void onEach(PC_TestCase testcase, TPRINTER *t) override {
        DUT->cmd = testcase.cmd;
        DUT->rst = testcase.rst;
        DUT->load_pc = testcase.load_pc;
    }

public:
    void afterEach(PC_TestCase testcase, TPRINTER *t) override {
        TASSERTF_INFO("%#-10x", DUT->pc, testcase.pc_expected, "%-6s rst: %d", testcase.name, DUT->rst);
    }

public:
    PC_Tester(const std::string &testname, const std::string &vcdname, const std::vector<PC_TestCase> &testcases)
            : TESTER(testname, vcdname, testcases) {}

    PC_Tester(const std::string &testname, const std::vector<PC_TestCase> &testcases) : TESTER(testname, testcases) {}
};

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);

    auto pc = new PC_Tester("pc", PC_testcases);
    pc->run();
    delete pc;

    exit(EXIT_SUCCESS);
}