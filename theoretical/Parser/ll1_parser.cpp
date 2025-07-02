#include "ll1_parser.hpp"
#include <stdexcept>
#include <cstdlib>
#include <sstream>

LL1Parser::LL1Parser(const std::vector<Token> &tokens)
    : tokens(tokens), pos(0) {}

Program *LL1Parser::parse_program()
{
    auto program = new Program();
    while (lookahead().type != TOKEN_EOF)
    {
        Expr *expr = parse_expr();
        match(SEMICOLON);
        program->stmts.emplace_back(std::make_unique<ExprStmt>(ExprPtr(expr)));
    }
    return program;
}

Stmt *LL1Parser::parse_stmt()
{
    // Implementación pendiente
    return nullptr;
}

// expr -> or_expr
Expr *LL1Parser::parse_expr()
{
    return parse_or();
}

// or_expr -> and_expr (OR and_expr)*
Expr *LL1Parser::parse_or()
{
    Expr *left = parse_and();
    while (lookahead().type == OR)
    {
        match(OR);
        Expr *right = parse_and();
        left = new BinaryExpr(BinaryExpr::OP_OR, ExprPtr(left), ExprPtr(right));
    }
    return left;
}

// and_expr -> cmp_expr (AND cmp_expr)*
Expr *LL1Parser::parse_and()
{
    Expr *left = parse_cmp();
    while (lookahead().type == AND)
    {
        match(AND);
        Expr *right = parse_cmp();
        left = new BinaryExpr(BinaryExpr::OP_AND, ExprPtr(left), ExprPtr(right));
    }
    return left;
}

// cmp_expr -> concat_expr ((<|>|<=|>=|==|!=) concat_expr)*
Expr *LL1Parser::parse_cmp()
{
    Expr *left = parse_concat();
    while (lookahead().type == LESS_THAN || lookahead().type == GREATER_THAN ||
           lookahead().type == LE || lookahead().type == GE ||
           lookahead().type == EQ || lookahead().type == NEQ)
    {
        TokenType op = lookahead().type;
        match(op);
        Expr *right = parse_concat();
        switch (op)
        {
        case LESS_THAN:
            left = new BinaryExpr(BinaryExpr::OP_LT, ExprPtr(left), ExprPtr(right));
            break;
        case GREATER_THAN:
            left = new BinaryExpr(BinaryExpr::OP_GT, ExprPtr(left), ExprPtr(right));
            break;
        case LE:
            left = new BinaryExpr(BinaryExpr::OP_LE, ExprPtr(left), ExprPtr(right));
            break;
        case GE:
            left = new BinaryExpr(BinaryExpr::OP_GE, ExprPtr(left), ExprPtr(right));
            break;
        case EQ:
            left = new BinaryExpr(BinaryExpr::OP_EQ, ExprPtr(left), ExprPtr(right));
            break;
        case NEQ:
            left = new BinaryExpr(BinaryExpr::OP_NEQ, ExprPtr(left), ExprPtr(right));
            break;
        default:
            throw std::runtime_error("Unexpected comparison operator");
        }
    }
    return left;
}

// concat_expr -> add_expr ((CONCAT | CONCAT_WS) add_expr)*
Expr *LL1Parser::parse_concat()
{
    Expr *left = parse_add();
    while (lookahead().type == CONCAT || lookahead().type == CONCAT_WS)
    {
        TokenType op = lookahead().type;
        match(op);
        Expr *right = parse_add();
        if (op == CONCAT)
        {
            left = new BinaryExpr(BinaryExpr::OP_CONCAT, ExprPtr(left), ExprPtr(right));
        }
        else
        {
            left = new BinaryExpr(BinaryExpr::OP_CONCAT_WS, ExprPtr(left), ExprPtr(right));
        }
    }
    return left;
}

