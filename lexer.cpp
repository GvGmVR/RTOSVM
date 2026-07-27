//lexer.cpp
#include "functions.h"
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <cctype>

// Helper to print token names
std::string Token::toString() const{
    return "[Line"+ std::to_string(line) + " Column" + std::to_string(column) + "] Lexeme:" + lexeme;
}

static const std::unordered_map<std::string, TokenType> keywords = {
    {"CREATE", TokenType::CREATE},
    {"NODE", TokenType::NODE},
    {"TASK", TokenType::TASK},
    {"AS", TokenType::AS},
    {"CONNECT", TokenType::CONNECT},
    {"TO", TokenType::TO},
    {"ON", TokenType::ON},
    {"AFTER", TokenType::AFTER},
    {"ACTION", TokenType::ACTION},
    {"SET", TokenType::SET}
};

std::vector<Token> lexer(const std::filesystem::path& filepath){
    std::vector<Token> tokens;

    //Read dsl file
    std::ifstream file(filepath);
    if(!file.is_open()){
        std::cerr << "[Lexer Error]: Could not open file " << filepath << "\n";
        return tokens;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string src = buffer.str();

    //scan through code
    size_t cursor = 0;
    int line=1;
    int column=1;
    size_t length = src.length();

    //Scanner loop
    while(cursor < length){
        char c = src[cursor];

        // Remove whitespace and track line/column numbers
        if(std::isspace(c)){
            if(c=='\n'){
                line++;
                column=1;
            }else{
                column++;
            }
            cursor++;
            continue;
        }

        // Handle comments
        // Single line comments start with #
        if(c=='#'){
            while(cursor<length && src[cursor]!='\n'){
                cursor++;
            }
            continue;
        }

        // Handle characters and symbols
        if(c== '='){
            tokens.push_back({TokenType::EQUALS, "=",line,column});
            cursor++;
            column++;
            continue;
        }
        if(c== ','){
            tokens.push_back({TokenType::COMMA, ",",line,column});
            cursor++;
            column++;
            continue;
        }
        if(c== '('){
            tokens.push_back({TokenType::LPAREN, "(",line,column});
            cursor++;
            column++;
            continue;
        }
        if(c== ')'){
            tokens.push_back({TokenType::RPAREN, ")",line,column});
            cursor++;
            column++;
            continue;
        }
        if(c== ';'){
            tokens.push_back({TokenType::SEMICOLON, ";",line,column});
            cursor++;
            column++;
            continue;
        }

        // Mark identifiers and keywords
        if(std::isalpha(c)|| c=='_'){
            int startcol = column;
            std::string word ="";
            while(cursor<length && (std::isalnum(src[cursor])|| src[cursor]=='_')){
                word+=src[cursor];
                cursor++;
                column++;
            }
            auto it = keywords.find(word);
            if (it != keywords.end()){
                tokens.push_back({it->second, word, line, startcol});
            }else{
                tokens.push_back({TokenType::INDENTIFIER, word, line, startcol});
            }
            continue;
        }

        // Handle numbers
        if(std::isdigit(c)){
            int startcol = column;
            std::string numStr="";
            while(cursor<length && std::isdigit(src[cursor])){
                numStr+=src[cursor];
                cursor++;
                column++;
            }
            tokens.push_back({TokenType::NUMBER, numStr, line, startcol});
            continue;
        }

        // Handle unknown characters
        std::cerr << "[Lexer Error]: Unexpected characer '"<< c<< "'at line " << line<< ",col"<<column << "\n";
        tokens.push_back({TokenType::UNKNOWN, std::string(1,c), line, column});
        cursor++;
        column++;
    }

    //Append EOF control token at end
    tokens.push_back({TokenType::TOK_EOF, "", line, column});
    return tokens;
}