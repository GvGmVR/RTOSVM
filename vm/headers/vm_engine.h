#ifndef VM_ENGINE_H
#define VM_ENGINE_H

#include "vm_opcodes.h"
#include "vm_types.h"
#include "vm_hal.h"

#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>

class VMEngine {
private:
    std::vector<uint8_t> code;
    size_t ip = 0;

    std::unordered_map<std::string, VMNode*> nodes;
    std::unordered_map<std::string, VMTask*> tasks;
    std::unordered_map<std::string, VMState*> states;

    VMState* currentState = nullptr;

    uint8_t readByte();
    int32_t readInt32();
    std::string readString();

public:
    VMEngine() = default;
    ~VMEngine();

    bool loadBytecodeFile(const std::string& filename);
    void decodeAndSetup();
    void runFSM(int maxTransitions = -1);
};

#endif // VM_ENGINE_H