#include "theoretical/Lexer/theoretical_lexer.hpp"
#include "theoretical/token.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

void printToken(const Token &token)
{
    std::cout << "Token: " << getTokenName(token.type)
              << " | Value: '" << token.lexeme << "'"
              << " | Line: " << token.line
              << " | Column: " << token.column << std::endl;
}

int main(int argc, char *argv[])
{
    std::string input;

    if (argc > 1)
    {
        // Leer desde archivo
        std::ifstream file(argv[1]);
        if (!file.is_open())
        {
            std::cerr << "Error: No se pudo abrir el archivo " << argv[1] << std::endl;
            return 1;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        input = buffer.str();
    }
    else
    {
        // Entrada estándar
        std::cout << "Ingrese el código H.U.L.K (Ctrl+D para terminar):" << std::endl;
        std::string line;
        while (std::getline(std::cin, line))
        {
            input += line + "\n";
        }
    }

    TheoreticalLexer lexer(input);

    std::cout << "=== Análisis Léxico Teórico ===" << std::endl;
    std::cout << "Entrada:" << std::endl
              << input << std::endl;
    std::cout << "Tokens:" << std::endl;

    std::vector<Token> tokens = lexer.tokenize();

    for (const Token &token : tokens)
    {
        if (token.type == TOKEN_EOF)
        {
            std::cout << "EOF" << std::endl;
            break;
        }
        printToken(token);
    }

    return 0;
}