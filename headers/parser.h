#ifndef PARSER_H
#define PARSER_H

#include "functions.h"
#include "ast.h"
#include <vector>

class Parser {
private:
    std::vector<Token> tokens;
    size_t p = 0; // Token pointer index

    // LL(k) Flashlight Helper Methods
    Token LT(int i);              // Lookahead Token object at offset i
    TokenType LA(int i);          // Lookahead TokenType at offset i
    void consume();               // Advance pointer p
    Token match(TokenType type);  // Verify token type & consume

    // Helper to accept identifiers or general keyword words as names
    Token matchIdentifierLike();

    // Private Grammar Parsing Rules
    ASTNode* parseStatement();
    NodeDefNode* parseCreateNode();
    TaskDefNode* parseCreateTask();
    StateDefNode* parseCreateState();
    ConnectDefNode* parseConnect();
    std::vector<AttributeNode*> parseAttributeList();

public:
    explicit Parser(std::vector<Token> tokStream);
    ASTNode* parse(); // Main entry point
};

#endif // PARSER_H