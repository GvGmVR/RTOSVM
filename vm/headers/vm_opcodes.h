#ifndef VM_OPCODES_H
#define VM_OPCODES_H

#include <cstdint>

// Bytecode Opcodes - MUST match the compiler's Opcode values in bytecode_gen.h
enum VMOpcode : uint8_t {
    OP_HALT          = 0x00, // Halts VM execution / signals end of instructions
    OP_DEFINE_NODE   = 0x01, // Defines a hardware node binding (e.g., LED, Motor)
    OP_DEFINE_TASK   = 0x02, // Defines an RTOS task configuration
    OP_DEFINE_STATE  = 0x03, // Defines an FSM state and its associated action
    OP_CONNECT_STATE = 0x04  // Connects two FSM states with a delay transition
};

#endif // VM_OPCODES_H