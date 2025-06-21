/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_SRC_PARSER_PARSER_TAB_HPP_INCLUDED
# define YY_YY_SRC_PARSER_PARSER_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    LET = 258,                     /* LET  */
    IN = 259,                      /* IN  */
    WHILE = 260,                   /* WHILE  */
    FOR = 261,                     /* FOR  */
    IDENT = 262,                   /* IDENT  */
    IF = 263,                      /* IF  */
    ELSE = 264,                    /* ELSE  */
    ELIF = 265,                    /* ELIF  */
    FUNCTION = 266,                /* FUNCTION  */
    ARROW = 267,                   /* ARROW  */
    ASSIGN = 268,                  /* ASSIGN  */
    ASSIGN_DESTRUCT = 269,         /* ASSIGN_DESTRUCT  */
    TYPE = 270,                    /* TYPE  */
    DOT = 271,                     /* DOT  */
    SELF = 272,                    /* SELF  */
    NEW = 273,                     /* NEW  */
    INHERITS = 274,                /* INHERITS  */
    BASE = 275,                    /* BASE  */
    IS = 276,                      /* IS  */
    TRUE = 277,                    /* TRUE  */
    FALSE = 278,                   /* FALSE  */
    NUMBER = 279,                  /* NUMBER  */
    STRING = 280,                  /* STRING  */
    PLUS = 281,                    /* PLUS  */
    MINUS = 282,                   /* MINUS  */
    MULT = 283,                    /* MULT  */
    DIV = 284,                     /* DIV  */
    MOD = 285,                     /* MOD  */
    POW = 286,                     /* POW  */
    CONCAT = 287,                  /* CONCAT  */
    CONCAT_WS = 288,               /* CONCAT_WS  */
    LE = 289,                      /* LE  */
    GE = 290,                      /* GE  */
    EQ = 291,                      /* EQ  */
    NEQ = 292,                     /* NEQ  */
    LESS_THAN = 293,               /* LESS_THAN  */
    GREATER_THAN = 294,            /* GREATER_THAN  */
    OR = 295,                      /* OR  */
    AND = 296,                     /* AND  */
    LPAREN = 297,                  /* LPAREN  */
    RPAREN = 298,                  /* RPAREN  */
    LBRACE = 299,                  /* LBRACE  */
    RBRACE = 300,                  /* RBRACE  */
    COMMA = 301,                   /* COMMA  */
    SEMICOLON = 302,               /* SEMICOLON  */
    COLON = 303,                   /* COLON  */
    INT = 304,                     /* INT  */
    FLOAT = 305,                   /* FLOAT  */
    BOOL = 306,                    /* BOOL  */
    VOID = 307,                    /* VOID  */
    UMINUS = 308                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 81 "src/Parser/parser.y"

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

#line 142 "src/Parser/parser.tab.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;

int yyparse (void);


#endif /* !YY_YY_SRC_PARSER_PARSER_TAB_HPP_INCLUDED  */
