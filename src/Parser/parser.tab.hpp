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
     AS = 277,
     TRUE = 278,
     FALSE = 279,
     NUMBER = 280,
     STRING = 281,
     PLUS = 282,
     MINUS = 283,
     MULT = 284,
     DIV = 285,
     MOD = 286,
     POW = 287,
     CONCAT = 288,
     CONCAT_WS = 289,
     LE = 290,
     GE = 291,
     EQ = 292,
     NEQ = 293,
     LESS_THAN = 294,
     GREATER_THAN = 295,
     OR = 296,
     AND = 297,
     LPAREN = 298,
     RPAREN = 299,
     LBRACE = 300,
     RBRACE = 301,
     COMMA = 302,
     SEMICOLON = 303,
     COLON = 304,
     INT = 305,
     FLOAT = 306,
     BOOL = 307,
     VOID = 308,
     UMINUS = 309
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
#define AS 277
#define TRUE 278
#define FALSE 279
#define NUMBER 280
#define STRING 281
#define PLUS 282
#define MINUS 283
#define MULT 284
#define DIV 285
#define MOD 286
#define POW 287
#define CONCAT 288
#define CONCAT_WS 289
#define LE 290
#define GE 291
#define EQ 292
#define NEQ 293
#define LESS_THAN 294
#define GREATER_THAN 295
#define OR 296
#define AND 297
#define LPAREN 298
#define RPAREN 299
#define LBRACE 300
#define RBRACE 301
#define COMMA 302
#define SEMICOLON 303
#define COLON 304
#define INT 305
#define FLOAT 306
#define BOOL 307
#define VOID 308
#define UMINUS 309




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 90 "src/Parser/parser.y"
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
#line 182 "src/Parser/parser.tab.hpp"
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
