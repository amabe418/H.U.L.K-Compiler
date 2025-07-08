#pragma once
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <iostream>
#include "theoretical/token.hpp"

struct DerivationNode
{
    std::string symbol; // No terminal o terminal
    std::vector<std::unique_ptr<DerivationNode>> children;
    std::optional<Token> token; // Solo para terminales
    int line_number;
    int column_number;

    DerivationNode(const std::string &sym) : symbol(sym), line_number(0), column_number(0) {}
    DerivationNode(const std::string &sym, const Token &tok) : symbol(sym), token(tok), line_number(tok.line), column_number(tok.column) {}

    // Método para agregar un hijo
    void addChild(std::unique_ptr<DerivationNode> child)
    {
        children.push_back(std::move(child));
    }

    // Obtener el símbolo
    std::string getSymbol() const
    {
        return symbol;
    }

    // Establecer el token (para terminales)
    void setToken(const Token &tok)
    {
        token = tok;
        line_number = tok.line;
        column_number = tok.column;
    }

    // Método para imprimir el árbol (para debugging)
    void print(int depth = 0) const
    {
        std::string indent(depth * 2, ' ');
        std::cout << indent << symbol;
        if (token.has_value())
        {
            std::cout << " ('" << token->lexeme << "')";
        }
        std::cout << std::endl;

        for (const auto &child : children)
        {
            child->print(depth + 1);
        }
    }
};

using DerivationNodePtr = std::unique_ptr<DerivationNode>;