//byteCoder.cpp
#include <iostream>
#include <filesystem>
#include "functions.h"

namespace fs = std::filesystem;

int main(){
    fs::path filePath = "blink.dsl";

    if (fs::exists(filePath) && filePath.extension()== ".dsl"){
        std::cout << "[Read File]: DSL file read at:"<< filePath << "\n\n";
    }else{
        std::cerr << "[Read File]: DSL file not found or invalid";
        return 1;
    }

    std::vector<Token> tokens = lexer(filePath);

    std::cout << "[Lexer]: Tokens generated:\n";
    for(const auto& token: tokens){
        std::cout << token.toString() << "\n";
    }

    return 0;
}
