/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 1



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




/* Copy the first part of user declarations.  */
#line 5 "src/Parser/parser.y"

  /* Definir manualmente YYLTYPE */
  typedef struct YYLTYPE {
      int first_line;
      int first_column;
      int last_line;
      int last_column;
  } YYLTYPE;
  #define YYLTYPE_IS_DECLARED 1

  #include <cstdio>
  #include <cstdlib>
  #include "AST/ast.hpp"
  #include "Types/type_info.hpp"
  extern Program* rootAST;
  extern int yylex(void);
  extern FILE* yyin;
  extern int yylineno;

  /* 1) Función interna que maneja ubicación detallada */
  static void yyerror_loc(YYLTYPE *locp, const char *msg) {
      std::fprintf(stderr,
          "Error de parseo en línea %d, columna %d: %s\n",
          locp->first_line,
          locp->first_column,
          msg);
      
      // Información adicional de debug
      extern char* yytext;
      std::fprintf(stderr, "Token actual: '%s'\n", yytext ? yytext : "NULL");
      
      std::exit(EXIT_FAILURE);
  }

  /* 2) Firma que Bison invoca CON ubicación (2 args) */
  void yyerror(YYLTYPE *locp, const char *msg) {
      yyerror_loc(locp, msg);
  }

  /* 3) Firma que Bison invoca SIN ubicación (1 arg) */
  void yyerror(const char *msg) {
      /* `yylloc` es la variable global que Bison mantiene */
      extern YYLTYPE yylloc;
      yyerror_loc(&yylloc, msg);
  }

  #include <string>
  #include <vector>
  #include <memory>
  #include <iostream>
  #include <cstdlib>
  #include <cstring>

  void yyerror(const char* s);
  int yylex();

  extern char* yytext;

  // Inicialización por defecto para TypeInfo
  std::shared_ptr<TypeInfo> defaultTypeInfo() {
      return std::make_shared<TypeInfo>(TypeInfo::Void());
  }

  // Helper function to set location information for AST nodes
  template<typename T>
  void setLocation(T* node, const YYLTYPE& loc) {
      if (node) {
          node->line_number = loc.first_line;
          node->column_number = loc.first_column;
      }
  }

  // Helper function to set location information for expressions
  void setExprLocation(Expr* expr, const YYLTYPE& loc) {
      setLocation(expr, loc);
  }

  // Helper function to set location information for statements
  void setStmtLocation(Stmt* stmt, const YYLTYPE& loc) {
      setLocation(stmt, loc);
  }


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

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
/* Line 193 of yacc.c.  */
#line 312 "src/Parser/parser.tab.cpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

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


