#pragma once
#include <vector>
#include <iostream>
#include "theoretical/token.hpp"
#include "AST/ast.hpp"
using AttributeDeclPtr = std::unique_ptr<AttributeDecl>;
using MethodDeclPtr = std::unique_ptr<MethodDecl>;
class LL1ParserGenerated {
public:
    LL1ParserGenerated(const std::vector<Token>& tokens);
    Expr* parse_AddExpr();
    Expr* parse_AddExpr_prime();
    Expr* parse_AndExpr();
    Expr* parse_AndExpr_prime();
    std::vector<std::string> parse_ArgIdList();
    void parse_ArgIdListTail(std::vector<std::string>& args);
    std::vector<ExprPtr> parse_ArgList();
    void parse_ArgListTail(std::vector<std::string>& args);
    std::unique_ptr<AttributeDecl> parse_AttributeDef();
    Expr* parse_BlockStmt();
    Expr* parse_CmpExpr();
    Expr* parse_CmpExpr_prime();
    Expr* parse_ConcatExpr();
    Expr* parse_ConcatExpr_prime();
    std::pair<ExprPtr, ExprPtr> parse_ElifBranch();
    std::vector<std::pair<ExprPtr, ExprPtr>> parse_ElifList();
    Expr* parse_Expr();
    Expr* parse_Factor();
    Expr* parse_Factor_prime();
    Expr* parse_ForBody();
    Expr* parse_ForStmt();
    Expr* parse_FunctionBody();
    Stmt* parse_FunctionDef();
    Expr* parse_IfBody();
    Expr* parse_IfExpr();
    Expr* parse_LetBody();
    Expr* parse_LetExpr();
    std::unique_ptr<MethodDecl> parse_MethodDef();
    Expr* parse_OrExpr();
    Expr* parse_OrExpr_prime();
    Expr* parse_Power();
    Expr* parse_Primary();
    Expr* parse_PrimaryTail(const std::string& varName);
    Program* parse_Program();
    Stmt* parse_Stmt();
    std::vector<StmtPtr> parse_StmtList();
    std::vector<StmtPtr> parse_StmtListTail();
    Expr* parse_Term();
    Expr* parse_Term_prime();
    std::vector<ExprPtr> parse_TypeBaseArgs();
    std::pair<std::vector<std::unique_ptr<AttributeDecl>>, std::vector<std::unique_ptr<MethodDecl>>> parse_TypeBody();
    Stmt* parse_TypeDef();
    std::pair<std::string, std::vector<ExprPtr>> parse_TypeInheritance();
    std::pair<std::unique_ptr<AttributeDecl>, std::unique_ptr<MethodDecl>> parse_TypeMember();
    std::pair<std::unique_ptr<AttributeDecl>, std::unique_ptr<MethodDecl>> parse_TypeMemberTail(const std::string& memberName);
    std::vector<std::string> parse_TypeParams();
    Expr* parse_Unary();
    Expr* parse_VarBinding();
    std::vector<std::pair<std::string, ExprPtr>> parse_VarBindingList();
    void parse_VarBindingListTail(std::vector<std::pair<std::string, ExprPtr>>& bindings);
    Expr* parse_WhileBody();
    Expr* parse_WhileStmt();
private:
    std::vector<Token> tokens;
    size_t pos;
    const Token& lookahead() const { return tokens[pos]; }
    std::string tokenTypeToString(TokenType t) {
        switch(t) {
            case NUMBER: return "NUMBER";
            case STRING: return "STRING";
            case IDENT: return "IDENT";
            case TRUE: return "TRUE";
            case FALSE: return "FALSE";
            case IF: return "IF";
            case ELSE: return "ELSE";
            case ELIF: return "ELIF";
            case WHILE: return "WHILE";
            case FOR: return "FOR";
            case IN: return "IN";
            case LET: return "LET";
            case FUNCTION: return "FUNCTION";
            case LPAREN: return "LPAREN";
            case RPAREN: return "RPAREN";
            case LBRACE: return "LBRACE";
            case RBRACE: return "RBRACE";
            case SEMICOLON: return "SEMICOLON";
            case COMMA: return "COMMA";
            case PLUS: return "PLUS";
            case MINUS: return "MINUS";
            case MULT: return "MULT";
            case DIV: return "DIV";
            case MOD: return "MOD";
            case POW: return "POW";
            case ASSIGN: return "ASSIGN";
            case ASSIGN_DESTRUCT: return "ASSIGN_DESTRUCT";
            case ARROW: return "ARROW";
            case LESS_THAN: return "LESS_THAN";
            case GREATER_THAN: return "GREATER_THAN";
            case LE: return "LE";
            case GE: return "GE";
            case EQ: return "EQ";
            case NEQ: return "NEQ";
            case OR: return "OR";
            case AND: return "AND";
            case CONCAT: return "CONCAT";
            case CONCAT_WS: return "CONCAT_WS";
            case TYPE: return "TYPE";
            case INHERITS: return "INHERITS";
            case NEW: return "NEW";
            case SELF: return "SELF";
            case BASE: return "BASE";
            case DOT: return "DOT";
            case TOKEN_EOF: return "EOF";
            default: return "UNKNOWN";
        }
    }
    void match(TokenType t) { if (tokens[pos].type != t) { const Token& tok = tokens[pos]; std::cerr << "Error de parseo: Se esperaba token " << tokenTypeToString(t) << " pero se encontró '" << tok.lexeme << "' (" << tokenTypeToString(tok.type) << ") en línea " << tok.line << ", columna " << tok.column << std::endl; throw std::runtime_error("Token mismatch"); } ++pos; }
};
