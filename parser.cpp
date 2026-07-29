#include "headers/parser.h"
#include <stdexcept>

Parser::Parser(std::vector<Token> tokStream) : tokens(std::move(tokStream)) {}

// --- LL(k) Flashlight Methods ---
Token Parser::LT(int i) {
    if (p + i - 1 >= tokens.size()) {
        return tokens.back(); // Returns EOF token
    }
    return tokens[p + i - 1];
}

TokenType Parser::LA(int i) {
    return LT(i).type;
}

void Parser::consume() {
    p++;
}

Token Parser::match(TokenType type) {
    if (LA(1) == type) {
        Token tok = LT(1);
        consume();
        return tok;
    }
    throw std::runtime_error("[Parser Error Line " + std::to_string(LT(1).line) + 
                             "]: Expected token type " + std::to_string(static_cast<int>(type)) + 
                             " but found '" + LT(1).lexeme + "'");
}

Token Parser::matchIdentifierLike() {
    TokenType t = LA(1);
    if (t == TokenType::IDENTIFIER || t == TokenType::NODE || t == TokenType::TASK || 
        t == TokenType::STATE || t == TokenType::AS || t == TokenType::ON) {
        Token tok = LT(1);
        consume();
        return tok;
    }
    return match(TokenType::IDENTIFIER);
}

// --- Main Parsing Dispatcher ---
ASTNode* Parser::parse() {
    ProgramNode* prog = new ProgramNode();
    while (LA(1) != TokenType::TOK_EOF) {
        ASTNode* stmt = parseStatement();
        if (stmt) {
            prog->children.push_back(stmt);
        }
    }
    return prog;
}

// --- LL(2) Flashlight Decision Point ---
ASTNode* Parser::parseStatement() {
    // Lookahead at 2 tokens: LA(1) and LA(2)
    if (LA(1) == TokenType::CREATE && LA(2) == TokenType::NODE) {
        return parseCreateNode();
    } 
    else if (LA(1) == TokenType::CREATE && LA(2) == TokenType::TASK) {
        return parseCreateTask();
    } 
    else if (LA(1) == TokenType::CREATE && LA(2) == TokenType::STATE) {
        return parseCreateState();
    } 
    else if (LA(1) == TokenType::CONNECT) {
        return parseConnect();
    } 
    else {
        throw std::runtime_error("[Parser Error Line " + std::to_string(LT(1).line) + 
                                 "]: Unexpected statement starting with '" + LT(1).lexeme + "'");
    }
}

// Rule 1: CREATE NODE <name> AS <type> ( <attributes> ) ;
NodeDefNode* Parser::parseCreateNode() {
    match(TokenType::CREATE);
    match(TokenType::NODE);
    
    std::string nodeName = matchIdentifierLike().lexeme;
    match(TokenType::AS);
    std::string deviceType = matchIdentifierLike().lexeme;

    NodeDefNode* nodeDef = new NodeDefNode(nodeName, deviceType);

    match(TokenType::LPAREN);
    std::vector<AttributeNode*> attrs = parseAttributeList();
    match(TokenType::RPAREN);
    match(TokenType::SEMICOLON);

    for (auto attr : attrs) {
        nodeDef->children.push_back(attr);
    }
    return nodeDef;
}

// Rule 2: CREATE TASK <name> ( <attributes> ) ;
TaskDefNode* Parser::parseCreateTask() {
    match(TokenType::CREATE);
    match(TokenType::TASK);

    std::string taskName = matchIdentifierLike().lexeme;
    TaskDefNode* taskDef = new TaskDefNode(taskName);

    match(TokenType::LPAREN);
    std::vector<AttributeNode*> attrs = parseAttributeList();
    match(TokenType::RPAREN);
    match(TokenType::SEMICOLON);

    for (auto attr : attrs) {
        taskDef->children.push_back(attr);
    }
    return taskDef;
}

// Rule 3: CREATE STATE <name> IN TASK <taskName> ( ACTION = SET <target> = <val> ) ;
StateDefNode* Parser::parseCreateState() {
    match(TokenType::CREATE);
    match(TokenType::STATE);

    std::string stateName = matchIdentifierLike().lexeme;
    match(TokenType::IN);
    match(TokenType::TASK);
    std::string taskName = matchIdentifierLike().lexeme;

    match(TokenType::LPAREN);
    match(TokenType::ACTION);
    match(TokenType::EQUALS);
    match(TokenType::SET);

    std::string target = matchIdentifierLike().lexeme;
    match(TokenType::EQUALS);
    
    std::string val;
    if (LA(1) == TokenType::NUMBER) {
        val = match(TokenType::NUMBER).lexeme;
    } else {
        val = matchIdentifierLike().lexeme;
    }

    match(TokenType::RPAREN);
    match(TokenType::SEMICOLON);

    return new StateDefNode(stateName, taskName, target, val);
}

// Rule 4: CONNECT <fromState> TO <toState> AFTER <delayMs> ;
ConnectDefNode* Parser::parseConnect() {
    match(TokenType::CONNECT);
    std::string fromState = matchIdentifierLike().lexeme;
    match(TokenType::TO);
    std::string toState = matchIdentifierLike().lexeme;

    match(TokenType::AFTER);
    int delay = std::stoi(match(TokenType::NUMBER).lexeme);
    match(TokenType::SEMICOLON);

    return new ConnectDefNode(fromState, toState, delay);
}

// Attribute Loop: ( KEY = VALUE , KEY = VALUE )
std::vector<AttributeNode*> Parser::parseAttributeList() {
    std::vector<AttributeNode*> attrs;

    while (LA(1) != TokenType::RPAREN && LA(1) != TokenType::TOK_EOF) {
        std::string key = matchIdentifierLike().lexeme;
        match(TokenType::EQUALS);

        std::string value;
        if (LA(1) == TokenType::NUMBER) {
            value = match(TokenType::NUMBER).lexeme;
        } else {
            value = matchIdentifierLike().lexeme;
        }

        attrs.push_back(new AttributeNode(key, value));

        if (LA(1) == TokenType::COMMA) {
            match(TokenType::COMMA);
        } else {
            break;
        }
    }
    return attrs;
}