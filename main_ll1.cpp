#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "theoretical/Lexer/theoretical_lexer.hpp"
#include "Parser/ll1_parser.hpp"
#include "PrintVisitor/print_visitor.hpp"
#include "SemanticCheck/semantic_checker.hpp"
#include "Codegen/codegen.hpp"
#include "Evaluator/evaluator.hpp"

int main(int argc, char *argv[])
{
    // Verificar argumentos de línea de comandos
    if (argc != 2)
    {
        std::cerr << "Uso: " << argv[0] << " <archivo.hulk>" << std::endl;
        return 1;
    }

    // 1. Leer el archivo especificado
    std::ifstream file(argv[1]);
    if (!file)
    {
        std::cerr << "No se pudo abrir " << argv[1] << std::endl;
        return 1;
    }
    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // 2. Analizar léxicamente
    TheoreticalLexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    // 3. Imprimir la lista de tokens (opcional)
    std::cout << "Tokens:\n";
    for (const auto &token : tokens)
    {
        std::cout << getTokenName(token.type) << " ('" << token.lexeme << "') en linea " << token.line << ", columna " << token.column << std::endl;
    }

    // 4. Parsear con el nuevo parser LL(1) que calcula FIRST/FOLLOW/LL1
    std::cout << "\n=== Parsing con LL(1) ===" << std::endl;
    LL1Parser parser("Parser/grammar.ll1", tokens);
    Program *program = nullptr;
    try
    {
        auto [cst, ast] = parser.parse();
        // program = ast;

        // Mostrar información de debugging
        // parser.printProductions();
        // parser.printFirstSets();
        // parser.printFollowSets();
        // parser.printLL1Table();
        // parser.printDerivationTree();
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Error de parseo: " << ex.what() << std::endl;
        return 2;
    }

    // // 5. Análisis semántico
    // if (program)
    // {
    //     std::cout << "\nRealizando análisis semántico...\n";
    //     SemanticAnalyzer semantic_analyzer;
    //     semantic_analyzer.analyze(program);

    //     if (semantic_analyzer.hasErrors())
    //     {
    //         std::cerr << "\nErrores semánticos encontrados:\n";
    //         semantic_analyzer.printErrors();
    //         delete program;
    //         return 3;
    //     }
    //     else
    //     {
    //         std::cout << "Análisis semántico completado sin errores.\n";
    //     }
    // }

    // // 6. Generación de código
    // if (program)
    // {
    //     std::cout << "\nGenerando código LLVM...\n";
    //     CodeGenerator codegen;
    //     codegen.initialize("hulk_module");
    //     codegen.generateCode(program);

    //     std::cout << "Código LLVM generado:\n";
    //     codegen.printIR();

    //     // Opcional: guardar el código IR en un archivo
    //     std::string output_filename = std::string(argv[1]) + ".ll";
    //     codegen.writeIRToFile(output_filename);
    //     std::cout << "Código IR guardado en: " << output_filename << std::endl;
    // }

    // 7. Evaluación del programa
    // if (program)
    // {
    //     std::cout << "\nEjecutando programa...\n";
    //     try
    //     {
    //         EvaluatorVisitor evaluator;
    //         program->accept(&evaluator);
    //         std::cout << "Programa ejecutado exitosamente.\n";
    //     }
    //     catch (const std::exception &ex)
    //     {
    //         std::cerr << "Error durante la ejecución: " << ex.what() << std::endl;
    //         delete program;
    //         return 4;
    //     }
    // }

    // 8. Imprimir el AST
    if (program)
    {
        std::cout << "\nAST:\n";
        PrintVisitor printer;
        program->accept(&printer);
    }
    else
    {
        std::cout << "No se generó AST." << std::endl;
    }

    delete program;
    return 0;
}