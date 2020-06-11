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

struct Storage_TestCase {
    const char name[10];
    uint32_t addr;
    uint8_t enable_write;
    uint32_t write_data;
    uint32_t expected_out;
};

class Storage_Tester : public TESTER<Memory, Storage_TestCase> {
public:
    void onEach(Storage_TestCase testcase, TPRINTER *t) override {
        DUT->addr = testcase.addr;
        DUT->enable_write = testcase.enable_write;
        DUT->write_data = testcase.write_data;
    }

    void afterEach(Storage_TestCase testcase, TPRINTER *t) override {
        auto name = colorize(lrpad(testcase.name), ForegroundColor::BLACK, BackgroundColor::BLUE);
        if (testcase.expected_out == IGNORE_OUT) {
            t->TPRINTF("%s  enable_write=%d  write_data=%#-10x\n", name, testcase.enable_write, testcase.write_data);
            return;
        }
        TASSERTF_INFO("%#-10x", DUT->read_out, testcase.expected_out,
                      "%s  enable_write=%d  write_data=%#-10x", name, testcase.enable_write, testcase.write_data);
    }

    Storage_Tester(const std::string &testname, const std::vector<Storage_TestCase> &testcases) :
            TESTER(testname, testcases) {}
};

uint32_t mem_start = Memory_Parameters::MemStartFrom;
uint32_t mem_space = Memory_Parameters::MemSpace;
std::vector<Storage_TestCase> testcases = {
        Storage_TestCase{"write", mem_start + 1, 1, 0x213, IGNORE_OUT},
        Storage_TestCase{"read", mem_start + 1, 0, 0, 0x213},
        Storage_TestCase{"write", mem_start + mem_space, 1, 0x1103, IGNORE_OUT},
        Storage_TestCase{"read", mem_start + mem_space, 0, 0, 0x1103},
};

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);

    auto storage = new Storage_Tester("storage", testcases);
    storage->run();
    delete storage;

    exit(EXIT_SUCCESS);
}
