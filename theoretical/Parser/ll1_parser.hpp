#pragma once
#include "AST/ast.hpp"
#include "theoretical/token.hpp"
#include <vector>
#include <cstddef>

class LL1Parser
{
public:
    LL1Parser(const std::vector<Token> &tokens);
    Program *parse_program();
    Expr *parse_expr();

private:
    const std::vector<Token> &tokens;
    size_t pos;

    // Métodos auxiliares
    Stmt *parse_stmt();
    Expr *parse_or();
    Expr *parse_and();
    Expr *parse_cmp();
    Expr *parse_concat();
    Expr *parse_add();
    Expr *parse_term();
    Expr *parse_factor();
    Expr *parse_power();
    Expr *parse_unary();
    Expr *parse_primary();
    // ... otros según la gramática
    const Token &lookahead() const;
    void match(TokenType expected);
};