// add_expr -> term ((PLUS|MINUS) term)*
Expr *LL1Parser::parse_add()
{
    Expr *left = parse_term();
    while (lookahead().type == PLUS || lookahead().type == MINUS)
    {
        TokenType op = lookahead().type;
        match(op);
        Expr *right = parse_term();
        if (op == PLUS)
        {
            left = new BinaryExpr(BinaryExpr::OP_ADD, ExprPtr(left), ExprPtr(right));
        }
        else
        {
            left = new BinaryExpr(BinaryExpr::OP_SUB, ExprPtr(left), ExprPtr(right));
        }
    }
    return left;
}

Expr *LL1Parser::parse_term()
{
    Expr *left = parse_factor();
    while (lookahead().type == MULT || lookahead().type == DIV || lookahead().type == MOD)
    {
        TokenType op = lookahead().type;
        match(op);
        Expr *right = parse_factor();
        if (op == MULT)
        {
            left = new BinaryExpr(BinaryExpr::OP_MUL, ExprPtr(left), ExprPtr(right));
        }
        else if (op == DIV)
        {
            left = new BinaryExpr(BinaryExpr::OP_DIV, ExprPtr(left), ExprPtr(right));
        }
        else
        {
            left = new BinaryExpr(BinaryExpr::OP_MOD, ExprPtr(left), ExprPtr(right));
        }
    }
    return left;
}

Expr *LL1Parser::parse_factor()
{
    Expr *left = parse_power();
    while (lookahead().type == POW)
    {
        match(POW);
        Expr *right = parse_power();
        left = new BinaryExpr(BinaryExpr::OP_POW, ExprPtr(left), ExprPtr(right));
    }
    return left;
}

Expr *LL1Parser::parse_power()
{
    return parse_unary();
}

Expr *LL1Parser::parse_unary()
{
    if (lookahead().type == MINUS)
    {
        match(MINUS);
        Expr *operand = parse_unary();
        return new UnaryExpr(UnaryExpr::OP_NEG, ExprPtr(operand));
    }
    return parse_primary();
}

Expr *LL1Parser::parse_primary()
{
    const Token &tok = lookahead();
    if (tok.type == NUMBER)
    {
        double value = std::stod(tok.lexeme);
        match(NUMBER);
        return new NumberExpr(value);
    }
    else if (tok.type == STRING)
    {
        // Quitar comillas si es necesario
        std::string value = tok.lexeme;
        if (!value.empty() && value.front() == '"' && value.back() == '"')
            value = value.substr(1, value.size() - 2);
        match(STRING);
        return new StringExpr(value);
    }
    else if (tok.type == TRUE)
    {
        match(TRUE);
        return new BooleanExpr(true);
    }
    else if (tok.type == FALSE)
    {
        match(FALSE);
        return new BooleanExpr(false);
    }
    else if (tok.type == IDENT)
    {
        std::string name = tok.lexeme;
        match(IDENT);
        return new VariableExpr(name);
    }
    else if (tok.type == LPAREN)
    {
        match(LPAREN);
        Expr *e = parse_expr();
        match(RPAREN);
        return e;
    }
    else
    {
        std::ostringstream oss;
        oss << "Unexpected token in primary: "
            << getTokenName(tok.type) << " ('" << tok.lexeme << "') en linea " << tok.line << ", columna " << tok.column;
        throw std::runtime_error(oss.str());
    }
}

const Token &LL1Parser::lookahead() const
{
    if (pos < tokens.size())
        return tokens[pos];
    throw std::out_of_range("No more tokens");
}

void LL1Parser::match(TokenType expected)
{
    const Token &tok = lookahead();
    if (tok.type == expected)
    {
        ++pos;
    }
    else
    {
        std::ostringstream oss;
        oss << "Unexpected token: esperado " << getTokenName(expected)
            << ", pero se encontró " << getTokenName(tok.type) << " ('" << tok.lexeme << "') en linea " << tok.line << ", columna " << tok.column;
        throw std::runtime_error(oss.str());
    }
}