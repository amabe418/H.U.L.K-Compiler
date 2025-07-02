#pragma once
#include <string>

// Enum de tipos de token basado en lexer.l
enum TokenType
{
    // EOF
    TOKEN_EOF = 0,

    // Literales
    NUMBER,
    STRING,
    TRUE,
    FALSE,

    // Identificadores
    IDENT,

    // Palabras clave
    IF,
    ELSE,
    ELIF,
    LET,
    IN,
    WHILE,
    FOR,
    TYPE,
    SELF,
    NEW,
    BASE,
    INHERITS,
    FUNCTION,
    IS,
    AS,

    // Operadores
    PLUS,  // +
    MINUS, // -
    MULT,  // *
    DIV,   // /
    MOD,   // %
    POW,   // ** o ^

    // Operadores de comparación
    LE,           // <=
    GE,           // >=
    EQ,           // ==
    NEQ,          // !=
    LESS_THAN,    // <
    GREATER_THAN, // >

    // Operadores lógicos
    OR,  // ||
    AND, // &&

    // Asignación
    ASSIGN,          // =
    ASSIGN_DESTRUCT, // :=

    // Símbolos especiales
    ARROW,     // =>
    CONCAT,    // @
    CONCAT_WS, // @@

    // Puntuación
    COMMA,     // ,
    SEMICOLON, // ;
    DOT,       // .
    COLON,     // :
    LPAREN,    // (
    RPAREN,    // )
    LBRACE,    // {
    RBRACE,    // }

    // Error
    ERROR
};

// Estructura Token con información completa
struct Token
{
    TokenType type;
    std::string lexeme;
    int line;
    int column;

    Token(TokenType t, const std::string &lex, int l, int c)
        : type(t), lexeme(lex), line(l), column(c) {}

    Token() : type(TOKEN_EOF), line(0), column(0) {}
};

// Función para obtener el nombre del token como string
std::string getTokenName(TokenType type);

// Función para verificar si un string es una palabra clave
TokenType getKeywordType(const std::string &word);