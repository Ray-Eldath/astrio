//
// Created by Ray Eldath on 2020/6/11 0011.
//
#include "Memory.h"
#include "Memory_Parameters.h"
#include "verilated.h"
#include <vector>
#include "althas/config.h"

#undef RESET_LINE_ENABLED

#include "althas/althas.hpp"

#define IGNORE_OUT 0x1234

struct Memory_TestCase {
    const char name[10];
    uint32_t addr;
    uint8_t enable_write;
    uint32_t write_data;
    uint32_t expected_out;
};

class Memory_Tester : public TESTER<Memory, Memory_TestCase> {
public:
    void onEach(Memory_TestCase testcase, TPRINTER *t) override {
        DUT->addr = testcase.addr;
        DUT->enable_write = testcase.enable_write;
        DUT->write_data = testcase.write_data;
    }

    void afterEach(Memory_TestCase testcase, TPRINTER *t) override {
        auto name = colorize(lrpad(testcase.name), ForegroundColor::BLACK, BackgroundColor::BLUE);
        if (testcase.expected_out == IGNORE_OUT) {
            t->TPRINTF("%s  enable_write=%d  write_data=%#-10x\n", name, testcase.enable_write, testcase.write_data);
            return;
        }
        TASSERTF_INFO("%#-10x", DUT->read_out, testcase.expected_out,
                      "%s  enable_write=%d  write_data=%#-10x", name, testcase.enable_write, testcase.write_data);
    }

    Memory_Tester(const std::string &testname, const std::vector<Memory_TestCase> &testcases) :
            TESTER(testname, testcases) {}
};

std::vector<Memory_TestCase> testcases = {
        Memory_TestCase{"write", Memory_Parameters::GPAt, 1, 0x213, IGNORE_OUT},
        Memory_TestCase{"read", Memory_Parameters::GPAt, 0, 0, 0x213},
        Memory_TestCase{"write", Memory_Parameters::SPAt, 1, 0x1103, IGNORE_OUT},
        Memory_TestCase{"read", Memory_Parameters::SPAt, 0, 0, 0x1103},
};

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);

    auto memory = new Memory_Tester("memory", testcases);
    memory->run();
    delete memory;

    exit(EXIT_SUCCESS);
}
