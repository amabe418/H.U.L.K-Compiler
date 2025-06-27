#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "./Parser/parser.cpp"
#include "./Parser/AstBuilderVisitor.cpp"
#include "./Lexer/Lexer.cpp"
#include "./Semantic/SemanticAnalizer.cpp"
#include "./include/CodeGen/CodeGenerationContext.hpp"

int main() {

// 

    std::string inputFile = "script.hulk";  // Usa uno fijo o por argv
    std::ifstream file(inputFile);
    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el archivo: " << inputFile << std::endl;
        return 1;
    }
    std::ostringstream buffer;
    buffer <<file.rdbuf();
    std::string input=buffer.str();
    
  
    ErrorHandler error;
    Lexer lexer(error);
    auto tokens=lexer.scanTokens(input);
    if(lexer.errorHandler.hasErrors())
    {
        lexer.errorHandler.printErrors();
        return 0;
    }


    auto grs=Grammar::loadGrammar("grammar.txt");
    int x=3;
     Parser parser(error);
     auto cst= parser.parse(tokens);
    if(cst==nullptr || parser.errorHandler.hasErrors())
    {
        parser.errorHandler.printErrors();
        return 0;
    }

    AstBuilderVisitor collector;
    AstNode* ast=cst->accept(collector);

    ast->print();

    SemanticAnalizer semantic;
    semantic.errorHandler=error;

    semantic.check(ast);
    if(semantic.errorHandler.hasErrors())
    {
      return 0;
    };

    std::filesystem::create_directory("hulk");

    CodeGenerationContext codegen;

    codegen.generateIR(ast);

    codegen.dumpIR();
    return 0;

}
