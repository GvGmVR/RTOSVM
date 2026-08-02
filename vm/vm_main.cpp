#include <iostream>
#include <filesystem>
#include "headers/vm_engine.h"

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    std::cout << "===========================================\n";
    std::cout << "    RTOS DSL VIRTUAL MACHINE RUNTIME       \n";
    std::cout << "===========================================\n\n";

    // Path to compiled bytecode file
    std::string bcPath = "blink.bc";
    if (argc > 1) {
        bcPath = argv[1];
    } else if (!fs::exists(bcPath) && fs::exists("../blink.bc")) {
        bcPath = "../blink.bc";
    }

    if (!fs::exists(bcPath)) {
        std::cerr << "[VM Error]: Could not find bytecode file '" << bcPath << "'!\n";
        std::cerr << "Run byteCoder.exe first to compile 'blink.dsl' -> 'blink.bc'\n";
        return 1;
    }

    // Initialize VMEngine
    VMEngine vm;

    // Load binary bytecode file (211 bytes)
    if (!vm.loadBytecodeFile(bcPath)) {
        return 1;
    }

    // Phase 1: Decode opcodes and wire up FSM state graph in RAM
    vm.decodeAndSetup();

    // Phase 2: Run 6 FSM state transitions (3 full blink cycles) for demo
    vm.runFSM(6);

    std::cout << "\n[VM Execution Completed Successfully]\n";
    return 0;
}