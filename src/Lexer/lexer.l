%option noyywrap
%option noinput nounput
%option yylineno

%{
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <cstring>
#include <string>

#include "../AST/ast.hpp"
#include "../Parser/parser.tab.hpp"

extern int yylineno;

int yycolumn = 1;                   /* columna actual */
/* Antes de cada token, actualizamos:
   - yylloc (para Bison)
   - yycolumn (aumenta por la longitud del token) */
#define YY_USER_ACTION                            \
    yylloc.first_line   = yylineno;               \
    yylloc.first_column = yycolumn;               \
    yylloc.last_line    = yylineno;               \
    yylloc.last_column  = yycolumn + yyleng - 1;  \
    yycolumn += yyleng;

// Función para imprimir información del token
void print_token(const char* token_name, const char* token_value) {
    printf("Token: %-15s Value: %-20s Line: %d, Column: %d\n", 
           token_name, token_value, yylineno, yycolumn - yyleng);
}

// Necesario para evitar conflictos con strdup (definido en <string.h>)
#ifdef strdup
#undef strdup
#endif
%}

%%

<<EOF>>                     { return 0; }
\n                          { yylineno++; yycolumn = 1; }
\r\n                        { yylineno++; yycolumn = 1; }
\r                          { yylineno++; yycolumn = 1; }
[ \t]+                      { /* skip whitespace */ }
"//"[^\n]*                 { /* skip comments */ }                     

[0-9]+(\.[0-9]+)?           {
                              yylval.expr = new NumberExpr(atof(yytext));
                              print_token("NUMBER", yytext);
                              return NUMBER;
                           }

\"([^\"]*)\"                {
                              std::string raw = yytext;
                              // Eliminar comillas del principio y final
                              std::string content = raw.substr(1, raw.length() - 2);
                              yylval.expr = new StringExpr(content);
                              print_token("STRING", content.c_str());
                              return STRING;
                           }

[A-Za-z][A-Za-z0-9_]*  {
    if (strcmp(yytext, "if") == 0)       { print_token("IF", yytext); return IF; }
    if (strcmp(yytext, "else") == 0)     { print_token("ELSE", yytext); return ELSE; }
    if (strcmp(yytext, "elif") == 0)     { print_token("ELIF", yytext); return ELIF; }
    if (strcmp(yytext, "let") == 0)      { print_token("LET", yytext); return LET; }
    if (strcmp(yytext, "in") == 0)       { print_token("IN", yytext); return IN; }
    if (strcmp(yytext, "while") == 0)    { print_token("WHILE", yytext); return WHILE; }
    if (strcmp(yytext, "for") == 0)      { print_token("FOR", yytext); return FOR; }
    if (strcmp(yytext, "type") == 0)     { print_token("TYPE", yytext); return TYPE; }
    if (strcmp(yytext, "self") == 0)     { print_token("SELF", yytext); return SELF; }
    if (strcmp(yytext, "new") == 0)      { print_token("NEW", yytext); return NEW; }
    if (strcmp(yytext, "base") == 0)     { print_token("BASE", yytext); return BASE; }
    if (strcmp(yytext, "inherits") == 0) { print_token("INHERITS", yytext); return INHERITS; }
    if (strcmp(yytext, "true") == 0)     { 
        yylval.expr = new BooleanExpr(true);
        print_token("TRUE", yytext);
        return TRUE; 
    }
    if (strcmp(yytext, "false") == 0)    { 
        yylval.expr = new BooleanExpr(false);
        print_token("FALSE", yytext);
        return FALSE; 
    }
    if (strcmp(yytext,"function") == 0)  { print_token("FUNCTION", yytext); return FUNCTION; }
    if (strcmp(yytext, "is") == 0)       { print_token("IS", yytext); return IS; }

    // Si no es ninguna palabra reservada, entonces es IDENT
    yylval.str = strdup(yytext);
    print_token("IDENT", yytext);
    return IDENT;
}

                    
\,                          { print_token("COMMA", ","); return COMMA; }
\+                          { print_token("PLUS", "+"); return PLUS; }
\-                          { print_token("MINUS", "-"); return MINUS; }
\*                          { print_token("MULT", "*"); return MULT; }
\/                          { print_token("DIV", "/"); return DIV; }
\**                         { print_token("POW", "**"); return POW; }
\(                          { print_token("LPAREN", "("); return LPAREN; }
\)                          { print_token("RPAREN", ")"); return RPAREN; }
\;                          { print_token("SEMICOLON", ";"); return SEMICOLON; }
\%                          { print_token("MOD", "%"); return MOD; }
"<="                        { print_token("LE", "<="); return LE; }
">="                        { print_token("GE", ">="); return GE; }
"=="                        { print_token("EQ", "=="); return EQ; }
\!=                         { print_token("NEQ", "!="); return NEQ; }
"<"                         { print_token("LESS_THAN", "<"); return LESS_THAN; }
">"                         { print_token("GREATER_THAN", ">"); return GREATER_THAN; }
"||"                        { print_token("OR", "||"); return OR; }
"&&"                        { print_token("AND", "&&"); return AND; }  
"="                         { print_token("ASSIGN", "="); return ASSIGN; }
\{                          { print_token("LBRACE", "{"); return LBRACE; }
\}                          { print_token("RBRACE", "}"); return RBRACE; }
":="                        { print_token("ASSIGN_DESTRUCT", ":="); return ASSIGN_DESTRUCT; }
"=>"                        { print_token("ARROW", "=>"); return ARROW; }
"@"                         { print_token("CONCAT", "@"); return CONCAT; }
"@@"                        { print_token("CONCAT_WS", "@@"); return CONCAT_WS; }
\.                          { print_token("DOT", "."); return DOT; }
\:                          { print_token("COLON", ":"); return COLON; }
\^                          { print_token("POW", "^"); return POW; }
.                           {
                              fprintf(stderr, "Caracter no reconocido: %s en la línea %d\n", yytext, yylineno);
                           }

%%
