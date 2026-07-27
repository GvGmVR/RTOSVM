//byteCoder.cpp
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <functions.h>

namespace fs = std::filesystem;

int main(){
    fs::path filePath = "blink.dsl";

    if (filePath.extension()== ".dsl"){
        std::cout << "[Read File]: dsl file read";
    }else{
        std::cout << "[Read File]: dsl file not found or read";
        return 1;
    }

    lexer(filePath);
}
