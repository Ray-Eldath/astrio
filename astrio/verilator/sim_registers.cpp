//
// Created by Ray Eldath on 2020/6/9 0009.
//
#include "Registers.h"
#include <vector>

#include "althas/config.h"

#undef RESET_LINE_ENABLED

#include "althas/althas.hpp"

struct Registers_TestCase {
    const char name[20];
    uint8_t read1;
    uint8_t read2;
    uint8_t enable_write;
    uint8_t write_id;
    uint32_t write_data;
    uint32_t expected_read1_out;
    uint32_t expected_read2_out;
};

#define IGNORE_READ 0x1234

class Registers_Tester : public TESTER<Registers, Registers_TestCase> {
public:
    void onEach(Registers_TestCase testcase, TPRINTER *t) override {
        DUT->read1 = testcase.read1;
        DUT->read2 = testcase.read2;
        DUT->enable_write = testcase.enable_write;
        DUT->write_id = testcase.write_id;
        DUT->write_data = testcase.write_data;
    }

    void afterEach(Registers_TestCase testcase, TPRINTER *t) override {
        t->printIndent();
        printf("%s %s %s  read1=%-4d read2=%-4d enable_write=%-4d write_id= %-4d write_data=%-#8X\n",
               color(ForegroundColor::BLACK, BackgroundColor::BLUE), testcase.name, RESET_COLOR,
               testcase.read1, testcase.read2, testcase.enable_write, testcase.write_id, testcase.write_data);
        t->increaseIndent();

        if (testcase.expected_read1_out != IGNORE_READ)
            TASSERTF_INFO("%-#8X", DUT->read1_out, testcase.expected_read1_out, "read1_out=%-#8X", DUT->read1_out);
        if (testcase.expected_read2_out != IGNORE_READ)
            TASSERTF_INFO("%-#8X", DUT->read2_out, testcase.expected_read2_out, "read2_out=%-#8X", DUT->read2_out);

        t->decreaseIndent();
    }

    Registers_Tester(const std::string &testname, const std::vector<Registers_TestCase> &testcases) :
            TESTER(testname, testcases) {}
};

std::vector<Registers_TestCase> testcases = {
        Registers_TestCase{"write12", 0, 0, 1, 12, 0xAF, IGNORE_READ, IGNORE_READ},
        Registers_TestCase{"read12", 12, 0, 0, 0, 0, 0xAF, IGNORE_READ},
        Registers_TestCase{"write13", 0, 0, 1, 13, 0xABCD, IGNORE_READ, IGNORE_READ},
        Registers_TestCase{"read13", 0, 13, 0, 0, 0, IGNORE_READ, 0xABCD},
        Registers_TestCase{"read1213", 12, 13, 0, 0, 0, 0xAF, 0xABCD},
        Registers_TestCase{"read12write13", 12, 0, 1, 13, 0x11, 0xAF, IGNORE_READ},
        Registers_TestCase{"read13", 0, 13, 0, 0, 0, IGNORE_READ, 0x11},
        Registers_TestCase{"read0write0", 0, 0, 1, 0, 0x11, 0, 0},
};

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);

    auto registers = new Registers_Tester("registers", testcases);
    registers->run();
    delete registers;

    exit(EXIT_SUCCESS);
}
