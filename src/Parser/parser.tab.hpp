/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     LET = 258,
     IN = 259,
     WHILE = 260,
     FOR = 261,
     IDENT = 262,
     IF = 263,
     ELSE = 264,
     ELIF = 265,
     FUNCTION = 266,
     ARROW = 267,
     ASSIGN = 268,
     ASSIGN_DESTRUCT = 269,
     TYPE = 270,
     DOT = 271,
     SELF = 272,
     NEW = 273,
     INHERITS = 274,
     BASE = 275,
     IS = 276,
     TRUE = 277,
     FALSE = 278,
     NUMBER = 279,
     STRING = 280,
     PLUS = 281,
     MINUS = 282,
     MULT = 283,
     DIV = 284,
     MOD = 285,
     POW = 286,
     CONCAT = 287,
     CONCAT_WS = 288,
     LE = 289,
     GE = 290,
     EQ = 291,
     NEQ = 292,
     LESS_THAN = 293,
     GREATER_THAN = 294,
     OR = 295,
     AND = 296,
     LPAREN = 297,
     RPAREN = 298,
     LBRACE = 299,
     RBRACE = 300,
     COMMA = 301,
     SEMICOLON = 302,
     COLON = 303,
     INT = 304,
     FLOAT = 305,
     BOOL = 306,
     VOID = 307,
     UMINUS = 308
   };
#endif
/* Tokens.  */
#define LET 258
#define IN 259
#define WHILE 260
#define FOR 261
#define IDENT 262
#define IF 263
#define ELSE 264
#define ELIF 265
#define FUNCTION 266
#define ARROW 267
#define ASSIGN 268
#define ASSIGN_DESTRUCT 269
#define TYPE 270
#define DOT 271
#define SELF 272
#define NEW 273
#define INHERITS 274
#define BASE 275
#define IS 276
#define TRUE 277
#define FALSE 278
#define NUMBER 279
#define STRING 280
#define PLUS 281
#define MINUS 282
#define MULT 283
#define DIV 284
#define MOD 285
#define POW 286
#define CONCAT 287
#define CONCAT_WS 288
#define LE 289
#define GE 290
#define EQ 291
#define NEQ 292
#define LESS_THAN 293
#define GREATER_THAN 294
#define OR 295
#define AND 296
#define LPAREN 297
#define RPAREN 298
#define LBRACE 299
#define RBRACE 300
#define COMMA 301
#define SEMICOLON 302
#define COLON 303
#define INT 304
#define FLOAT 305
#define BOOL 306
#define VOID 307
#define UMINUS 308




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 81 "src/Parser/parser.y"
{
  Expr* expr;
  Stmt* stmt;
  Program* prog;
  std::vector<StmtPtr>* stmts;
  std::vector<ExprPtr>* expr_list;
  std::vector<std::string>* str_list;
  std::pair<std::string, Expr*>* binding;
  std::vector<std::pair<std::string, Expr*>>* bindings;
  std::vector<std::pair<std::string, std::pair<Expr*, std::shared_ptr<TypeInfo>>>>* bindings_with_types;
  std::pair<
      std::vector<std::unique_ptr<AttributeDecl>>,
      std::vector<std::unique_ptr<MethodDecl>>
    >* mems;
  std::pair<AttributeDecl*, MethodDecl*>* member_pair; 
    AttributeDecl* attribute_decl;
    MethodDecl* method_decl;
    std::shared_ptr<TypeInfo>* type_info;
    std::vector<std::shared_ptr<TypeInfo>>* type_info_list;
    std::pair<std::string, std::shared_ptr<TypeInfo>>* param_with_type;
    std::vector<std::pair<std::string, std::shared_ptr<TypeInfo>>>* param_list;
    std::pair<std::string, std::pair<Expr*, std::shared_ptr<TypeInfo>>>* binding_with_type;
    char* str;
}
/* Line 1529 of yacc.c.  */
#line 180 "src/Parser/parser.tab.hpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif

extern YYLTYPE yylloc;