/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 337 "src/Parser/parser.tab.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
    YYLTYPE yyls;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1227

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  55
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  19
/* YYNRULES -- Number of rules.  */
#define YYNRULES  106
/* YYNRULES -- Number of states.  */
#define YYNSTATES  273

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   309

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     6,    10,    14,    16,    20,    24,
      30,    32,    43,    53,    62,    70,    77,    79,    80,    84,
      88,    89,    91,    95,    97,    99,   101,   103,   105,   109,
     113,   117,   121,   125,   129,   133,   137,   141,   145,   149,
     153,   157,   161,   165,   169,   172,   177,   181,   187,   192,
     199,   205,   210,   214,   219,   228,   235,   237,   243,   251,
     261,   265,   269,   273,   279,   286,   288,   292,   296,   300,
     308,   321,   332,   344,   352,   371,   386,   387,   389,   393,
     407,   418,   426,   435,   446,   452,   453,   456,   459,   462,
     466,   472,   479,   486,   495,   503,   511,   521,   527,   535,
     542,   551,   553,   554,   556,   560,   562
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      56,     0,    -1,    57,    -1,    -1,    57,    60,    48,    -1,
      57,    63,    48,    -1,    59,    -1,    58,    47,    59,    -1,
       7,    13,    63,    -1,     7,    49,    71,    13,    63,    -1,
      63,    -1,    11,     7,    43,    72,    44,    49,    71,    45,
      61,    46,    -1,    11,     7,    43,    72,    44,    49,    71,
      12,    63,    -1,    11,     7,    43,    44,    49,    71,    12,
      63,    -1,    11,     7,    43,    72,    44,    12,    63,    -1,
      11,     7,    43,    44,    12,    63,    -1,    66,    -1,    -1,
      61,    60,    48,    -1,    61,    63,    48,    -1,    -1,     7,
      -1,    62,    47,     7,    -1,    25,    -1,    26,    -1,    23,
      -1,    24,    -1,     7,    -1,    63,    27,    63,    -1,    63,
      28,    63,    -1,    63,    29,    63,    -1,    63,    30,    63,
      -1,    63,    31,    63,    -1,    63,    32,    63,    -1,    63,
      33,    63,    -1,    63,    34,    63,    -1,    63,    39,    63,
      -1,    63,    40,    63,    -1,    63,    35,    63,    -1,    63,
      36,    63,    -1,    63,    37,    63,    -1,    63,    38,    63,
      -1,    63,    42,    63,    -1,    63,    41,    63,    -1,    28,
      63,    -1,     7,    43,    65,    44,    -1,     7,    43,    44,
      -1,    18,     7,    43,    65,    44,    -1,    18,     7,    43,
      44,    -1,    63,    16,     7,    43,    65,    44,    -1,    63,
      16,     7,    43,    44,    -1,    20,    43,    65,    44,    -1,
      20,    43,    44,    -1,     3,    58,     4,    63,    -1,     3,
       7,    49,    71,    13,    63,     4,    63,    -1,     3,     7,
      13,    63,     4,    63,    -1,    64,    -1,     5,    63,    45,
      61,    46,    -1,     6,    43,     7,     4,    63,    44,    63,
      -1,     6,    43,     7,     4,    63,    44,    45,    61,    46,
      -1,    45,    61,    46,    -1,    43,    63,    44,    -1,    63,
      16,     7,    -1,    63,    16,     7,    14,    63,    -1,    63,
      16,     7,    43,    65,    44,    -1,    17,    -1,    17,    14,
      63,    -1,    20,    43,    44,    -1,     7,    14,    63,    -1,
       8,    43,    63,    44,    63,     9,    63,    -1,     8,    43,
      63,    44,    63,    10,    43,    63,    44,    63,     9,    63,
      -1,     8,    43,    63,    44,    63,    10,    43,    63,    44,
      63,    -1,     8,    43,    63,    44,    45,    61,    46,     9,
      45,    61,    46,    -1,     8,    43,    63,    44,    45,    61,
      46,    -1,     8,    43,    63,    44,    45,    61,    46,    10,
      43,    63,    44,    45,    61,    46,     9,    45,    61,    46,
      -1,     8,    43,    63,    44,    45,    61,    46,    10,    43,
      63,    44,    45,    61,    46,    -1,    -1,    63,    -1,    65,
      47,    63,    -1,    15,     7,    43,    72,    44,    19,     7,
      43,    65,    44,    45,    67,    46,    -1,    15,     7,    19,
       7,    43,    65,    44,    45,    67,    46,    -1,    15,     7,
      19,     7,    45,    67,    46,    -1,    15,     7,    43,    72,
      44,    45,    67,    46,    -1,    15,     7,    43,    72,    44,
      19,     7,    45,    67,    46,    -1,    15,     7,    45,    67,
      46,    -1,    -1,    67,    68,    -1,    69,    48,    -1,    70,
      48,    -1,     7,    13,    63,    -1,     7,    49,    71,    13,
      63,    -1,     7,    43,    62,    44,    12,    63,    -1,     7,
      43,    72,    44,    12,    63,    -1,     7,    43,    72,    44,
      49,    71,    12,    63,    -1,     7,    43,    62,    44,    45,
      61,    46,    -1,     7,    43,    72,    44,    45,    61,    46,
      -1,     7,    43,    72,    44,    49,    71,    45,    61,    46,
      -1,     7,    43,    44,    12,    63,    -1,     7,    43,    44,
      49,    71,    12,    63,    -1,     7,    43,    44,    45,    61,
      46,    -1,     7,    43,    44,    49,    71,    45,    61,    46,
      -1,     7,    -1,    -1,    73,    -1,    72,    47,    73,    -1,
       7,    -1,     7,    49,    71,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   166,   166,   170,   171,   177,   188,   193,   201,   208,
     219,   224,   248,   268,   284,   302,   316,   322,   323,   327,
     334,   335,   336,   340,   345,   346,   350,   354,   360,   365,
     370,   375,   380,   385,   390,   395,   400,   405,   410,   415,
     420,   425,   430,   435,   440,   445,   456,   462,   473,   479,
     490,   496,   506,   511,   538,   546,   553,   555,   562,   568,
     576,   582,   586,   592,   598,   605,   610,   615,   620,   632,
     637,   645,   653,   662,   669,   684,   701,   702,   707,   715,
     736,   751,   766,   785,   806,   821,   828,   840,   845,   853,
     859,   869,   877,   890,   904,   915,   931,   948,   955,   963,
     973,   987,  1005,  1006,  1011,  1019,  1026
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "LET", "IN", "WHILE", "FOR", "IDENT",
  "IF", "ELSE", "ELIF", "FUNCTION", "ARROW", "ASSIGN", "ASSIGN_DESTRUCT",
  "TYPE", "DOT", "SELF", "NEW", "INHERITS", "BASE", "IS", "AS", "TRUE",
  "FALSE", "NUMBER", "STRING", "PLUS", "MINUS", "MULT", "DIV", "MOD",
  "POW", "CONCAT", "CONCAT_WS", "LE", "GE", "EQ", "NEQ", "LESS_THAN",
  "GREATER_THAN", "OR", "AND", "LPAREN", "RPAREN", "LBRACE", "RBRACE",
  "COMMA", "SEMICOLON", "COLON", "INT", "FLOAT", "BOOL", "VOID", "UMINUS",
  "$accept", "input", "program", "binding_list", "binding", "stmt",
  "stmt_list", "ident_list", "expr", "if_expr", "argument_list",
  "type_decl", "member_list", "member", "attribute", "method", "type",
  "param_list", "param", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    55,    56,    57,    57,    57,    58,    58,    59,    59,
      60,    60,    60,    60,    60,    60,    60,    61,    61,    61,
      62,    62,    62,    63,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    64,
      64,    64,    64,    64,    64,    64,    65,    65,    65,    66,
      66,    66,    66,    66,    66,    67,    67,    68,    68,    69,
      69,    70,    70,    70,    70,    70,    70,    70,    70,    70,
      70,    71,    72,    72,    72,    73,    73
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     3,     3,     1,     3,     3,     5,
       1,    10,     9,     8,     7,     6,     1,     0,     3,     3,
       0,     1,     3,     1,     1,     1,     1,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     4,     3,     5,     4,     6,
       5,     4,     3,     4,     8,     6,     1,     5,     7,     9,
       3,     3,     3,     5,     6,     1,     3,     3,     3,     7,
      12,    10,    11,     7,    18,    14,     0,     1,     3,    13,
      10,     7,     8,    10,     5,     0,     2,     2,     2,     3,
       5,     6,     6,     8,     7,     7,     9,     5,     7,     6,
       8,     1,     0,     1,     3,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     2,     1,     0,     0,     0,    27,     0,     0,
       0,    65,     0,     0,    25,    26,    23,    24,     0,     0,
      17,     0,     0,    56,    16,     0,     0,     6,     0,     0,
       0,    76,     0,     0,     0,     0,     0,    76,    44,     0,
       0,     4,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     5,
       0,     0,     0,     0,    17,     0,    68,    46,    77,     0,
       0,   102,     0,   102,    85,    66,    76,    52,     0,    61,
      60,     0,     0,    62,    28,    29,    30,    31,    32,    33,
      34,    35,    38,    39,    40,    41,    36,    37,    43,    42,
       8,   101,     0,    53,     0,     7,     0,     0,    45,     0,
       0,   105,     0,     0,   103,     0,     0,     0,    48,     0,
      51,    18,    19,     0,    76,     0,     0,     0,     0,    57,
       0,    78,    17,     0,     0,     0,     0,     0,     0,    76,
      85,     0,     0,    84,    86,     0,     0,    47,    63,    50,
       0,    55,     9,     8,     0,     0,     0,     0,     0,   106,
      15,     0,     0,     0,   104,     0,     0,     0,    85,     0,
      20,     0,    87,    88,    49,     0,     0,    17,    58,    60,
      69,     0,     0,    14,     0,     0,    81,     0,     0,    89,
      21,     0,     0,     0,     0,    54,     9,     0,     0,     0,
       0,    13,     0,    17,    85,    76,    85,    82,     0,    17,
       0,     0,     0,     0,     0,    59,    17,     0,     0,    12,
       0,     0,     0,     0,    97,     0,     0,     0,    17,    22,
       0,    17,     0,    90,     0,     0,    71,    11,    80,     0,
      83,    99,     0,    17,    91,     0,    92,     0,     0,    72,
       0,     0,    85,    98,     0,    94,    95,     0,    17,    17,
      70,     0,   100,    93,     0,     0,    79,    96,    75,     0,
      17,     0,    74
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    26,    27,    81,    40,   192,    82,    23,
      69,    24,   117,   144,   145,   146,   102,   113,   114
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -110
static const yytype_int16 yypact[] =
{
    -110,    11,   549,  -110,    86,   689,    51,    -7,   109,    93,
     131,     4,   137,   121,  -110,  -110,  -110,  -110,   689,   689,
    -110,   114,   894,  -110,  -110,    78,    10,  -110,   948,   158,
     689,   573,   689,   125,   113,   689,   127,   602,   156,   975,
     228,  -110,   168,   689,   689,   689,   689,   689,   689,   689,
     689,   689,   689,   689,   689,   689,   689,   689,   689,  -110,
     689,   169,   689,   170,  -110,   174,  1110,  -110,  1110,   -39,
    1002,    65,   186,   192,  -110,  1110,   631,  -110,   -38,  -110,
    -110,   150,   921,   116,   119,   119,   -12,   -12,   -12,   -12,
    1187,  1187,   191,   191,   155,   155,   191,   191,  1137,  1162,
     760,  -110,   188,  1110,    79,  -110,   261,   689,  -110,   689,
     713,   153,    68,   -34,  -110,   -20,   -32,    27,  -110,   -25,
    -110,  -110,  -110,   689,   660,   689,   689,   689,   169,  -110,
    1029,  1110,  -110,   833,   169,   689,   169,    71,   192,   689,
    -110,   112,    91,  -110,  -110,   157,   161,  -110,  1110,  -110,
      37,  1110,   799,  1110,   197,   737,   285,   689,   171,  -110,
    1110,   199,   689,   169,  -110,    41,    30,   206,  -110,   689,
      75,   169,  -110,  -110,  -110,   689,   689,  -110,  1110,    17,
    1110,   689,   689,  1110,    56,   159,  -110,   118,    32,  1110,
     153,    54,    42,    43,   204,  1110,  1110,   309,   179,   183,
    1056,  1110,   689,  -110,  -110,   689,  -110,  -110,   689,  -110,
     169,    84,   220,    57,   689,  -110,  -110,   689,   689,  1110,
     333,    64,    95,    70,  1110,   357,   100,   689,  -110,  -110,
     689,  -110,   169,  1110,   381,  1083,   867,  -110,  -110,   184,
    -110,  -110,   689,  -110,  1110,   405,  1110,   429,   101,  -110,
     185,   689,  -110,  1110,   453,  -110,  -110,   689,  -110,  -110,
    1110,    72,  -110,  1110,   477,   501,  -110,  -110,   223,   193,
    -110,   525,  -110
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -110,  -110,  -110,  -110,   178,   235,   -62,  -110,    -2,  -110,
     -13,  -110,  -109,  -110,  -110,  -110,   -96,   -72,   104
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -74
static const yytype_int16 yytable[] =
{
      22,   116,   106,    28,    42,   108,   120,    30,   109,   109,
     137,     3,   141,   138,    62,   138,    38,    39,    35,   147,
      48,   -73,   109,   139,    78,   140,   198,   199,    66,    68,
      70,   166,   154,    75,   142,    68,    31,   142,   159,   142,
     161,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,    63,   100,   188,
     103,   -73,   -73,   119,   -73,   -73,   208,   184,   202,   230,
     156,   142,   111,   143,    68,   194,   186,   142,   207,   142,
     135,   174,   190,   162,   109,   185,   211,   213,   109,   212,
     138,    60,   127,    25,    29,   221,   227,   223,   193,   209,
      33,   203,   231,   210,   169,   130,   232,   131,   133,   112,
     238,   150,   242,   257,   226,   197,   240,   136,   266,   191,
     163,   148,    68,   151,   152,   153,   165,    61,   128,   228,
     123,   167,    72,   160,   170,    42,   248,    68,    34,   239,
     171,   220,   109,   261,    36,   243,   258,   225,    45,    46,
      47,    48,    32,   178,   234,   180,    73,   168,    74,   124,
     183,   205,    41,   206,    37,    65,   245,   189,    71,   247,
      76,    42,    42,   195,   196,    83,   101,   104,   107,   200,
     201,   254,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,   222,   115,    55,    56,   264,   265,   121,   111,
     219,   126,   134,    68,   204,   172,   224,    42,   271,   173,
     176,   182,   233,   187,   181,   235,   236,   214,    43,    44,
      45,    46,    47,    48,   216,   244,   217,   229,   246,   252,
     259,     4,   269,     5,     6,     7,     8,    21,   270,     9,
     253,   105,   164,    10,     0,    11,    12,     0,    13,   260,
       0,    14,    15,    16,    17,   263,    18,     0,     0,     0,
       0,     0,     0,     0,     4,     0,     5,     6,     7,     8,
       0,    19,     9,    20,    80,     0,    10,     0,    11,    12,
       0,    13,     0,     0,    14,    15,    16,    17,     4,    18,
       5,     6,     7,     8,     0,     0,     9,     0,     0,     0,
      10,     0,    11,    12,    19,    13,    20,   129,    14,    15,
      16,    17,     4,    18,     5,     6,     7,     8,     0,     0,
       9,     0,     0,     0,    10,     0,    11,    12,    19,    13,
      20,   179,    14,    15,    16,    17,     4,    18,     5,     6,
       7,     8,     0,     0,     9,     0,     0,     0,    10,     0,
      11,    12,    19,    13,    20,   215,    14,    15,    16,    17,
       4,    18,     5,     6,     7,     8,     0,     0,     9,     0,
       0,     0,    10,     0,    11,    12,    19,    13,    20,   237,
      14,    15,    16,    17,     4,    18,     5,     6,     7,     8,
       0,     0,     9,     0,     0,     0,    10,     0,    11,    12,
      19,    13,    20,   241,    14,    15,    16,    17,     4,    18,
       5,     6,     7,     8,     0,     0,     9,     0,     0,     0,
      10,     0,    11,    12,    19,    13,    20,   249,    14,    15,
      16,    17,     4,    18,     5,     6,     7,     8,     0,     0,
       9,     0,     0,     0,    10,     0,    11,    12,    19,    13,
      20,   255,    14,    15,    16,    17,     4,    18,     5,     6,
       7,     8,     0,     0,     9,     0,     0,     0,    10,     0,
      11,    12,    19,    13,    20,   256,    14,    15,    16,    17,
       4,    18,     5,     6,     7,     8,     0,     0,     9,     0,
       0,     0,    10,     0,    11,    12,    19,    13,    20,   262,
      14,    15,    16,    17,     4,    18,     5,     6,     7,     8,
       0,     0,     9,     0,     0,     0,    10,     0,    11,    12,
      19,    13,    20,   267,    14,    15,    16,    17,     4,    18,
       5,     6,     7,     8,     0,     0,     9,     0,     0,     0,
      10,     0,    11,    12,    19,    13,    20,   268,    14,    15,
      16,    17,     4,    18,     5,     6,     7,     8,     0,     0,
       9,     0,     0,     0,    10,     0,    11,    12,    19,    13,
      20,   272,    14,    15,    16,    17,     4,    18,     5,     6,
       7,     8,     0,     0,     0,     0,     0,     0,     0,     0,
      11,    12,    19,    13,    20,     0,    14,    15,    16,    17,
       0,    18,     0,     0,     0,     4,     0,     5,     6,     7,
       8,     0,     0,     0,     0,     0,    19,    67,    20,    11,
      12,     0,    13,     0,     0,    14,    15,    16,    17,     0,
      18,     0,     0,     0,     4,     0,     5,     6,     7,     8,
       0,     0,     0,     0,     0,    19,    77,    20,    11,    12,
       0,    13,     0,     0,    14,    15,    16,    17,     0,    18,
       0,     0,     0,     4,     0,     5,     6,     7,     8,     0,
       0,     0,     0,     0,    19,   118,    20,    11,    12,     0,
      13,     0,     0,    14,    15,    16,    17,     0,    18,     0,
       0,     0,     4,     0,     5,     6,     7,     8,     0,     0,
       0,     0,     0,    19,   149,    20,    11,    12,     0,    13,
       0,     0,    14,    15,    16,    17,     4,    18,     5,     6,
       7,     8,     0,     0,     0,     0,     0,     0,     0,     0,
      11,    12,    19,    13,    20,     0,    14,    15,    16,    17,
       4,    18,     5,     6,     7,     8,     0,     0,     0,     0,
       0,     0,     0,     0,    11,    12,    19,    13,   132,     0,
      14,    15,    16,    17,   125,    18,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    42,     0,     0,     0,
      19,     0,   177,     0,     0,     0,     0,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,   175,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    42,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,   157,   158,     0,     0,     0,     0,     0,    42,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,   251,     0,     0,     0,
       0,     0,     0,    42,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      42,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    42,     0,     0,
       0,     0,    59,     0,     0,     0,     0,     0,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    42,     0,     0,     0,     0,   122,
       0,     0,     0,     0,     0,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    42,     0,    64,     0,     0,     0,     0,     0,     0,
       0,     0,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    42,    79,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    42,   110,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    42,   155,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    42,
     218,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    42,   250,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    42,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    42,    58,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    42,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    43,    44,    45,    46,    47,    48,
       0,     0,    51,    52,     0,     0,    55,    56
};

static const yytype_int16 yycheck[] =
{
       2,    73,    64,     5,    16,    44,    44,    14,    47,    47,
      44,     0,    44,    47,     4,    47,    18,    19,    14,    44,
      32,     4,    47,    43,    37,    45,     9,    10,    30,    31,
      32,   140,   128,    35,     7,    37,    43,     7,   134,     7,
     136,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    47,    60,   168,
      62,    44,    45,    76,    47,    48,    12,   163,    12,    12,
     132,     7,     7,    46,    76,   171,    46,     7,    46,     7,
      12,    44,     7,    12,    47,    44,    44,    44,    47,    47,
      47,    13,    13,     7,    43,   204,    12,   206,   170,    45,
       7,    45,    45,    49,    13,   107,    49,   109,   110,    44,
      46,   124,    12,    12,   210,   177,    46,    49,    46,    44,
      49,   123,   124,   125,   126,   127,   139,    49,    49,    45,
      14,    19,    19,   135,    43,    16,   232,   139,     7,    44,
      49,   203,    47,   252,     7,    45,    45,   209,    29,    30,
      31,    32,    43,   155,   216,   157,    43,    45,    45,    43,
     162,    43,    48,    45,    43,     7,   228,   169,    43,   231,
      43,    16,    16,   175,   176,     7,     7,     7,     4,   181,
     182,   243,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,   205,     7,    39,    40,   258,   259,    48,     7,
     202,    13,    49,   205,    45,    48,   208,    16,   270,    48,
      13,    12,   214,     7,    43,   217,   218,    13,    27,    28,
      29,    30,    31,    32,    45,   227,    43,     7,   230,    45,
      45,     3,     9,     5,     6,     7,     8,     2,    45,    11,
     242,    63,   138,    15,    -1,    17,    18,    -1,    20,   251,
      -1,    23,    24,    25,    26,   257,    28,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,    -1,     5,     6,     7,     8,
      -1,    43,    11,    45,    46,    -1,    15,    -1,    17,    18,
      -1,    20,    -1,    -1,    23,    24,    25,    26,     3,    28,
       5,     6,     7,     8,    -1,    -1,    11,    -1,    -1,    -1,
      15,    -1,    17,    18,    43,    20,    45,    46,    23,    24,
      25,    26,     3,    28,     5,     6,     7,     8,    -1,    -1,
      11,    -1,    -1,    -1,    15,    -1,    17,    18,    43,    20,
      45,    46,    23,    24,    25,    26,     3,    28,     5,     6,
       7,     8,    -1,    -1,    11,    -1,    -1,    -1,    15,    -1,
      17,    18,    43,    20,    45,    46,    23,    24,    25,    26,
       3,    28,     5,     6,     7,     8,    -1,    -1,    11,    -1,
      -1,    -1,    15,    -1,    17,    18,    43,    20,    45,    46,
      23,    24,    25,    26,     3,    28,     5,     6,     7,     8,
      -1,    -1,    11,    -1,    -1,    -1,    15,    -1,    17,    18,
      43,    20,    45,    46,    23,    24,    25,    26,     3,    28,
       5,     6,     7,     8,    -1,    -1,    11,    -1,    -1,    -1,
      15,    -1,    17,    18,    43,    20,    45,    46,    23,    24,
      25,    26,     3,    28,     5,     6,     7,     8,    -1,    -1,
      11,    -1,    -1,    -1,    15,    -1,    17,    18,    43,    20,
      45,    46,    23,    24,    25,    26,     3,    28,     5,     6,
       7,     8,    -1,    -1,    11,    -1,    -1,    -1,    15,    -1,
      17,    18,    43,    20,    45,    46,    23,    24,    25,    26,
       3,    28,     5,     6,     7,     8,    -1,    -1,    11,    -1,
      -1,    -1,    15,    -1,    17,    18,    43,    20,    45,    46,
      23,    24,    25,    26,     3,    28,     5,     6,     7,     8,
      -1,    -1,    11,    -1,    -1,    -1,    15,    -1,    17,    18,
      43,    20,    45,    46,    23,    24,    25,    26,     3,    28,
       5,     6,     7,     8,    -1,    -1,    11,    -1,    -1,    -1,
      15,    -1,    17,    18,    43,    20,    45,    46,    23,    24,
      25,    26,     3,    28,     5,     6,     7,     8,    -1,    -1,
      11,    -1,    -1,    -1,    15,    -1,    17,    18,    43,    20,
      45,    46,    23,    24,    25,    26,     3,    28,     5,     6,
       7,     8,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      17,    18,    43,    20,    45,    -1,    23,    24,    25,    26,
      -1,    28,    -1,    -1,    -1,     3,    -1,     5,     6,     7,
       8,    -1,    -1,    -1,    -1,    -1,    43,    44,    45,    17,
      18,    -1,    20,    -1,    -1,    23,    24,    25,    26,    -1,
      28,    -1,    -1,    -1,     3,    -1,     5,     6,     7,     8,
      -1,    -1,    -1,    -1,    -1,    43,    44,    45,    17,    18,
      -1,    20,    -1,    -1,    23,    24,    25,    26,    -1,    28,
      -1,    -1,    -1,     3,    -1,     5,     6,     7,     8,    -1,
      -1,    -1,    -1,    -1,    43,    44,    45,    17,    18,    -1,
      20,    -1,    -1,    23,    24,    25,    26,    -1,    28,    -1,
      -1,    -1,     3,    -1,     5,     6,     7,     8,    -1,    -1,
      -1,    -1,    -1,    43,    44,    45,    17,    18,    -1,    20,
      -1,    -1,    23,    24,    25,    26,     3,    28,     5,     6,
       7,     8,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      17,    18,    43,    20,    45,    -1,    23,    24,    25,    26,
       3,    28,     5,     6,     7,     8,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    17,    18,    43,    20,    45,    -1,
      23,    24,    25,    26,     4,    28,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    16,    -1,    -1,    -1,
      43,    -1,    45,    -1,    -1,    -1,    -1,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,     4,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    16,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,     9,    10,    -1,    -1,    -1,    -1,    -1,    16,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     9,    -1,    -1,    -1,
      -1,    -1,    -1,    16,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      16,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    16,    -1,    -1,
      -1,    -1,    48,    -1,    -1,    -1,    -1,    -1,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    16,    -1,    -1,    -1,    -1,    48,
      -1,    -1,    -1,    -1,    -1,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    16,    -1,    45,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    16,    44,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    16,    44,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    16,    44,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    16,
      44,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    16,    44,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    16,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    16,    42,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    16,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    27,    28,    29,    30,    31,    32,
      -1,    -1,    35,    36,    -1,    -1,    39,    40
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    56,    57,     0,     3,     5,     6,     7,     8,    11,
      15,    17,    18,    20,    23,    24,    25,    26,    28,    43,
      45,    60,    63,    64,    66,     7,    58,    59,    63,    43,
      14,    43,    43,     7,     7,    14,     7,    43,    63,    63,
      61,    48,    16,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    48,
      13,    49,     4,    47,    45,     7,    63,    44,    63,    65,
      63,    43,    19,    43,    45,    63,    43,    44,    65,    44,
      46,    60,    63,     7,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      63,     7,    71,    63,     7,    59,    61,     4,    44,    47,
      44,     7,    44,    72,    73,     7,    72,    67,    44,    65,
      44,    48,    48,    14,    43,     4,    13,    13,    49,    46,
      63,    63,    45,    63,    49,    12,    49,    44,    47,    43,
      45,    44,     7,    46,    68,    69,    70,    44,    63,    44,
      65,    63,    63,    63,    71,    44,    61,     9,    10,    71,
      63,    71,    12,    49,    73,    65,    67,    19,    45,    13,
      43,    49,    48,    48,    44,     4,    13,    45,    63,    46,
      63,    43,    12,    63,    71,    44,    46,     7,    67,    63,
       7,    44,    62,    72,    71,    63,    63,    61,     9,    10,
      63,    63,    12,    45,    45,    43,    45,    46,    12,    45,
      49,    44,    47,    44,    13,    46,    45,    43,    44,    63,
      61,    67,    65,    67,    63,    61,    71,    12,    45,     7,
      12,    45,    49,    63,    61,    63,    63,    46,    46,    44,
      46,    46,    12,    45,    63,    61,    63,    61,    71,    46,
      44,     9,    45,    63,    61,    46,    46,    12,    45,    45,
      63,    67,    46,    63,    61,    61,    46,    46,    46,     9,
      45,    61,    46
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value, Location); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;
/* Location data for the look-ahead symbol.  */
YYLTYPE yylloc;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;

  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[2];

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;
#if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 0;
#endif

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
	YYSTACK_RELOCATE (yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 166 "src/Parser/parser.y"
    { rootAST = (yyvsp[(1) - (1)].prog); ;}
    break;

  case 3:
#line 170 "src/Parser/parser.y"
    { (yyval.prog) = new Program(); ;}
    break;

  case 4:
#line 172 "src/Parser/parser.y"
    {
      setStmtLocation((yyvsp[(2) - (3)].stmt), (yylsp[(2) - (3)]));
      (yyvsp[(1) - (3)].prog)->stmts.emplace_back( StmtPtr((yyvsp[(2) - (3)].stmt)) );
      (yyval.prog) = (yyvsp[(1) - (3)].prog);
    ;}
    break;

  case 5:
#line 178 "src/Parser/parser.y"
    {
      auto exprStmt = new ExprStmt(ExprPtr((yyvsp[(2) - (3)].expr)));
      setStmtLocation(exprStmt, (yylsp[(2) - (3)]));
      setExprLocation((yyvsp[(2) - (3)].expr), (yylsp[(2) - (3)]));
      (yyvsp[(1) - (3)].prog)->stmts.emplace_back( StmtPtr(exprStmt) );
      (yyval.prog) = (yyvsp[(1) - (3)].prog);
    ;}
    break;

  case 6:
#line 188 "src/Parser/parser.y"
    {
          (yyval.bindings_with_types) = new std::vector<std::pair<std::string, std::pair<Expr*, std::shared_ptr<TypeInfo>>>>();
          (yyval.bindings_with_types)->push_back(std::make_pair((yyvsp[(1) - (1)].binding_with_type)->first, std::make_pair((yyvsp[(1) - (1)].binding_with_type)->second.first, (yyvsp[(1) - (1)].binding_with_type)->second.second)));
          delete (yyvsp[(1) - (1)].binding_with_type);
      ;}
    break;

  case 7:
#line 193 "src/Parser/parser.y"
    {
          (yyvsp[(1) - (3)].bindings_with_types)->push_back(std::make_pair((yyvsp[(3) - (3)].binding_with_type)->first, std::make_pair((yyvsp[(3) - (3)].binding_with_type)->second.first, (yyvsp[(3) - (3)].binding_with_type)->second.second)));
          delete (yyvsp[(3) - (3)].binding_with_type);
          (yyval.bindings_with_types) = (yyvsp[(1) - (3)].bindings_with_types);
      ;}
    break;

  case 8:
#line 201 "src/Parser/parser.y"
    {
          (yyval.binding_with_type) = new std::pair<std::string, std::pair<Expr*, std::shared_ptr<TypeInfo>>>(
              std::string((yyvsp[(1) - (3)].str)), 
              std::make_pair((yyvsp[(3) - (3)].expr), std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown))
          );
          free((yyvsp[(1) - (3)].str));
      ;}
    break;

  case 9:
#line 208 "src/Parser/parser.y"
    {
          (yyval.binding_with_type) = new std::pair<std::string, std::pair<Expr*, std::shared_ptr<TypeInfo>>>(
              std::string((yyvsp[(1) - (5)].str)), 
              std::make_pair((yyvsp[(5) - (5)].expr), *(yyvsp[(3) - (5)].type_info))
          );
          free((yyvsp[(1) - (5)].str));
          delete (yyvsp[(3) - (5)].type_info);
      ;}
    break;

  case 10:
#line 219 "src/Parser/parser.y"
    {
        auto exprStmt = new ExprStmt(ExprPtr((yyvsp[(1) - (1)].expr)));
        setStmtLocation(exprStmt, (yylsp[(1) - (1)]));
        (yyval.stmt) = exprStmt;
    ;}
    break;

  case 11:
#line 224 "src/Parser/parser.y"
    {
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *(yyvsp[(4) - (10)].param_list)) {
            args.push_back(param.first);
            param_types.push_back(param.second);
        }
        
        // Crear un bloque de expresiones y envolverlo en un ExprStmt
        auto exprBlock = new ExprBlock(std::move(*(yyvsp[(9) - (10)].stmts)));
        auto exprStmt = new ExprStmt(ExprPtr(exprBlock));
        
        auto funcDecl = static_cast<Stmt*>(new FunctionDecl(
            std::string((yyvsp[(2) - (10)].str)),
            std::move(args),
            StmtPtr(exprStmt),
            std::move(param_types),
            *(yyvsp[(7) - (10)].type_info)
        ));
        setStmtLocation(funcDecl, (yylsp[(2) - (10)]));
        (yyval.stmt) = funcDecl;
        delete (yyvsp[(4) - (10)].param_list); delete (yyvsp[(7) - (10)].type_info); delete (yyvsp[(9) - (10)].stmts);
        free((yyvsp[(2) - (10)].str));
    ;}
    break;

  case 12:
