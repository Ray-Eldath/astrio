//
// Created by Ray Eldath on 2020/6/9 0009.
//
#include "ALU.h"
#include "ALU_ALUType.h"
#include <vector>
#include <cstdint>

#include "althas/config.h"

#undef RESET_LINE_ENABLED
#undef CLOCK_LINE_ENABLED

#include "althas/althas.hpp"

#define IGNORE_OUT 0xABCD
struct ALU_TestCase {
    const char name[10];
    uint8_t cmd;
    int64_t a;
    int64_t b;
    int32_t expected_out;
    uint8_t expected_overflow;
    uint8_t expected_zero;
};

class ALU_Tester : public TESTER<ALU, ALU_TestCase> {
public:
    ALU_Tester(const std::string &testname, const std::string &vcdname, const std::vector<ALU_TestCase> &testcases)
            : TESTER(testname, vcdname, testcases) {}

    ALU_Tester(const std::string &testname, const std::vector<ALU_TestCase> &testcases) : TESTER(testname, testcases) {}

    void onEach(ALU_TestCase testcase, TPRINTER *t) override {
        DUT->cmd = testcase.cmd;
        DUT->a = testcase.a;
        DUT->b = testcase.b;
    }

    void afterEach(ALU_TestCase testcase, TPRINTER *t) override {
        t->printIndent();
        char name[20] = " ";
        strcat(name, testcase.name);
        strcat(name, " ");
        printf("%-10s  cmd=%-5d a=%-#8lX b=%-#8lX\n",
               colorize(name, ForegroundColor::BLACK, BackgroundColor::BLUE),
               testcase.cmd, testcase.a, testcase.b);
        t->increaseIndent();

        TASSERTF_INFO("%d", DUT->overflow, testcase.expected_overflow, "overflow=%d", DUT->overflow);
        TASSERTF_INFO("%d", DUT->zero, testcase.expected_zero, "zero=%d", DUT->zero);
        if (testcase.expected_out != IGNORE_OUT)
            TASSERTF_INFO("%#-10X", DUT->out, (uint32_t) testcase.expected_out, "out=%#-10X", DUT->out);

        t->decreaseIndent();
    }
};

typedef ALU_ALUType::alu_cmd_t cmd;
std::vector<ALU_TestCase> testcases = {
        ALU_TestCase{"and", cmd::AND, 0xFA, 0x1D, 0x18, 0, 0},
        ALU_TestCase{"or", cmd::OR, 0x1A, 0x11, 0x1B, 0, 0},
        ALU_TestCase{"add", cmd::ADD, 2, 3, 5, 0, 0},
        ALU_TestCase{"add zero", cmd::ADD, 2, -2, 0, 0, 1},
        ALU_TestCase{"sub", cmd::SUB, 2, 3, -1, 0, 0},
        ALU_TestCase{"sub zero", cmd::SUB, 2, 2, 0, 0, 1},
        ALU_TestCase{"overflow", cmd::ADD, INT32_MAX, 1, IGNORE_OUT, 1, 0},
        ALU_TestCase{"overflow", cmd::ADD, INT32_MAX, 5, IGNORE_OUT, 1, 0},
        ALU_TestCase{"underflow", cmd::SUB, INT32_MIN, 1, IGNORE_OUT, 1, 0},
        ALU_TestCase{"underflow", cmd::SUB, INT32_MIN, 5, IGNORE_OUT, 1, 0},
};

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);

    auto alu = new ALU_Tester("alu", testcases);
    alu->run();
    delete alu;

    exit(EXIT_SUCCESS);
}