//functions.h
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include <vector>
#include <filesystem>
#include <iostream>

// Token types

enum class TokenType {
    //Keywords
    CREATE,
    NODE,
    TASK,
    AS,
    CONNECT,
    TO,
    ON,
    AFTER,
    // Literals and numbers
    INDENTIFIER,
    NUMBER,
    STRING,
    //Symbols
    EQUALS,
    COMMA,  //,
    LOAREN, //(
    RPAREN, //)
    SEMICOLON, //;
    //Control tokens
    TOK_EOF,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme; //Text
    int line;
    int column;

    //Helper method for token to string conversion(debug)
    std::string toString() const;
};

std::vector<Token> lexer(const std::filesystem::path& filePath);


#endif