#line 248 "src/Parser/parser.y"
    {
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *(yyvsp[(4) - (9)].param_list)) {
            args.push_back(param.first);
            param_types.push_back(param.second);
        }
        
        auto funcDecl = static_cast<Stmt*>(new FunctionDecl(
            std::string((yyvsp[(2) - (9)].str)), 
            std::move(args), 
            std::make_unique<ExprStmt>(ExprPtr((yyvsp[(9) - (9)].expr))), 
            std::move(param_types),
            *(yyvsp[(7) - (9)].type_info)
        ));
        setStmtLocation(funcDecl, (yylsp[(2) - (9)]));
        (yyval.stmt) = funcDecl;
        delete (yyvsp[(4) - (9)].param_list); delete (yyvsp[(7) - (9)].type_info);
        free((yyvsp[(2) - (9)].str));
    ;}
    break;

  case 13:
#line 268 "src/Parser/parser.y"
    {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        
        auto funcDecl = static_cast<Stmt*>(new FunctionDecl(
            std::string((yyvsp[(2) - (8)].str)), 
            std::move(params), 
            std::make_unique<ExprStmt>(ExprPtr((yyvsp[(8) - (8)].expr))), 
            std::move(param_types),
            *(yyvsp[(6) - (8)].type_info)
        ));
        setStmtLocation(funcDecl, (yylsp[(2) - (8)]));
        (yyval.stmt) = funcDecl;
        delete (yyvsp[(6) - (8)].type_info);
        free((yyvsp[(2) - (8)].str));
    ;}
    break;

  case 14:
