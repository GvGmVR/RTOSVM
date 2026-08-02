#include "headers/vm_engine.h"
#include <fstream>
#include <iostream>

VMEngine::~VMEngine() {
    for (auto& pair : nodes)  delete pair.second;
    for (auto& pair : tasks)  delete pair.second;
    for (auto& pair : states) delete pair.second;
}

uint8_t VMEngine::readByte() {
    if (ip >= code.size()) return OP_HALT;
    return code[ip++];
}

int32_t VMEngine::readInt32() {
    uint8_t b0 = readByte();
    uint8_t b1 = readByte();
    uint8_t b2 = readByte();
    uint8_t b3 = readByte();
    return (static_cast<int32_t>(b0) << 24) | 
           (static_cast<int32_t>(b1) << 16) | 
           (static_cast<int32_t>(b2) << 8)  | 
            static_cast<int32_t>(b3);
}

std::string VMEngine::readString() {
    int32_t len = readInt32();
    std::string str;
    str.reserve(len);
    for (int32_t i = 0; i < len; ++i) {
        str.push_back(static_cast<char>(readByte()));
    }
    return str;
}

bool VMEngine::loadBytecodeFile(const std::string& filename) {
    std::ifstream inFile(filename, std::ios::binary | std::ios::ate);
    if (!inFile.is_open()) {
        std::cerr << "[VM Error]: Could not open bytecode file: " << filename << "\n";
        return false;
    }

    std::streamsize size = inFile.tellg();
    inFile.seekg(0, std::ios::beg);

    code.resize(size);
    if (!inFile.read(reinterpret_cast<char*>(code.data()), size)) {
        std::cerr << "[VM Error]: Failed to read bytecode contents.\n";
        return false;
    }

    ip = 0;
    std::cout << "[VM Loader]: Loaded " << code.size() << " bytes from '" << filename << "'\n";
    return true;
}

// Phase 1: Instantiate Polymorphic Actions & Triggers during Setup
void VMEngine::decodeAndSetup() {
    std::cout << "\n--- [VM PHASE 1: POLYMORPHIC SETUP & GRAPH WIRING] ---\n";
    ip = 0;

    while (ip < code.size()) {
        uint8_t opcode = readByte();

        switch (opcode) {
            case OP_DEFINE_NODE: {
                std::string name = readString();
                std::string devType = readString();
                int pin = readInt32();
                uint8_t initState = readByte();

                auto node = new VMNode(name, devType, pin, initState);
                nodes[name] = node;

                hal_pinMode(pin, HAL_PIN_OUTPUT);
                hal_digitalWrite(pin, initState ? HAL_PIN_HIGH : HAL_PIN_LOW);
                break;
            }
            case OP_DEFINE_TASK: {
                std::string name = readString();
                int prio = readInt32();
                int stack = readInt32();

                tasks[name] = new VMTask(name, prio, stack);
                hal_log("Registered Task [" + name + "] (Priority=" + std::to_string(prio) + ")");
                break;
            }
            case OP_DEFINE_STATE: {
                std::string sName = readString();
                std::string tName = readString();
                std::string targetNode = readString();
                uint8_t val = readByte();

                auto state = new VMState(sName, tName);

                // POLYMORPHIC ACTION INSTANTIATION:
                // Look up the target node to get its pin number
                VMNode* node = nodes[targetNode];
                if (node) {
                    // Create a polymorphic SetGpioAction and attach to state!
                    state->actions.push_back(new SetGpioAction(node->pin, val));
                }

                states[sName] = state;

                if (!currentState) {
                    currentState = state;
                }
                hal_log("Registered State [" + sName + "] in Task [" + tName + "]");
                break;
            }
            case OP_CONNECT_STATE: {
                std::string fromName = readString();
                std::string toName = readString();
                int delayMs = readInt32();

                VMState* fromState = states[fromName];
                VMState* toState = states[toName];

                if (fromState && toState) {
                    // POLYMORPHIC TRIGGER INSTANTIATION:
                    VMTrigger* trigger = nullptr;
                    if (delayMs > 0) {
                        trigger = new TimerTrigger(delayMs);
                    } else {
                        trigger = new ImmediateTrigger();
                    }

                    // Create polymorphic transition edge and attach to fromState!
                    fromState->transitions.push_back(new VMTransition(trigger, toState));
                    hal_log("Wired FSM Edge: [" + fromName + "] -> [" + toName + "] (" + std::to_string(delayMs) + "ms)");
                }
                break;
            }
            case OP_HALT:
                std::cout << "[VM Loader]: Setup complete. Polymorphic Graph constructed.\n\n";
                return;
            default:
                std::cerr << "[VM Error]: Unknown opcode '0x" << std::hex << (int)opcode << "'\n";
                return;
        }
    }
}

// Phase 2: 100% Generic & Polymorphic FSM Execution Engine
void VMEngine::runFSM(int maxTransitions) {
    if (!currentState) {
        std::cerr << "[VM Error]: No initial state found to run FSM!\n";
        return;
    }

    std::cout << "--- [VM PHASE 2: POLYMORPHIC FSM EXECUTION] ---\n";
    hal_log("Starting FSM execution at State [" + currentState->stateName + "]...");

    int transitionCount = 0;

    while (currentState != nullptr) {
        hal_log("Entering State [" + currentState->stateName + "]");

        // 1. EXECUTE ALL POLYMORPHIC ACTIONS FOR THIS STATE
        for (VMAction* action : currentState->actions) {
            action->execute(); // Polymorphic call!
        }

        // 2. EVALUATE POLYMORPHIC TRANSITION TRIGGERS
        uint32_t stateStartTime = hal_getSystemTimeMs();
        VMState* nextState = nullptr;

        while (nextState == nullptr) {
            uint32_t elapsedTime = hal_getSystemTimeMs() - stateStartTime;

            // Check each outgoing transition edge
            for (VMTransition* trans : currentState->transitions) {
                if (trans->trigger->isSatisfied(elapsedTime)) { // Polymorphic call!
                    nextState = trans->targetState;
                    break;
                }
            }

            if (nextState == nullptr) {
                hal_yieldTask(); // Yield CPU to RTOS / System scheduler
            }
        }

        // 3. JUMP TO NEXT STATE IN GRAPH
        currentState = nextState;

        transitionCount++;
        if (maxTransitions > 0 && transitionCount >= maxTransitions) {
            hal_log("Reached max transition limit (" + std::to_string(maxTransitions) + "). Stopping demo.");
            break;
        }
    }
}