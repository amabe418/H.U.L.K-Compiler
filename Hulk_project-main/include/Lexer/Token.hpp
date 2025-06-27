#pragma once
#include <string>
#include <filesystem>

enum TokenType {
    Number,
    EOFs,
    Identifier,
    String,

    // Operators 
    op_Plus,           // +
    op_Minus,          // -
    op_Multiply,       // *
    op_Exp , 
    op_Divide,         // /
    op_Modulo,         // %
    op_Equal,          // ==
    op_NotEqual,       // !=
    op_Less,           // <
    op_LessEqual,      // <=
    op_Greater,        // >
    op_GreaterEqual,   // >=
    op_LogicalAnd,     // &&
    op_LogicalOr,       // ||
    op_LogicalNot,    
    op_And,            // &
    op_Or,             // |
    op_Not,            // !
    op_destruc,

    // Punctuators
    punc_LeftParen,      // (
    punc_RightParen,     // )
    punc_LeftBrace,      // {
    punc_RightBrace,     // }
    punc_LeftBracket,    // [
    punc_RightBracket,   // ]
    punc_Semicolon,      // ;
    punc_Comma,          // ,
    punc_Dot,            // .
    punc_Colon,         // :
    punc_at,           //@
    punc_doubleAt,     //@@

    // Asignación 
    Assignment,     // =

    // Keywords
    kw_if,            
    kw_else,           
    kw_for,            
    kw_while,          
    kw_break,
    kw_Switch,
    kw_Case,
    kw_Default,
    kw_Continue,
    kw_Return,
    kw_elif,
    kw_protocol,
    kw_in,
    kw_let,
    kw_function,
    kw_inherits,
    kw_true_,         
    kw_false_,
    kw_new_,
    kw_null_,
    kw_extends,
    kw_type,
    kw_is,
    kw_as,
    

    arrow, // =>

    //Internal
    Keyword,
    Operator,
    Punctuator,
    Error,
    UNKNOWN,

};

std::string getStringOfToken(TokenType type);
TokenType GetType(TokenType token);
int tokenPrecedence(TokenType token);
bool morePriority(TokenType token1, TokenType token2);

struct Token{
    
    std::string lexeme;
    TokenType type;
    int line,column;

    Token(std::string lexeme,TokenType type,int row,int col)
    {
        this->lexeme=lexeme;
        this->column=col;
        this->type=type;
        this->line=row;
    }
    Token(){}
};