#line 284 "src/Parser/parser.y"
    {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        
        for (const auto& param : *(yyvsp[(4) - (7)].param_list)) {
            params.push_back(param.first);
            param_types.push_back(param.second);
        }
        
        (yyval.stmt) = static_cast<Stmt*>(new FunctionDecl(
            std::string((yyvsp[(2) - (7)].str)), 
            std::move(params), 
            std::make_unique<ExprStmt>(ExprPtr((yyvsp[(7) - (7)].expr))), 
            std::move(param_types)
        ));
        delete (yyvsp[(4) - (7)].param_list);
        free((yyvsp[(2) - (7)].str));
    ;}
    break;

  case 15:
#line 302 "src/Parser/parser.y"
    {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        
        (yyval.stmt) = static_cast<Stmt*>(new FunctionDecl(
            std::string((yyvsp[(2) - (6)].str)), 
            std::move(params), 
            std::make_unique<ExprStmt>(ExprPtr((yyvsp[(6) - (6)].expr))), 
            std::move(param_types)
        ));
        free((yyvsp[(2) - (6)].str));
    ;}
    break;

  case 17:
#line 322 "src/Parser/parser.y"
    { (yyval.stmts) = new std::vector<StmtPtr>(); ;}
    break;

  case 18:
#line 323 "src/Parser/parser.y"
    { 
      (yyvsp[(1) - (3)].stmts)->push_back(StmtPtr((yyvsp[(2) - (3)].stmt))); 
      (yyval.stmts) = (yyvsp[(1) - (3)].stmts); 
    ;}
    break;

  case 19:
#line 327 "src/Parser/parser.y"
    { 
      (yyvsp[(1) - (3)].stmts)->push_back(StmtPtr(new ExprStmt(ExprPtr((yyvsp[(2) - (3)].expr))))); 
      (yyval.stmts) = (yyvsp[(1) - (3)].stmts); 
    ;}
    break;

  case 20:
#line 334 "src/Parser/parser.y"
    { (yyval.str_list) = new std::vector<std::string>(); ;}
    break;

  case 21:
#line 335 "src/Parser/parser.y"
    { (yyval.str_list) = new std::vector<std::string>(); (yyval.str_list)->push_back((yyvsp[(1) - (1)].str)); free((yyvsp[(1) - (1)].str)); ;}
    break;

  case 22:
#line 336 "src/Parser/parser.y"
    { (yyvsp[(1) - (3)].str_list)->push_back((yyvsp[(3) - (3)].str)); free((yyvsp[(3) - (3)].str)); (yyval.str_list) = (yyvsp[(1) - (3)].str_list); ;}
    break;

  case 23:
#line 340 "src/Parser/parser.y"
    { 
        setExprLocation((yyvsp[(1) - (1)].expr), (yylsp[(1) - (1)]));
        std::cout << "Evaluated expression statement" << std::endl; 
        (yyval.expr) = (yyvsp[(1) - (1)].expr); 
    ;}
    break;

  case 24:
#line 345 "src/Parser/parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 25:
#line 346 "src/Parser/parser.y"
    { 
        setExprLocation((yyvsp[(1) - (1)].expr), (yylsp[(1) - (1)]));
        (yyval.expr) = (yyvsp[(1) - (1)].expr); 
    ;}
    break;

  case 26:
#line 350 "src/Parser/parser.y"
    { 
        setExprLocation((yyvsp[(1) - (1)].expr), (yylsp[(1) - (1)]));
        (yyval.expr) = (yyvsp[(1) - (1)].expr); 
    ;}
    break;

  case 27:
#line 354 "src/Parser/parser.y"
    { 
        auto varExpr = new VariableExpr((yyvsp[(1) - (1)].str));
        setExprLocation(varExpr, (yylsp[(1) - (1)]));
        (yyval.expr) = varExpr; 
        free((yyvsp[(1) - (1)].str)); 
    ;}
    break;

  case 28:
#line 360 "src/Parser/parser.y"
    { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_ADD, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr)));
        setExprLocation(binExpr, (yylsp[(2) - (3)]));
        (yyval.expr) = binExpr; 
    ;}
    break;

  case 29:
#line 365 "src/Parser/parser.y"
    { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_SUB, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr)));
        setExprLocation(binExpr, (yylsp[(2) - (3)]));
        (yyval.expr) = binExpr; 
    ;}
    break;

  case 30:
#line 370 "src/Parser/parser.y"
    { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_MUL, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr)));
        setExprLocation(binExpr, (yylsp[(2) - (3)]));
        (yyval.expr) = binExpr; 
    ;}
    break;

  case 31:
#line 375 "src/Parser/parser.y"
    { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_DIV, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr)));
        setExprLocation(binExpr, (yylsp[(2) - (3)]));
        (yyval.expr) = binExpr; 
    ;}
    break;

  case 32:
#line 380 "src/Parser/parser.y"
    { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_MOD, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr)));
        setExprLocation(binExpr, (yylsp[(2) - (3)]));
        (yyval.expr) = binExpr; 
    ;}
    break;

  case 33:
#line 385 "src/Parser/parser.y"
    { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_POW, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr)));
        setExprLocation(binExpr, (yylsp[(2) - (3)]));
        (yyval.expr) = binExpr; 
    ;}
    break;

  case 34:
#line 390 "src/Parser/parser.y"
    { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_CONCAT, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr)));
        setExprLocation(binExpr, (yylsp[(2) - (3)]));
        (yyval.expr) = binExpr; 
    ;}
    break;

  case 35:
#line 395 "src/Parser/parser.y"
    { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_CONCAT_WS, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr)));
        setExprLocation(binExpr, (yylsp[(2) - (3)]));
        (yyval.expr) = binExpr; 
    ;}
    break;

  case 36:
#line 400 "src/Parser/parser.y"
    { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_LT, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr)));
        setExprLocation(binExpr, (yylsp[(2) - (3)]));
        (yyval.expr) = binExpr; 
    ;}
    break;

  case 37:
#line 405 "src/Parser/parser.y"
    { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_GT, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr)));
        setExprLocation(binExpr, (yylsp[(2) - (3)]));
        (yyval.expr) = binExpr; 
    ;}
    break;

  case 38:
#line 410 "src/Parser/parser.y"
    { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_LE, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr)));
        setExprLocation(binExpr, (yylsp[(2) - (3)]));
        (yyval.expr) = binExpr; 
    ;}
    break;

  case 39:
#line 415 "src/Parser/parser.y"
    { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_GE, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr)));
        setExprLocation(binExpr, (yylsp[(2) - (3)]));
        (yyval.expr) = binExpr; 
    ;}
    break;

  case 40:
#line 420 "src/Parser/parser.y"
    { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_EQ, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr)));
        setExprLocation(binExpr, (yylsp[(2) - (3)]));
        (yyval.expr) = binExpr; 
    ;}
    break;

  case 41:
#line 425 "src/Parser/parser.y"
    { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_NEQ, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr)));
        setExprLocation(binExpr, (yylsp[(2) - (3)]));
        (yyval.expr) = binExpr; 
    ;}
    break;

  case 42:
#line 430 "src/Parser/parser.y"
    { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_AND, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr)));
        setExprLocation(binExpr, (yylsp[(2) - (3)]));
        (yyval.expr) = binExpr; 
    ;}
    break;

  case 43:
#line 435 "src/Parser/parser.y"
    { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_OR, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr)));
        setExprLocation(binExpr, (yylsp[(2) - (3)]));
        (yyval.expr) = binExpr; 
    ;}
    break;

  case 44:
#line 440 "src/Parser/parser.y"
    { 
        auto unaryExpr = new UnaryExpr(UnaryExpr::OP_NEG, ExprPtr((yyvsp[(2) - (2)].expr)));
        setExprLocation(unaryExpr, (yylsp[(1) - (2)]));
        (yyval.expr) = unaryExpr; 
    ;}
    break;

  case 45:
#line 445 "src/Parser/parser.y"
    { 
      std::vector<ExprPtr> args;
      for (auto& arg : *(yyvsp[(3) - (4)].expr_list)) {
          args.push_back(std::move(arg));
      }
      auto callExpr = new CallExpr((yyvsp[(1) - (4)].str), std::move(args));
      setExprLocation(callExpr, (yylsp[(1) - (4)]));
      (yyval.expr) = callExpr; 
      free((yyvsp[(1) - (4)].str)); 
      delete (yyvsp[(3) - (4)].expr_list); 
  ;}
    break;

  case 46:
#line 456 "src/Parser/parser.y"
    { 
      auto callExpr = new CallExpr((yyvsp[(1) - (3)].str), std::vector<ExprPtr>());
      setExprLocation(callExpr, (yylsp[(1) - (3)]));
      (yyval.expr) = callExpr; 
      free((yyvsp[(1) - (3)].str)); 
  ;}
    break;

  case 47:
#line 462 "src/Parser/parser.y"
    {
      std::vector<ExprPtr> args;
      for (auto& arg : *(yyvsp[(4) - (5)].expr_list)) {
          args.push_back(std::move(arg));
      }
      auto newExpr = new NewExpr((yyvsp[(2) - (5)].str), std::move(args));
      setExprLocation(newExpr, (yylsp[(2) - (5)]));
      (yyval.expr) = newExpr;
      free((yyvsp[(2) - (5)].str));
      delete (yyvsp[(4) - (5)].expr_list);
  ;}
    break;

  case 48:
#line 473 "src/Parser/parser.y"
    {
      auto newExpr = new NewExpr((yyvsp[(2) - (4)].str), std::vector<ExprPtr>());
      setExprLocation(newExpr, (yylsp[(2) - (4)]));
      (yyval.expr) = newExpr;
      free((yyvsp[(2) - (4)].str));
  ;}
    break;

  case 49:
#line 479 "src/Parser/parser.y"
    {
      std::vector<ExprPtr> args;
      for (auto& arg : *(yyvsp[(5) - (6)].expr_list)) {
          args.push_back(std::move(arg));
      }
      auto methodCall = new MethodCallExpr(ExprPtr((yyvsp[(1) - (6)].expr)), (yyvsp[(3) - (6)].str), std::move(args));
      setExprLocation(methodCall, (yylsp[(3) - (6)]));
      (yyval.expr) = methodCall;
      free((yyvsp[(3) - (6)].str));
      delete (yyvsp[(5) - (6)].expr_list);
  ;}
    break;

  case 50:
#line 490 "src/Parser/parser.y"
    {
      auto methodCall = new MethodCallExpr(ExprPtr((yyvsp[(1) - (5)].expr)), (yyvsp[(3) - (5)].str), std::vector<ExprPtr>());
      setExprLocation(methodCall, (yylsp[(3) - (5)]));
      (yyval.expr) = methodCall;
      free((yyvsp[(3) - (5)].str));
  ;}
    break;

  case 51:
#line 496 "src/Parser/parser.y"
    {
      std::vector<ExprPtr> args;
      for (auto& arg : *(yyvsp[(3) - (4)].expr_list)) {
          args.push_back(std::move(arg));
      }
      auto baseCall = new BaseCallExpr(std::move(args));
      setExprLocation(baseCall, (yylsp[(1) - (4)]));
      (yyval.expr) = baseCall;
      delete (yyvsp[(3) - (4)].expr_list);
  ;}
    break;

  case 52:
#line 506 "src/Parser/parser.y"
    {
      auto baseCall = new BaseCallExpr({});
      setExprLocation(baseCall, (yylsp[(1) - (3)]));
      (yyval.expr) = baseCall;
  ;}
    break;

  case 53:
