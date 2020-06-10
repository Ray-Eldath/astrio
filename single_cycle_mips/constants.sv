package Parameters;
    parameter InstSpace/* verilator public */=1024;
    parameter InstStartFrom/* verilator public */=32'h0040_0000;
    parameter MemSpace/* verilator public */=1024;
    parameter MemStaticStartFrom/* verilator public */=32'h1000_0000;
    parameter MemDynamicStartFrom/* verilator public */=32'h1000_8000;
    parameter MemStartFrom/* verilator public */=MemStaticStartFrom;

    parameter GPAt/* verilator public */=MemDynamicStartFrom;
    parameter SPAt/* verilator public */=MemDynamicStartFrom+MemSpace;
endpackage : Parameters
