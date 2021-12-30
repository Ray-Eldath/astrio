package Parameters;
    parameter InstSpace/* verilator public */=1024;
    parameter InstStartFrom/* verilator public */=32'h0;
    parameter InstStartFromActual=InstStartFrom-'h4;

    parameter MemStaticStartFrom/* verilator public */=32'h1000_0000;
    parameter MemStaticSpace/* verilator public */=1024;
    parameter MemDynamicStartFrom/* verilator public */=32'h1000_8000;
    parameter MemDynamicSpace/* verilator public */=1024;

    parameter GPAt/* verilator public */=MemDynamicStartFrom;
    parameter SPAt/* verilator public */=MemDynamicStartFrom+MemDynamicSpace;
endpackage : Parameters