#line 511 "src/Parser/parser.y"
    { 
      // Desugar multiple bindings into nested LetExpr
      // Start from the last binding and work backwards
      Expr* result = (yyvsp[(4) - (4)].expr);
      
      // Process bindings in reverse order to maintain correct nesting
      for (int i = (yyvsp[(2) - (4)].bindings_with_types)->size() - 1; i >= 0; --i) {
          const auto& binding = (*(yyvsp[(2) - (4)].bindings_with_types))[i];
          std::string name = binding.first;
          Expr* init = binding.second.first;
          std::shared_ptr<TypeInfo> type = binding.second.second;
          
          if (i == (yyvsp[(2) - (4)].bindings_with_types)->size() - 1) {
              // Last binding: body is the original expression
              result = new LetExpr(name, ExprPtr(init), 
                                  std::make_unique<ExprStmt>(ExprPtr(result)), type);
          } else {
              // Other bindings: body is the nested LetExpr
              result = new LetExpr(name, ExprPtr(init), 
                                  std::make_unique<ExprStmt>(ExprPtr(result)), type);
          }
      }
      
      setExprLocation(result, (yylsp[(1) - (4)]));
      (yyval.expr) = result;
      delete (yyvsp[(2) - (4)].bindings_with_types); 
  ;}
    break;

  case 54:
#line 538 "src/Parser/parser.y"
    {
      auto letExpr = new LetExpr(std::string((yyvsp[(2) - (8)].str)), ExprPtr((yyvsp[(6) - (8)].expr)), 
                      std::make_unique<ExprStmt>(ExprPtr((yyvsp[(8) - (8)].expr))), *(yyvsp[(4) - (8)].type_info));
      setExprLocation(letExpr, (yylsp[(1) - (8)]));
      (yyval.expr) = letExpr;
      delete (yyvsp[(4) - (8)].type_info);
      free((yyvsp[(2) - (8)].str));
  ;}
    break;

  case 55:
#line 546 "src/Parser/parser.y"
    {
      auto letExpr = new LetExpr(std::string((yyvsp[(2) - (6)].str)), ExprPtr((yyvsp[(4) - (6)].expr)), 
                      std::make_unique<ExprStmt>(ExprPtr((yyvsp[(6) - (6)].expr))));
      setExprLocation(letExpr, (yylsp[(1) - (6)]));
      (yyval.expr) = letExpr;
      free((yyvsp[(2) - (6)].str));
  ;}
    break;

  case 57:
#line 555 "src/Parser/parser.y"
    { 
      auto block = ExprPtr(new ExprBlock(std::move(*(yyvsp[(4) - (5)].stmts))));
      auto whileExpr = new WhileExpr(ExprPtr((yyvsp[(2) - (5)].expr)), std::move(block));
      setExprLocation(whileExpr, (yylsp[(1) - (5)]));
      (yyval.expr) = whileExpr; 
      delete (yyvsp[(4) - (5)].stmts);
  ;}
    break;

  case 58:
#line 562 "src/Parser/parser.y"
    {
      auto forExpr = new ForExpr(std::string((yyvsp[(3) - (7)].str)), ExprPtr((yyvsp[(5) - (7)].expr)), ExprPtr((yyvsp[(7) - (7)].expr)));
      setExprLocation(forExpr, (yylsp[(1) - (7)]));
      (yyval.expr) = forExpr;
      free((yyvsp[(3) - (7)].str));
  ;}
    break;

  case 59:
#line 568 "src/Parser/parser.y"
    {
      auto block = ExprPtr(new ExprBlock(std::move(*(yyvsp[(8) - (9)].stmts))));
      auto forExpr = new ForExpr(std::string((yyvsp[(3) - (9)].str)), ExprPtr((yyvsp[(5) - (9)].expr)), std::move(block));
      setExprLocation(forExpr, (yylsp[(1) - (9)]));
      (yyval.expr) = forExpr;
      free((yyvsp[(3) - (9)].str));
      delete (yyvsp[(8) - (9)].stmts);
  ;}
    break;

  case 60:
#line 576 "src/Parser/parser.y"
    { 
      auto block = new ExprBlock(std::move(*(yyvsp[(2) - (3)].stmts)));
      setExprLocation(block, (yylsp[(1) - (3)]));
      (yyval.expr) = block; 
      delete (yyvsp[(2) - (3)].stmts);
  ;}
    break;

  case 61:
#line 582 "src/Parser/parser.y"
    { 
      setExprLocation((yyvsp[(2) - (3)].expr), (yylsp[(2) - (3)]));
      (yyval.expr) = (yyvsp[(2) - (3)].expr); 
  ;}
    break;

  case 62:
#line 586 "src/Parser/parser.y"
    {
        auto getAttr = new GetAttrExpr(ExprPtr((yyvsp[(1) - (3)].expr)), (yyvsp[(3) - (3)].str));
        setExprLocation(getAttr, (yylsp[(2) - (3)]));
        (yyval.expr) = getAttr;
        free((yyvsp[(3) - (3)].str));
    ;}
    break;

  case 63:
#line 592 "src/Parser/parser.y"
    {
        auto setAttr = new SetAttrExpr(ExprPtr((yyvsp[(1) - (5)].expr)), (yyvsp[(3) - (5)].str), ExprPtr((yyvsp[(5) - (5)].expr)));
        setExprLocation(setAttr, (yylsp[(2) - (5)]));
        (yyval.expr) = setAttr;
        free((yyvsp[(3) - (5)].str));
    ;}
    break;

  case 64:
#line 598 "src/Parser/parser.y"
    {
        auto methodCall = new MethodCallExpr(ExprPtr((yyvsp[(1) - (6)].expr)), (yyvsp[(3) - (6)].str), std::move(*(yyvsp[(5) - (6)].expr_list)));
        setExprLocation(methodCall, (yylsp[(2) - (6)]));
        (yyval.expr) = methodCall;
        delete (yyvsp[(5) - (6)].expr_list);
        free((yyvsp[(3) - (6)].str));
    ;}
    break;

  case 65:
#line 605 "src/Parser/parser.y"
    {
        auto selfExpr = new SelfExpr();
        setExprLocation(selfExpr, (yylsp[(1) - (1)]));
        (yyval.expr) = selfExpr;
    ;}
    break;

  case 66:
#line 610 "src/Parser/parser.y"
    {
        auto assignExpr = new AssignExpr("self", ExprPtr((yyvsp[(3) - (3)].expr)));
        setExprLocation(assignExpr, (yylsp[(1) - (3)]));
        (yyval.expr) = assignExpr;
    ;}
    break;

  case 67:
#line 615 "src/Parser/parser.y"
    {
        auto baseCall = new BaseCallExpr({});
        setExprLocation(baseCall, (yylsp[(1) - (3)]));
        (yyval.expr) = baseCall;
    ;}
    break;

  case 68:
#line 620 "src/Parser/parser.y"
    {
        auto assignExpr = new AssignExpr(std::string((yyvsp[(1) - (3)].str)), ExprPtr((yyvsp[(3) - (3)].expr)));
        setExprLocation(assignExpr, (yylsp[(1) - (3)]));
        (yyval.expr) = assignExpr;
        free((yyvsp[(1) - (3)].str));
    ;}
    break;

  case 69:
#line 632 "src/Parser/parser.y"
    {
        auto ifExpr = new IfExpr(ExprPtr((yyvsp[(3) - (7)].expr)), ExprPtr((yyvsp[(5) - (7)].expr)), ExprPtr((yyvsp[(7) - (7)].expr)));
        setExprLocation(ifExpr, (yylsp[(1) - (7)]));
        (yyval.expr) = ifExpr;
    ;}
    break;

  case 70:
#line 637 "src/Parser/parser.y"
    {
        // Transformar: if (cond1) expr1 elif (cond2) expr2 else expr3
        // A: if (cond1) expr1 else if (cond2) expr2 else expr3
        auto nestedIf = new IfExpr(ExprPtr((yyvsp[(8) - (12)].expr)), ExprPtr((yyvsp[(10) - (12)].expr)), ExprPtr((yyvsp[(12) - (12)].expr)));
        auto ifExpr = new IfExpr(ExprPtr((yyvsp[(3) - (12)].expr)), ExprPtr((yyvsp[(5) - (12)].expr)), ExprPtr(nestedIf));
        setExprLocation(ifExpr, (yylsp[(1) - (12)]));
        (yyval.expr) = ifExpr;
    ;}
    break;

  case 71:
#line 645 "src/Parser/parser.y"
    {
        // Transformar: if (cond1) expr1 elif (cond2) expr2
        // A: if (cond1) expr1 else if (cond2) expr2
        auto nestedIf = new IfExpr(ExprPtr((yyvsp[(8) - (10)].expr)), ExprPtr((yyvsp[(10) - (10)].expr)), nullptr);
        auto ifExpr = new IfExpr(ExprPtr((yyvsp[(3) - (10)].expr)), ExprPtr((yyvsp[(5) - (10)].expr)), ExprPtr(nestedIf));
        setExprLocation(ifExpr, (yylsp[(1) - (10)]));
        (yyval.expr) = ifExpr;
    ;}
    break;

  case 72:
#line 653 "src/Parser/parser.y"
    { 
      auto ifBlock = ExprPtr(new ExprBlock(std::move(*(yyvsp[(6) - (11)].stmts))));
      auto elseBlock = ExprPtr(new ExprBlock(std::move(*(yyvsp[(10) - (11)].stmts))));
      auto ifExpr = new IfExpr(ExprPtr((yyvsp[(3) - (11)].expr)), std::move(ifBlock), std::move(elseBlock));
      setExprLocation(ifExpr, (yylsp[(1) - (11)]));
      (yyval.expr) = ifExpr; 
      delete (yyvsp[(6) - (11)].stmts);
      delete (yyvsp[(10) - (11)].stmts);
  ;}
    break;

  case 73:
#line 662 "src/Parser/parser.y"
    { 
      auto ifBlock = ExprPtr(new ExprBlock(std::move(*(yyvsp[(6) - (7)].stmts))));
      auto ifExpr = new IfExpr(ExprPtr((yyvsp[(3) - (7)].expr)), std::move(ifBlock), nullptr);
      setExprLocation(ifExpr, (yylsp[(1) - (7)]));
      (yyval.expr) = ifExpr; 
      delete (yyvsp[(6) - (7)].stmts);
  ;}
    break;

  case 74:
