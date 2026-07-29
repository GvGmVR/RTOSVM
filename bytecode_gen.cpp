#include "headers/bytecode_gen.h"
#include <fstream>
#include <iostream>
#include <iomanip>

void BytecodeGenerator::emitByte(uint8_t byte) {
    bytecode.push_back(byte);
}

void BytecodeGenerator::emitInt32(int32_t val) {
    emitByte((val >> 24) & 0xFF);
    emitByte((val >> 16) & 0xFF);
    emitByte((val >> 8) & 0xFF);
    emitByte(val & 0xFF);
}

void BytecodeGenerator::emitString(const std::string& str) {
    emitInt32(static_cast<int32_t>(str.length()));
    for (char c : str) {
        emitByte(static_cast<uint8_t>(c));
    }
}

std::vector<uint8_t> BytecodeGenerator::generate(ASTNode* root) {
    bytecode.clear();

    if (!root) return bytecode;

    // Walk AST nodes and emit bytecodes
    for (ASTNode* child : root->children) {
        if (child->nodeType == ASTNodeType::NODE_DEF) {
            auto nd = static_cast<NodeDefNode*>(child);
            auto sym = static_cast<NodeSymbol*>(symTable.resolve(nd->name));

            emitByte(OP_DEFINE_NODE);
            emitString(nd->name);
            emitString(nd->deviceType);
            emitInt32(sym ? sym->pin : -1);
            emitByte(sym && sym->initialState == "ON" ? 1 : 0);
        }
        else if (child->nodeType == ASTNodeType::TASK_DEF) {
            auto td = static_cast<TaskDefNode*>(child);
            auto sym = static_cast<TaskSymbol*>(symTable.resolve(td->name));

            emitByte(OP_DEFINE_TASK);
            emitString(td->name);
            emitInt32(sym ? sym->priority : 1);
            emitInt32(sym ? sym->stackSize : 512);
        }
        else if (child->nodeType == ASTNodeType::STATE_DEF) {
            auto sd = static_cast<StateDefNode*>(child);

            emitByte(OP_DEFINE_STATE);
            emitString(sd->stateName);
            emitString(sd->taskName);
            emitString(sd->actionTarget);
            emitByte(sd->actionValue == "ON" ? 1 : 0);
        }
        else if (child->nodeType == ASTNodeType::CONNECT_DEF) {
            auto cd = static_cast<ConnectDefNode*>(child);

            emitByte(OP_CONNECT_STATE);
            emitString(cd->fromState);
            emitString(cd->toState);
            emitInt32(cd->delayMs);
        }
    }

    emitByte(OP_HALT);
    return bytecode;
}

bool BytecodeGenerator::saveToFile(const std::string& filename) const {
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile.is_open()) return false;

    outFile.write(reinterpret_cast<const char*>(bytecode.data()), bytecode.size());
    outFile.close();
    return true;
}

void BytecodeGenerator::printDisassembly() const {
    std::cout << "=== BYTECODE DISASSEMBLY (" << bytecode.size() << " bytes) ===\n";
    size_t ip = 0;

    auto readInt32 = [&](size_t& offset) -> int32_t {
        int32_t val = (bytecode[offset] << 24) | (bytecode[offset+1] << 16) |
                      (bytecode[offset+2] << 8)  | bytecode[offset+3];
        offset += 4;
        return val;
    };

    auto readString = [&](size_t& offset) -> std::string {
        int32_t len = readInt32(offset);
        std::string s(reinterpret_cast<const char*>(&bytecode[offset]), len);
        offset += len;
        return s;
    };

    while (ip < bytecode.size()) {
        std::cout << "0x" << std::hex << std::setw(4) << std::setfill('0') << ip << std::dec << ": ";
        uint8_t op = bytecode[ip++];

        switch (op) {
            case OP_DEFINE_NODE: {
                std::string name = readString(ip);
                std::string dev = readString(ip);
                int pin = readInt32(ip);
                uint8_t init = bytecode[ip++];
                std::cout << "OP_DEFINE_NODE name='" << name << "', type='" << dev 
                          << "', pin=" << pin << ", init=" << (init ? "ON" : "OFF") << "\n";
                break;
            }
            case OP_DEFINE_TASK: {
                std::string name = readString(ip);
                int prio = readInt32(ip);
                int stack = readInt32(ip);
                std::cout << "OP_DEFINE_TASK name='" << name << "', prio=" << prio 
                          << ", stack=" << stack << "\n";
                break;
            }
            case OP_DEFINE_STATE: {
                std::string sName = readString(ip);
                std::string tName = readString(ip);
                std::string target = readString(ip);
                uint8_t val = bytecode[ip++];
                std::cout << "OP_DEFINE_STATE state='" << sName << "', task='" << tName 
                          << "', action=(SET " << target << " = " << (val ? "ON" : "OFF") << ")\n";
                break;
            }
            case OP_CONNECT_STATE: {
                std::string from = readString(ip);
                std::string to = readString(ip);
                int delay = readInt32(ip);
                std::cout << "OP_CONNECT_STATE from='" << from << "' -> to='" << to 
                          << "' delay=" << delay << "ms\n";
                break;
            }
            case OP_HALT:
                std::cout << "OP_HALT\n";
                break;
            default:
                std::cout << "UNKNOWN_OPCODE (0x" << std::hex << (int)op << ")\n";
                return;
        }
    }
    std::cout << "===========================================\n\n";
}