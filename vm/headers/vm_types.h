#ifndef VM_TYPES_H
#define VM_TYPES_H

#include "vm_hal.h"
#include <string>
#include <vector>
#include <cstdint>
#include <utility>

// ============================================================================
// 1. POLYMORPHIC ACTIONS (Behavior Strategy Pattern)
// ============================================================================

// Abstract Base Class for ANY action in your DSL
class VMAction {
public:
    virtual ~VMAction() = default;
    virtual void execute() = 0; // Each subclass implements its own behavior!
};

// Polymorphic Subclass 1: Set GPIO Pin State
class SetGpioAction : public VMAction {
private:
    int pin;
    uint8_t state;
public:
    SetGpioAction(int p, uint8_t s) : pin(p), state(s) {}
    void execute() override {
        hal_digitalWrite(pin, state ? HAL_PIN_HIGH : HAL_PIN_LOW);
    }
};

// ============================================================================
// 2. POLYMORPHIC TRIGGERS & TRANSITIONS (State Graph Strategy Pattern)
// ============================================================================

// Abstract Base Class for ANY transition condition in your DSL
class VMTrigger {
public:
    virtual ~VMTrigger() = default;
    virtual bool isSatisfied(uint32_t timeInStateMs) = 0;
};

// Polymorphic Trigger 1: Timed Delay Trigger (AFTER 500ms)
class TimerTrigger : public VMTrigger {
private:
    uint32_t targetDelayMs;
public:
    explicit TimerTrigger(uint32_t delay) : targetDelayMs(delay) {}
    bool isSatisfied(uint32_t timeInStateMs) override {
        return timeInStateMs >= targetDelayMs;
    }
};

// Polymorphic Trigger 2: Immediate Trigger (0ms delay / unconditional)
class ImmediateTrigger : public VMTrigger {
public:
    bool isSatisfied(uint32_t /*timeInStateMs*/) override {
        return true;
    }
};

// Forward declaration of VMState for graph edge pointers
struct VMState;

// Polymorphic Graph Edge (Connects a Trigger to a Target State)
struct VMTransition {
    VMTrigger* trigger;   // Polymorphic condition (Timer, Pin, Queue, etc.)
    VMState* targetState; // Pointer to destination VMState in RAM

    VMTransition(VMTrigger* trig, VMState* target)
        : trigger(trig), targetState(target) {}

    ~VMTransition() {
        delete trigger;
    }
};

// ============================================================================
// 3. HARDWARE NODES, TASKS & POLYMORPHIC FSM STATES
// ============================================================================

// Hardware Node (Pin Binding)
struct VMNode {
    std::string name;
    std::string deviceType;
    int pin;
    uint8_t currentState;

    VMNode() : pin(-1), currentState(0) {}
    VMNode(std::string n, std::string dev, int p, uint8_t initState)
        : name(std::move(n)), deviceType(std::move(dev)), pin(p), currentState(initState) {}
};

// RTOS Task Configuration
struct VMTask {
    std::string name;
    int priority;
    int stackSize;

    VMTask() : priority(1), stackSize(512) {}
    VMTask(std::string n, int prio, int stack)
        : name(std::move(n)), priority(prio), stackSize(stack) {}
};

// FSM State (Holds vectors of Polymorphic Actions & Polymorphic Transitions)
struct VMState {
    std::string stateName;
    std::string taskName;

    // Polymorphic Action List (Executes upon state entry)
    std::vector<VMAction*> actions;

    // Polymorphic Transition Edge List (Evaluates conditions to jump to next state)
    std::vector<VMTransition*> transitions;

    VMState(std::string sName, std::string tName)
        : stateName(std::move(sName)), taskName(std::move(tName)) {}

    ~VMState() {
        for (auto action : actions) delete action;
        for (auto trans : transitions) delete trans;
    }
};

#endif // VM_TYPES_H