#line 669 "src/Parser/parser.y"
    {
      // Transformar: if (cond1) { body1 } elif (cond2) { body2 } else { body3 }
      // A: if (cond1) { body1 } else if (cond2) { body2 } else { body3 }
      auto elifBlock = ExprPtr(new ExprBlock(std::move(*(yyvsp[(13) - (18)].stmts))));
      auto elseBlock = ExprPtr(new ExprBlock(std::move(*(yyvsp[(17) - (18)].stmts))));
      auto nestedIf = new IfExpr(ExprPtr((yyvsp[(10) - (18)].expr)), std::move(elifBlock), std::move(elseBlock));
      
      auto ifBlock = ExprPtr(new ExprBlock(std::move(*(yyvsp[(6) - (18)].stmts))));
      auto ifExpr = new IfExpr(ExprPtr((yyvsp[(3) - (18)].expr)), std::move(ifBlock), ExprPtr(nestedIf));
      setExprLocation(ifExpr, (yylsp[(1) - (18)]));
      (yyval.expr) = ifExpr;
      delete (yyvsp[(6) - (18)].stmts);
      delete (yyvsp[(13) - (18)].stmts);
      delete (yyvsp[(17) - (18)].stmts);
  ;}
    break;

  case 75:
#line 684 "src/Parser/parser.y"
    {
      // Transformar: if (cond1) { body1 } elif (cond2) { body2 }
      // A: if (cond1) { body1 } else if (cond2) { body2 }
      auto elifBlock = ExprPtr(new ExprBlock(std::move(*(yyvsp[(13) - (14)].stmts))));
      auto nestedIf = new IfExpr(ExprPtr((yyvsp[(10) - (14)].expr)), std::move(elifBlock), nullptr);
      
      auto ifBlock = ExprPtr(new ExprBlock(std::move(*(yyvsp[(6) - (14)].stmts))));
      auto ifExpr = new IfExpr(ExprPtr((yyvsp[(3) - (14)].expr)), std::move(ifBlock), ExprPtr(nestedIf));
      setExprLocation(ifExpr, (yylsp[(1) - (14)]));
      (yyval.expr) = ifExpr;
      delete (yyvsp[(6) - (14)].stmts);
      delete (yyvsp[(13) - (14)].stmts);
  ;}
    break;

  case 76:
#line 701 "src/Parser/parser.y"
    { (yyval.expr_list) = new std::vector<ExprPtr>(); ;}
    break;

  case 77:
#line 703 "src/Parser/parser.y"
    {
        (yyval.expr_list) = new std::vector<ExprPtr>();
        (yyval.expr_list)->emplace_back(ExprPtr((yyvsp[(1) - (1)].expr)));
      ;}
    break;

  case 78:
#line 708 "src/Parser/parser.y"
    {
        (yyvsp[(1) - (3)].expr_list)->emplace_back(ExprPtr((yyvsp[(3) - (3)].expr)));
        (yyval.expr_list) = (yyvsp[(1) - (3)].expr_list);
      ;}
    break;

  case 79:
#line 715 "src/Parser/parser.y"
    {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *(yyvsp[(4) - (13)].param_list)) {
            params.push_back(param.first);
            param_types.push_back(param.second);
        }
        auto typeDecl = static_cast<Stmt*>(new TypeDecl(
            std::string((yyvsp[(2) - (13)].str)),
            std::move(params),
            std::move(param_types),
            std::move((yyvsp[(12) - (13)].mems)->first),
            std::move((yyvsp[(12) - (13)].mems)->second),
            std::string((yyvsp[(7) - (13)].str)),
            std::move(*(yyvsp[(9) - (13)].expr_list))
        ));
        setStmtLocation(typeDecl, (yylsp[(1) - (13)]));
        (yyval.stmt) = typeDecl;
        delete (yyvsp[(4) - (13)].param_list); delete (yyvsp[(9) - (13)].expr_list); delete (yyvsp[(12) - (13)].mems);
        free((yyvsp[(2) - (13)].str)); free((yyvsp[(7) - (13)].str));
    ;}
    break;

  case 80:
#line 736 "src/Parser/parser.y"
    {
        auto typeDecl = static_cast<Stmt*>(new TypeDecl(
            std::string((yyvsp[(2) - (10)].str)),
            {},
            {},
            std::move((yyvsp[(9) - (10)].mems)->first),
            std::move((yyvsp[(9) - (10)].mems)->second),
            std::string((yyvsp[(4) - (10)].str)),
            std::move(*(yyvsp[(6) - (10)].expr_list))
        ));
        setStmtLocation(typeDecl, (yylsp[(1) - (10)]));
        (yyval.stmt) = typeDecl;
        delete (yyvsp[(6) - (10)].expr_list); delete (yyvsp[(9) - (10)].mems);
        free((yyvsp[(2) - (10)].str)); free((yyvsp[(4) - (10)].str));
    ;}
    break;

  case 81:
#line 751 "src/Parser/parser.y"
    {
        auto typeDecl = static_cast<Stmt*>(new TypeDecl(
            std::string((yyvsp[(2) - (7)].str)),
            {}, // sin parámetros propios
            {},
            std::move((yyvsp[(6) - (7)].mems)->first),
            std::move((yyvsp[(6) - (7)].mems)->second),
            std::string((yyvsp[(4) - (7)].str)), // tipo base
            {} // sin baseArgs explícitos: herencia implícita
        ));
        setStmtLocation(typeDecl, (yylsp[(1) - (7)]));
        (yyval.stmt) = typeDecl;
        delete (yyvsp[(6) - (7)].mems);
        free((yyvsp[(2) - (7)].str)); free((yyvsp[(4) - (7)].str));
    ;}
    break;

  case 82:
#line 766 "src/Parser/parser.y"
    {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *(yyvsp[(4) - (8)].param_list)) {
            params.push_back(param.first);
            param_types.push_back(param.second);
        }
        auto typeDecl = static_cast<Stmt*>(new TypeDecl(
            std::string((yyvsp[(2) - (8)].str)),
            std::move(params),
            std::move(param_types),
            std::move((yyvsp[(7) - (8)].mems)->first),
            std::move((yyvsp[(7) - (8)].mems)->second)
        ));
        setStmtLocation(typeDecl, (yylsp[(1) - (8)]));
        (yyval.stmt) = typeDecl;
        delete (yyvsp[(4) - (8)].param_list); delete (yyvsp[(7) - (8)].mems);
        free((yyvsp[(2) - (8)].str));
    ;}
    break;

  case 83:
#line 785 "src/Parser/parser.y"
    {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *(yyvsp[(4) - (10)].param_list)) {
            params.push_back(param.first);
            param_types.push_back(param.second);
        }
        auto typeDecl = static_cast<Stmt*>(new TypeDecl(
            std::string((yyvsp[(2) - (10)].str)),
            std::move(params),
            std::move(param_types),
            std::move((yyvsp[(9) - (10)].mems)->first),
            std::move((yyvsp[(9) - (10)].mems)->second),
            std::string((yyvsp[(7) - (10)].str)),
            {} // sin baseArgs
        ));
        setStmtLocation(typeDecl, (yylsp[(1) - (10)]));
        (yyval.stmt) = typeDecl;
        delete (yyvsp[(4) - (10)].param_list); delete (yyvsp[(9) - (10)].mems);
        free((yyvsp[(2) - (10)].str)); free((yyvsp[(7) - (10)].str));
    ;}
    break;

  case 84:
#line 806 "src/Parser/parser.y"
    {
        auto typeDecl = static_cast<Stmt*>(new TypeDecl(
            std::string((yyvsp[(2) - (5)].str)),
            {},
            {},
            std::move((yyvsp[(4) - (5)].mems)->first),
            std::move((yyvsp[(4) - (5)].mems)->second)
        ));
        setStmtLocation(typeDecl, (yylsp[(1) - (5)]));
        (yyval.stmt) = typeDecl;
        delete (yyvsp[(4) - (5)].mems); free((yyvsp[(2) - (5)].str));
    ;}
    break;

  case 85:
#line 821 "src/Parser/parser.y"
    { 
        auto* pair = new std::pair<
            std::vector<std::unique_ptr<AttributeDecl>>,
            std::vector<std::unique_ptr<MethodDecl>>
        >();
        (yyval.mems) = pair;
    ;}
    break;

  case 86:
#line 828 "src/Parser/parser.y"
    {
        if ((yyvsp[(2) - (2)].member_pair)->first) {
            (yyvsp[(1) - (2)].mems)->first.push_back(std::unique_ptr<AttributeDecl>((yyvsp[(2) - (2)].member_pair)->first));
        } else {
            (yyvsp[(1) - (2)].mems)->second.push_back(std::unique_ptr<MethodDecl>((yyvsp[(2) - (2)].member_pair)->second));
        }
        delete (yyvsp[(2) - (2)].member_pair);
        (yyval.mems) = (yyvsp[(1) - (2)].mems);
    ;}
    break;

  case 87:
#line 840 "src/Parser/parser.y"
    {
        auto* pair = new std::pair<AttributeDecl*, MethodDecl*>();
        pair->first = (yyvsp[(1) - (2)].attribute_decl);
        (yyval.member_pair) = pair;
    ;}
    break;

  case 88:
#line 845 "src/Parser/parser.y"
    {
        auto* pair = new std::pair<AttributeDecl*, MethodDecl*>();
        pair->second = (yyvsp[(1) - (2)].method_decl);
        (yyval.member_pair) = pair;
    ;}
    break;

  case 89:
#line 853 "src/Parser/parser.y"
    {
        auto attrDecl = static_cast<AttributeDecl*>(new AttributeDecl((yyvsp[(1) - (3)].str), ExprPtr((yyvsp[(3) - (3)].expr))));
        setStmtLocation(attrDecl, (yylsp[(1) - (3)]));
        (yyval.attribute_decl) = attrDecl;
        free((yyvsp[(1) - (3)].str));
    ;}
    break;

  case 90:
