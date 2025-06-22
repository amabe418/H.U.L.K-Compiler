#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

#include "AST/ast.hpp"
#include "Evaluator/evaluator.hpp"
#include "PrintVisitor/print_visitor.hpp"
#include "SymbolTable/symbol_table.hpp"
#include "Value/value.hpp"
#include "SemanticCheck/semantic_checker.hpp"
#include "Codegen/codegen.hpp"

extern FILE *yyin;
extern int yyparse();
extern int yylineno;
extern int yydebug;
extern Program *rootAST;
SymbolTable symbolTable;

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    if (!input)
    {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    yylineno = 1;
    yyin = input;
    // extern int yydebug;
    // yydebug = 1;

    std::cout << "\n=== Token Analysis ===\n"
              << std::endl;

    if (yyparse() != 0)
    {
        std::cerr << "Error: Parsing failed" << std::endl;
        fclose(input);
        return 1;
    }
    fclose(input);

    std::cout << "\n=== End of Token Analysis ===\n"
              << std::endl;

    if (!rootAST)
    {
        std::cerr << "Error: No program to evaluate" << std::endl;
        return 1;
    }

    // Realizar análisis semántico
    std::cout << "\n=== Semantic Analysis ===\n"
              << std::endl;
    SemanticAnalyzer semanticAnalyzer;
    semanticAnalyzer.analyze(rootAST);

    // Verificar si hay errores semánticos
    if (semanticAnalyzer.hasErrors())
    {
        std::cerr << "\nSemantic errors found:" << std::endl;
        semanticAnalyzer.printErrors();
        return 1;
    }
    std::cout << "\n=== End of Semantic Analysis ===\n"
              << std::endl;

    std::cout << "\n=== Print AST ===\n"
              << std::endl;
    PrintVisitor printer;
    rootAST->accept(&printer);

    // Generate LLVM IR code
    std::cout << "\n=== LLVM Code Generation ===\n"
              << std::endl;
    CodeGenerator codegen;
    codegen.initialize("hulk_module");
    codegen.generateCode(rootAST);

    // Print generated IR
    // std::cout << "\nGenerated LLVM IR:" << std::endl;
    // codegen.printIR();

    // Write IR to file
    std::string output_file = std::string(argv[1]) + ".ll";
    codegen.writeIRToFile(output_file);
    // std::cout << "\nLLVM IR written to: " << output_file << std::endl;

    // std::cout << "\n=== End of LLVM Code Generation ===\n"
    //           << std::endl;

    // Evaluate the program
    std::cout << "\n=== Program Evaluation ===\n"
              << std::endl;
    EvaluatorVisitor evaluator;
    rootAST->accept(&evaluator);
    std::cout << "\n=== End of Program Evaluation ===\n"
              << std::endl;

    return 0;
}
