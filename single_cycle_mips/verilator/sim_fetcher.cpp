//
// Created by Ray Eldath on 2020/6/10 0010.
//
#include "Fetcher.h"
#include "Fetcher_Parameters.h"
#include "verilated.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "althas/config.h"

#undef RESET_LINE_ENABLED

#include "althas/althas.hpp"

struct Fetcher_TestCase {
    const char name[20];
    uint32_t addr;
    uint32_t expected_inst;
};

class Fetcher_Tester : public TESTER<Fetcher, Fetcher_TestCase> {
public:
    Fetcher_Tester(const std::string &testname, const std::vector<Fetcher_TestCase> &testcases) :
            TESTER(testname, testcases) {}

    void onEach(Fetcher_TestCase testcase, TPRINTER *t) override {
        DUT->addr = testcase.addr;
    }

    void afterEach(Fetcher_TestCase testcase, TPRINTER *t) override {
        auto name = colorize(lrpad(testcase.name), ForegroundColor::BLACK, BackgroundColor::BLUE);
        TASSERTF_INFO("%#-10x", DUT->inst, testcase.expected_inst, "%s  addr=%#x", name, testcase.addr);
    }
};

const int rate = 100;
std::vector<Fetcher_TestCase> testcases;
const uint32_t inst_start_from = Fetcher_Parameters::InstStartFrom;

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);

    std::ifstream fin("inst.data");
    char line[8];
    int lines = 0;
    while (fin.getline(line, 9)) { // caveat: \n or \r\n is included
        if (random() % 100 <= rate) {
            auto testcase = Fetcher_TestCase{"read", lines + inst_start_from, (uint32_t) strtol(line, nullptr, 16)};
//            printf("addr=%#-10x expected_inst=%#x\n", testcase.addr, testcase.expected_inst);
            testcases.push_back(testcase);
        }
        lines += 1;
    }

    auto fetcher = new Fetcher_Tester("fetcher", testcases);
    fetcher->run();
    delete fetcher;

    exit(EXIT_SUCCESS);
}