#line 859 "src/Parser/parser.y"
    {
        auto attrDecl = static_cast<AttributeDecl*>(new AttributeDecl((yyvsp[(1) - (5)].str), ExprPtr((yyvsp[(5) - (5)].expr)), *(yyvsp[(3) - (5)].type_info)));
        setStmtLocation(attrDecl, (yylsp[(1) - (5)]));
        (yyval.attribute_decl) = attrDecl;
        free((yyvsp[(1) - (5)].str));
        delete (yyvsp[(3) - (5)].type_info);
    ;}
    break;

  case 91:
#line 869 "src/Parser/parser.y"
    {
        std::vector<std::string> args = (yyvsp[(3) - (6)].str_list) ? std::move(*(yyvsp[(3) - (6)].str_list)) : std::vector<std::string>();
        delete (yyvsp[(3) - (6)].str_list);
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl((yyvsp[(1) - (6)].str), std::move(args), StmtPtr(new ExprStmt(ExprPtr((yyvsp[(6) - (6)].expr))))));
        setStmtLocation(methodDecl, (yylsp[(1) - (6)]));
        (yyval.method_decl) = methodDecl;
        free((yyvsp[(1) - (6)].str));
    ;}
    break;

  case 92:
#line 877 "src/Parser/parser.y"
    {
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *(yyvsp[(3) - (6)].param_list)) {
            args.push_back(param.first);
            param_types.push_back(param.second);
        }
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl((yyvsp[(1) - (6)].str), std::move(args), StmtPtr(new ExprStmt(ExprPtr((yyvsp[(6) - (6)].expr)))), std::move(param_types)));
        setStmtLocation(methodDecl, (yylsp[(1) - (6)]));
        (yyval.method_decl) = methodDecl;
        delete (yyvsp[(3) - (6)].param_list);
        free((yyvsp[(1) - (6)].str));
    ;}
    break;

  case 93:
#line 890 "src/Parser/parser.y"
    {
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *(yyvsp[(3) - (8)].param_list)) {
            args.push_back(param.first);
            param_types.push_back(param.second);
        }
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl((yyvsp[(1) - (8)].str), std::move(args), StmtPtr(new ExprStmt(ExprPtr((yyvsp[(8) - (8)].expr)))), std::move(param_types), *(yyvsp[(6) - (8)].type_info)));
        setStmtLocation(methodDecl, (yylsp[(1) - (8)]));
        (yyval.method_decl) = methodDecl;
        delete (yyvsp[(3) - (8)].param_list);
        delete (yyvsp[(6) - (8)].type_info);
        free((yyvsp[(1) - (8)].str));
    ;}
    break;

  case 94:
#line 904 "src/Parser/parser.y"
    {
        std::vector<std::string> args = (yyvsp[(3) - (7)].str_list) ? std::move(*(yyvsp[(3) - (7)].str_list)) : std::vector<std::string>();
        delete (yyvsp[(3) - (7)].str_list);
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*(yyvsp[(6) - (7)].stmts));
        delete (yyvsp[(6) - (7)].stmts);
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl((yyvsp[(1) - (7)].str), std::move(args), StmtPtr(std::move(block))));
        setStmtLocation(methodDecl, (yylsp[(1) - (7)]));
        (yyval.method_decl) = methodDecl;
        free((yyvsp[(1) - (7)].str));
    ;}
    break;

  case 95:
#line 915 "src/Parser/parser.y"
    {
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *(yyvsp[(3) - (7)].param_list)) {
            args.push_back(param.first);
            param_types.push_back(param.second);
        }
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*(yyvsp[(6) - (7)].stmts));
        delete (yyvsp[(6) - (7)].stmts);
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl((yyvsp[(1) - (7)].str), std::move(args), StmtPtr(std::move(block)), std::move(param_types)));
        setStmtLocation(methodDecl, (yylsp[(1) - (7)]));
        (yyval.method_decl) = methodDecl;
        delete (yyvsp[(3) - (7)].param_list);
        free((yyvsp[(1) - (7)].str));
    ;}
    break;

  case 96:
#line 931 "src/Parser/parser.y"
    {
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *(yyvsp[(3) - (9)].param_list)) {
            args.push_back(param.first);
            param_types.push_back(param.second);
        }
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*(yyvsp[(8) - (9)].stmts));
        delete (yyvsp[(8) - (9)].stmts);
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl((yyvsp[(1) - (9)].str), std::move(args), StmtPtr(std::move(block)), std::move(param_types), *(yyvsp[(6) - (9)].type_info)));
        setStmtLocation(methodDecl, (yylsp[(1) - (9)]));
        (yyval.method_decl) = methodDecl;
        delete (yyvsp[(3) - (9)].param_list);
        delete (yyvsp[(6) - (9)].type_info);
        free((yyvsp[(1) - (9)].str));
    ;}
    break;

  case 97:
#line 948 "src/Parser/parser.y"
    {
        std::vector<std::string> args;
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl((yyvsp[(1) - (5)].str), std::move(args), StmtPtr(new ExprStmt(ExprPtr((yyvsp[(5) - (5)].expr))))));
        setStmtLocation(methodDecl, (yylsp[(1) - (5)]));
        (yyval.method_decl) = methodDecl;
        free((yyvsp[(1) - (5)].str));
    ;}
    break;

  case 98:
#line 955 "src/Parser/parser.y"
    {
        std::vector<std::string> args;
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl((yyvsp[(1) - (7)].str), std::move(args), StmtPtr(new ExprStmt(ExprPtr((yyvsp[(7) - (7)].expr)))), {}, *(yyvsp[(5) - (7)].type_info)));
        setStmtLocation(methodDecl, (yylsp[(1) - (7)]));
        (yyval.method_decl) = methodDecl;
        delete (yyvsp[(5) - (7)].type_info);
        free((yyvsp[(1) - (7)].str));
    ;}
    break;

  case 99:
#line 963 "src/Parser/parser.y"
    {
        std::vector<std::string> args;
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*(yyvsp[(5) - (6)].stmts));
        delete (yyvsp[(5) - (6)].stmts);
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl((yyvsp[(1) - (6)].str), std::move(args), StmtPtr(std::move(block))));
        setStmtLocation(methodDecl, (yylsp[(1) - (6)]));
        (yyval.method_decl) = methodDecl;
        free((yyvsp[(1) - (6)].str));
    ;}
    break;

  case 100:
#line 973 "src/Parser/parser.y"
    {
        std::vector<std::string> args;
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*(yyvsp[(7) - (8)].stmts));
        delete (yyvsp[(7) - (8)].stmts);
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl((yyvsp[(1) - (8)].str), std::move(args), StmtPtr(std::move(block)), {}, *(yyvsp[(5) - (8)].type_info)));
        setStmtLocation(methodDecl, (yylsp[(1) - (8)]));
        (yyval.method_decl) = methodDecl;
        delete (yyvsp[(5) - (8)].type_info);
        free((yyvsp[(1) - (8)].str));
    ;}
    break;

  case 101:
#line 987 "src/Parser/parser.y"
    { 
        // Si es un tipo conocido, usarlo, sino usar Object
        std::string type_name((yyvsp[(1) - (1)].str));
        if (type_name == "Number") {
            (yyval.type_info) = new std::shared_ptr<TypeInfo>(std::make_shared<TypeInfo>(TypeInfo::Number()));
        } else if (type_name == "Boolean") {
            (yyval.type_info) = new std::shared_ptr<TypeInfo>(std::make_shared<TypeInfo>(TypeInfo::Boolean()));
        } else if (type_name == "String") {
            (yyval.type_info) = new std::shared_ptr<TypeInfo>(std::make_shared<TypeInfo>(TypeInfo::String()));
        } else {
            // Para cualquier otro tipo, usar Object y guardar el nombre del tipo
            (yyval.type_info) = new std::shared_ptr<TypeInfo>(std::make_shared<TypeInfo>(TypeInfo::Object(type_name)));
        }
        free((yyvsp[(1) - (1)].str));
    ;}
    break;

  case 102:
#line 1005 "src/Parser/parser.y"
    { (yyval.param_list) = new std::vector<std::pair<std::string, std::shared_ptr<TypeInfo>>>(); ;}
    break;

  case 103:
#line 1006 "src/Parser/parser.y"
    { 
        (yyval.param_list) = new std::vector<std::pair<std::string, std::shared_ptr<TypeInfo>>>();
        (yyval.param_list)->push_back(*(yyvsp[(1) - (1)].param_with_type));
        delete (yyvsp[(1) - (1)].param_with_type);
    ;}
    break;

  case 104:
#line 1011 "src/Parser/parser.y"
    {
        (yyvsp[(1) - (3)].param_list)->push_back(*(yyvsp[(3) - (3)].param_with_type));
        delete (yyvsp[(3) - (3)].param_with_type);
        (yyval.param_list) = (yyvsp[(1) - (3)].param_list);
    ;}
    break;

  case 105:
#line 1019 "src/Parser/parser.y"
    { 
        (yyval.param_with_type) = new std::pair<std::string, std::shared_ptr<TypeInfo>>(
            std::string((yyvsp[(1) - (1)].str)), 
            std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown)
        );
        free((yyvsp[(1) - (1)].str));
    ;}
    break;

  case 106:
#line 1026 "src/Parser/parser.y"
    {
        (yyval.param_with_type) = new std::pair<std::string, std::shared_ptr<TypeInfo>>(
            std::string((yyvsp[(1) - (3)].str)), 
            *(yyvsp[(3) - (3)].type_info)
        );
        free((yyvsp[(1) - (3)].str));
        delete (yyvsp[(3) - (3)].type_info);
    ;}
    break;


/* Line 1267 of yacc.c.  */
#line 3196 "src/Parser/parser.tab.cpp"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval, &yylloc);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  yyerror_range[0] = yylsp[1-yylen];
  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the look-ahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 1036 "src/Parser/parser.y"

