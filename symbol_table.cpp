#include "headers/symbol_table.h"

SymbolTable::~SymbolTable() {
    for (auto& pair : symbols) {
        delete pair.second;
    }
}

bool SymbolTable::define(Symbol* sym) {
    if (contains(sym->name)) {
        std::cerr << "[Semantic Error]: Duplicate symbol definition for '" << sym->name << "'\n";
        return false;
    }
    symbols[sym->name] = sym;
    return true;
}

Symbol* SymbolTable::resolve(const std::string& name) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return it->second;
    }
    return nullptr;
}

bool SymbolTable::contains(const std::string& name) {
    return symbols.find(name) != symbols.end();
}

void SymbolTable::print() const {
    std::cout << "\n=== SYMBOL TABLE REGISTRY ===\n";
    for (const auto& pair : symbols) {
        Symbol* s = pair.second;
        if (s->type == SymbolType::NODE) {
            auto ns = static_cast<NodeSymbol*>(s);
            std::cout << "  [NODE] " << ns->name << " (" << ns->deviceType 
                      << ") -> Pin: " << ns->pin << ", Init: " << ns->initialState << "\n";
        } else if (s->type == SymbolType::TASK) {
            auto ts = static_cast<TaskSymbol*>(s);
            std::cout << "  [TASK] " << ts->name << " -> Priority: " 
                      << ts->priority << ", Stack: " << ts->stackSize << "\n";
        } else if (s->type == SymbolType::STATE) {
            auto ss = static_cast<StateSymbol*>(s);
            std::cout << "  [STATE] " << ss->name << " in Task [" << ss->taskName 
                      << "] -> Action: SET " << ss->actionTarget << " = " << ss->actionValue << "\n";
        }
    }
    std::cout << "=============================\n\n";
}

bool SemanticAnalyzer::analyze(ASTNode* root) {
    if (!root) return false;

    bool hasErrors = false;

    // --- PASS 1: Symbol Definition Pass ---
    for (ASTNode* child : root->children) {
        if (child->nodeType == ASTNodeType::NODE_DEF) {
            auto nd = static_cast<NodeDefNode*>(child);
            auto nodeSym = new NodeSymbol(nd->name, nd->deviceType);

            // Extract attributes from child ATTR nodes
            for (ASTNode* attrChild : nd->children) {
                if (attrChild->nodeType == ASTNodeType::ATTRIBUTE) {
                    auto attr = static_cast<AttributeNode*>(attrChild);
                    if (attr->key == "PIN") nodeSym->pin = std::stoi(attr->value);
                    if (attr->key == "INITIAL_STATE") nodeSym->initialState = attr->value;
                }
            }
            if (!symTable.define(nodeSym)) {
                delete nodeSym;
                hasErrors = true;
            }
        } 
        else if (child->nodeType == ASTNodeType::TASK_DEF) {
            auto td = static_cast<TaskDefNode*>(child);
            auto taskSym = new TaskSymbol(td->name);

            for (ASTNode* attrChild : td->children) {
                if (attrChild->nodeType == ASTNodeType::ATTRIBUTE) {
                    auto attr = static_cast<AttributeNode*>(attrChild);
                    if (attr->key == "PRIORITY") taskSym->priority = std::stoi(attr->value);
                    if (attr->key == "STACK_SIZE") taskSym->stackSize = std::stoi(attr->value);
                }
            }
            if (!symTable.define(taskSym)) {
                delete taskSym;
                hasErrors = true;
            }
        }
        else if (child->nodeType == ASTNodeType::STATE_DEF) {
            auto sd = static_cast<StateDefNode*>(child);

            // Check if enclosing Task exists
            Symbol* taskSym = symTable.resolve(sd->taskName);
            if (!taskSym || taskSym->type != SymbolType::TASK) {
                std::cerr << "[Semantic Error]: State '" << sd->stateName 
                          << "' references undefined Task '" << sd->taskName << "'\n";
                hasErrors = true;
            }

            // Check if Action target Node exists
            Symbol* nodeSym = symTable.resolve(sd->actionTarget);
            if (!nodeSym || nodeSym->type != SymbolType::NODE) {
                std::cerr << "[Semantic Error]: State '" << sd->stateName 
                          << "' action references undefined Node '" << sd->actionTarget << "'\n";
                hasErrors = true;
            }

            auto stateSym = new StateSymbol(sd->stateName, sd->taskName, sd->actionTarget, sd->actionValue);
            if (!symTable.define(stateSym)) {
                delete stateSym;
                hasErrors = true;
            }
        }
    }

    // --- PASS 2: Symbol Resolution & Relationship Pass ---
    for (ASTNode* child : root->children) {
        if (child->nodeType == ASTNodeType::CONNECT_DEF) {
            auto cd = static_cast<ConnectDefNode*>(child);

            Symbol* fromSym = symTable.resolve(cd->fromState);
            Symbol* toSym = symTable.resolve(cd->toState);

            if (!fromSym || fromSym->type != SymbolType::STATE) {
                std::cerr << "[Semantic Error]: CONNECT references undefined State '" << cd->fromState << "'\n";
                hasErrors = true;
            }
            if (!toSym || toSym->type != SymbolType::STATE) {
                std::cerr << "[Semantic Error]: CONNECT references undefined State '" << cd->toState << "'\n";
                hasErrors = true;
            }
        }
    }

    return !hasErrors;
}