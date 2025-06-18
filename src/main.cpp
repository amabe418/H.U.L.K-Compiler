#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

#include "AST/ast.hpp"
#include "Evaluator/evaluator.hpp"
#include "PrintVisitor/print_visitor.hpp"
#include "PrintVisitor/typed_print_visitor.hpp"
#include "SymbolCollector/symbol_collector.hpp"
#include "Symbols/symbol_table.hpp"
#include "Types/type_checker.hpp"
#include "Value/value.hpp"

extern FILE* yyin;
extern int yyparse();
extern int yylineno;
extern int yydebug;
extern Program* rootAST;
SymbolTable symbolTable;

int
main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    FILE* input = fopen(argv[1], "r");
    if (!input)
    {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    yylineno = 1;
    yyin = input;
    // extern int yydebug;
    // yydebug = 1;

    std::cout << "\n=== Token Analysis ===\n" << std::endl;

    if (yyparse() != 0)
    {
        std::cerr << "Error: Parsing failed" << std::endl;
        fclose(input);
        return 1;
    }
    fclose(input);

    std::cout << "\n=== End of Token Analysis ===\n" << std::endl;

    if (!rootAST)
    {
        std::cerr << "Error: No program to evaluate" << std::endl;
        return 1;
    }

    PrintVisitor printerr;
    rootAST->accept(&printerr);

    // Create symbol table and collector
    SymbolCollector symbolCollector(symbolTable);

    // Collect symbols
    rootAST->accept(&symbolCollector);

    // Print symbol table contents
    // symbolCollector.printSymbolTable();

    // Create type checker
    TypeChecker typeChecker;

    // Type check the program
    if (!typeChecker.check(rootAST))
    {
        std::cerr << "Error: Type checking failed" << std::endl;
        return 1;
    }
    std::cout << "Type checking passed" << std::endl;

    // Print the typed AST
    TypedAstPrinter printer(typeChecker);
    rootAST->accept(&printer);

    // Evaluate the program
    EvaluatorVisitor evaluator;
    rootAST->accept(&evaluator);

    return 0;
}
