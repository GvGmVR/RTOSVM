//byteCoder.cpp
#include <iostream>
#include <filesystem>
#include "headers/functions.h"

namespace fs = std::filesystem;

int main(){
    std::cout << "[ByteCoder]: Application started..." << std::endl;
    fs::path filePath = "blink.dsl";

    if (fs::exists(filePath) && filePath.extension()== ".dsl"){
        std::cout << "[Read File]: DSL file read at:"<< filePath << std::endl << std::endl;
    }else{
        std::cerr << "[Read File]: DSL file not found or invalid"<< fs::absolute(filePath) << std::endl;
        return 1;
    }

    std::vector<Token> tokens = lexer(filePath);

    std::cout << "[Lexer]: Tokens generated:"<< std::endl;
    for(const auto& token: tokens){
        std::cout << token.toString() << std::endl;
    }

    return 0;
}
