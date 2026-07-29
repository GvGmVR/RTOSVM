#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <iostream>

enum class ASTNodeType {
    PROGRAM,
    NODE_DEF,
    TASK_DEF,
    STATE_DEF,
    CONNECT_DEF,
    ATTRIBUTE
};

// Base AST Class
class ASTNode {
public:
    ASTNodeType nodeType;
    std::vector<ASTNode*> children;

    explicit ASTNode(ASTNodeType type) : nodeType(type) {}
    virtual ~ASTNode() {
        for (auto child : children) {
            delete child;
        }
    }

    virtual std::string toString(int indent = 0) const = 0;

protected:
    std::string getIndent(int indent) const {
        return std::string(indent * 2, ' ');
    }
};

// 1. Key-Value Attribute Node
class AttributeNode : public ASTNode {
public:
    std::string key;
    std::string value;

    AttributeNode(std::string k, std::string v) 
        : ASTNode(ASTNodeType::ATTRIBUTE), key(std::move(k)), value(std::move(v)) {}

    std::string toString(int indent = 0) const override {
        return getIndent(indent) + "ATTR: " + key + " = " + value + "\n";
    }
};

// 2. Hardware Node Definition AST Node
class NodeDefNode : public ASTNode {
public:
    std::string name;
    std::string deviceType;

    NodeDefNode(std::string n, std::string type)
        : ASTNode(ASTNodeType::NODE_DEF), name(std::move(n)), deviceType(std::move(type)) {}

    std::string toString(int indent = 0) const override {
        std::string res = getIndent(indent) + "CREATE_NODE [" + name + "] AS " + deviceType + "\n";
        for (auto child : children) {
            res += child->toString(indent + 1);
        }
        return res;
    }
};

// 3. RTOS Task Definition AST Node
class TaskDefNode : public ASTNode {
public:
    std::string name;

    explicit TaskDefNode(std::string n)
        : ASTNode(ASTNodeType::TASK_DEF), name(std::move(n)) {}

    std::string toString(int indent = 0) const override {
        std::string res = getIndent(indent) + "CREATE_TASK [" + name + "]\n";
        for (auto child : children) {
            res += child->toString(indent + 1);
        }
        return res;
    }
};

// 4. FSM State Definition AST Node
class StateDefNode : public ASTNode {
public:
    std::string stateName;
    std::string taskName;
    std::string actionTarget;
    std::string actionValue;

    StateDefNode(std::string sName, std::string tName, std::string target, std::string val)
        : ASTNode(ASTNodeType::STATE_DEF), stateName(std::move(sName)), 
          taskName(std::move(tName)), actionTarget(std::move(target)), actionValue(std::move(val)) {}

    std::string toString(int indent = 0) const override {
        return getIndent(indent) + "CREATE_STATE [" + stateName + "] IN TASK [" + taskName + 
               "] (ACTION: SET " + actionTarget + " = " + actionValue + ")\n";
    }
};

// 5. FSM Connection / Transition AST Node
class ConnectDefNode : public ASTNode {
public:
    std::string fromState;
    std::string toState;
    int delayMs;

    ConnectDefNode(std::string from, std::string to, int delay)
        : ASTNode(ASTNodeType::CONNECT_DEF), fromState(std::move(from)), 
          toState(std::move(to)), delayMs(delay) {}

    std::string toString(int indent = 0) const override {
        return getIndent(indent) + "CONNECT [" + fromState + "] -> [" + toState + 
               "] AFTER " + std::to_string(delayMs) + "ms\n";
    }
};

// 6. Root Program Node
class ProgramNode : public ASTNode {
public:
    ProgramNode() : ASTNode(ASTNodeType::PROGRAM) {}

    std::string toString(int indent = 0) const override {
        std::string res = getIndent(indent) + "=== AST ROOT (PROGRAM) ===\n";
        for (auto child : children) {
            res += child->toString(indent + 1);
        }
        return res;
    }
};

#endif // AST_H