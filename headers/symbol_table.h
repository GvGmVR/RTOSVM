#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "ast.h"
#include "functions.h"
#include <string>
#include <unordered_map>
#include <iostream>

enum class SymbolType {
    NODE,
    TASK,
    STATE
};

// Base Symbol Class
class Symbol {
public:
    std::string name;
    SymbolType type;
    virtual ~Symbol() = default;
    Symbol(std::string n, SymbolType t) : name(std::move(n)), type(t) {}
};

// Hardware Node Symbol
class NodeSymbol : public Symbol {
public:
    std::string deviceType;
    int pin = -1;
    std::string initialState = "OFF";

    NodeSymbol(std::string n, std::string devType)
        : Symbol(std::move(n), SymbolType::NODE), deviceType(std::move(devType)) {}
};

// RTOS Task Symbol
class TaskSymbol : public Symbol {
public:
    int priority = 1;
    int stackSize = 512;

    explicit TaskSymbol(std::string n)
        : Symbol(std::move(n), SymbolType::TASK) {}
};

// FSM State Symbol
class StateSymbol : public Symbol {
public:
    std::string taskName;
    std::string actionTarget;
    std::string actionValue;

    StateSymbol(std::string n, std::string tName, std::string target, std::string val)
        : Symbol(std::move(n), SymbolType::STATE),
          taskName(std::move(tName)),
          actionTarget(std::move(target)),
          actionValue(std::move(val)) {}
};

// Central Symbol Table Dictionary
class SymbolTable {
private:
    std::unordered_map<std::string, Symbol*> symbols;

public:
    ~SymbolTable();
    bool define(Symbol* sym);
    Symbol* resolve(const std::string& name);
    bool contains(const std::string& name);
    void print() const;
    const std::unordered_map<std::string, Symbol*>& getSymbols() const { return symbols; }
};

// Stage 3 Semantic Analyzer
class SemanticAnalyzer {
private:
    SymbolTable& symTable;

public:
    explicit SemanticAnalyzer(SymbolTable& table) : symTable(table) {}
    bool analyze(ASTNode* root);
};

#endif // SYMBOL_TABLE_H