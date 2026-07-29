#ifndef BYTECODE_GEN_H
#define BYTECODE_GEN_H

#include "ast.h"
#include "symbol_table.h"
#include <vector>
#include <string>
#include <cstdint>

// RTOS Bytecode Opcodes
enum Opcode : uint8_t {
    OP_HALT          = 0x00,
    OP_DEFINE_NODE   = 0x01,
    OP_DEFINE_TASK   = 0x02,
    OP_DEFINE_STATE  = 0x03,
    OP_CONNECT_STATE = 0x04
};

class BytecodeGenerator {
private:
    SymbolTable& symTable;
    std::vector<uint8_t> bytecode;

    void emitByte(uint8_t byte);
    void emitInt32(int32_t val);
    void emitString(const std::string& str);

public:
    explicit BytecodeGenerator(SymbolTable& table) : symTable(table) {}
    std::vector<uint8_t> generate(ASTNode* root);
    bool saveToFile(const std::string& filename) const;
    void printDisassembly() const;
};

#endif // BYTECODE_GEN_H