//byteCoder.cpp
#include <iostream>
#include <filesystem>
#include "headers/functions.h"
#include "headers/ast.h"
#include "headers/parser.h"
#include "headers/symbol_table.h"
#include "headers/bytecode_gen.h"

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

    // Lexer
    std::vector<Token> tokens = lexer(filePath);

    std::cout << "[Lexer]: Tokens generated:"<< std::endl;
    for(const auto& token: tokens){
        std::cout << token.toString() << std::endl;
    }

    // Parser
    Parser parser(tokens);
    ASTNode* rawAST = parser.parse();

    // Print the generated AST!
    std::cout << rawAST->toString() << "\n";

    // Symbol Table
    SymbolTable symTable;
    SemanticAnalyzer analyzer(symTable);
    bool isValid = analyzer.analyze(rawAST);
    
    symTable.print();

    if (!isValid) {
        std::cerr << "[Compiler Error]: Semantic analysis failed. Stopping compilation.\n";
        delete rawAST;
        return 1;
    }

    // Generate ByteCode
    BytecodeGenerator generator(symTable);
    std::vector<uint8_t> bytecodes = generator.generate(rawAST);
    
    generator.printDisassembly();

    if (generator.saveToFile("blink.bc")) {
        std::cout << "[Compiler Success]: Bytecode saved to 'blink.bc'\n";
    } else {
        std::cerr << "[Compiler Error]: Could not write 'blink.bc'\n";
    }
    delete rawAST; // Clean up memory

    return 0;
}
