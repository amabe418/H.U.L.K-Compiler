#include "token.hpp"
#include <map>

std::string getTokenName(TokenType type)
{
    switch (type)
    {
    case TOKEN_EOF:
        return "EOF";
    case NUMBER:
        return "NUMBER";
    case STRING:
        return "STRING";
    case TRUE:
        return "TRUE";
    case FALSE:
        return "FALSE";
    case IDENT:
        return "IDENT";
    case IF:
        return "IF";
    case ELSE:
        return "ELSE";
    case ELIF:
        return "ELIF";
    case LET:
        return "LET";
    case IN:
        return "IN";
    case WHILE:
        return "WHILE";
    case FOR:
        return "FOR";
    case TYPE:
        return "TYPE";
    case SELF:
        return "SELF";
    case NEW:
        return "NEW";
    case BASE:
        return "BASE";
    case INHERITS:
        return "INHERITS";
    case FUNCTION:
        return "FUNCTION";
    case IS:
        return "IS";
    case AS:
        return "AS";
    case PLUS:
        return "PLUS";
    case MINUS:
        return "MINUS";
    case MULT:
        return "MULT";
    case DIV:
        return "DIV";
    case MOD:
        return "MOD";
    case POW:
        return "POW";
    case LE:
        return "LE";
    case GE:
        return "GE";
    case EQ:
        return "EQ";
    case NEQ:
        return "NEQ";
    case LESS_THAN:
        return "LESS_THAN";
    case GREATER_THAN:
        return "GREATER_THAN";
    case OR:
        return "OR";
    case AND:
        return "AND";
    case ASSIGN:
        return "ASSIGN";
    case ASSIGN_DESTRUCT:
        return "ASSIGN_DESTRUCT";
    case ARROW:
        return "ARROW";
    case CONCAT:
        return "CONCAT";
    case CONCAT_WS:
        return "CONCAT_WS";
    case COMMA:
        return "COMMA";
    case SEMICOLON:
        return "SEMICOLON";
    case DOT:
        return "DOT";
    case COLON:
        return "COLON";
    case LPAREN:
        return "LPAREN";
    case RPAREN:
        return "RPAREN";
    case LBRACE:
        return "LBRACE";
    case RBRACE:
        return "RBRACE";
    case ERROR:
        return "ERROR";
    default:
        return "UNKNOWN";
    }
}

TokenType getKeywordType(const std::string &word)
{
    static const std::map<std::string, TokenType> keywords = {
        {"if", IF},
        {"else", ELSE},
        {"elif", ELIF},
        {"let", LET},
        {"in", IN},
        {"while", WHILE},
        {"for", FOR},
        {"type", TYPE},
        {"self", SELF},
        {"new", NEW},
        {"base", BASE},
        {"inherits", INHERITS},
        {"true", TRUE},
        {"false", FALSE},
        {"function", FUNCTION},
        {"is", IS},
        {"as", AS}};

    auto it = keywords.find(word);
    return (it != keywords.end()) ? it->second : IDENT;
}