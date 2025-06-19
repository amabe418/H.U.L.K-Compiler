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
     TRUE = 276,
     FALSE = 277,
     NUMBER = 278,
     STRING = 279,
     PLUS = 280,
     MINUS = 281,
     MULT = 282,
     DIV = 283,
     MOD = 284,
     POW = 285,
     CONCAT = 286,
     CONCAT_WS = 287,
     LE = 288,
     GE = 289,
     EQ = 290,
     NEQ = 291,
     LESS_THAN = 292,
     GREATER_THAN = 293,
     OR = 294,
     AND = 295,
     LPAREN = 296,
     RPAREN = 297,
     LBRACE = 298,
     RBRACE = 299,
     COMMA = 300,
     SEMICOLON = 301,
     COLON = 302,
     INT = 303,
     FLOAT = 304,
     BOOL = 305,
     VOID = 306,
     UMINUS = 307
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
#define TRUE 276
#define FALSE 277
#define NUMBER 278
#define STRING 279
#define PLUS 280
#define MINUS 281
#define MULT 282
#define DIV 283
#define MOD 284
#define POW 285
#define CONCAT 286
#define CONCAT_WS 287
#define LE 288
#define GE 289
#define EQ 290
#define NEQ 291
#define LESS_THAN 292
#define GREATER_THAN 293
#define OR 294
#define AND 295
#define LPAREN 296
#define RPAREN 297
#define LBRACE 298
#define RBRACE 299
#define COMMA 300
#define SEMICOLON 301
#define COLON 302
#define INT 303
#define FLOAT 304
#define BOOL 305
#define VOID 306
#define UMINUS 307




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
      // Asegurarnos de que la línea no sea mayor que el número real de líneas
      int line = locp->first_line;
      if (line < 1) line = 1;
      
      // Obtener el número de línea actual del lexer
      extern int yylineno;
      if (line > yylineno) {
          line = yylineno;
      }
      
      std::fprintf(stderr,
          "Error de parseo en línea %d, columna %d: %s\n",
          line,
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
    char* str;
}
/* Line 193 of yacc.c.  */
#line 297 "src/Parser/parser.tab.cpp"
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
#line 322 "src/Parser/parser.tab.cpp"

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
#define YYLAST   816

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  53
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  19
/* YYNRULES -- Number of rules.  */
#define YYNRULES  86
/* YYNRULES -- Number of states.  */
#define YYNSTATES  192

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   307

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
      45,    46,    47,    48,    49,    50,    51,    52
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     6,    10,    14,    16,    20,    24,
      26,    37,    46,    54,    61,    63,    64,    68,    72,    73,
      75,    79,    81,    83,    85,    87,    89,    93,    97,   101,
     105,   109,   113,   117,   121,   125,   129,   133,   137,   141,
     145,   149,   153,   156,   161,   165,   171,   176,   183,   189,
     194,   198,   203,   205,   211,   215,   219,   223,   229,   236,
     238,   242,   250,   262,   270,   271,   273,   277,   291,   302,
     310,   319,   325,   326,   329,   332,   335,   339,   346,   354,
     360,   361,   363,   364,   366,   370,   372
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      54,     0,    -1,    55,    -1,    -1,    55,    58,    46,    -1,
      55,    61,    46,    -1,    57,    -1,    56,    45,    57,    -1,
       7,    13,    61,    -1,    61,    -1,    11,     7,    41,    70,
      42,    47,    69,    43,    59,    44,    -1,    11,     7,    41,
      70,    42,    43,    59,    44,    -1,    11,     7,    41,    70,
      42,    12,    61,    -1,    11,     7,    41,    42,    12,    61,
      -1,    64,    -1,    -1,    59,    58,    46,    -1,    59,    61,
      46,    -1,    -1,     7,    -1,    60,    45,     7,    -1,    23,
      -1,    24,    -1,    21,    -1,    22,    -1,     7,    -1,    61,
      25,    61,    -1,    61,    26,    61,    -1,    61,    27,    61,
      -1,    61,    28,    61,    -1,    61,    29,    61,    -1,    61,
      30,    61,    -1,    61,    31,    61,    -1,    61,    32,    61,
      -1,    61,    37,    61,    -1,    61,    38,    61,    -1,    61,
      33,    61,    -1,    61,    34,    61,    -1,    61,    35,    61,
      -1,    61,    36,    61,    -1,    61,    40,    61,    -1,    61,
      39,    61,    -1,    26,    61,    -1,     7,    41,    63,    42,
      -1,     7,    41,    42,    -1,    18,     7,    41,    63,    42,
      -1,    18,     7,    41,    42,    -1,    61,    16,     7,    41,
      63,    42,    -1,    61,    16,     7,    41,    42,    -1,    20,
      41,    63,    42,    -1,    20,    41,    42,    -1,     3,    56,
       4,    61,    -1,    62,    -1,     5,    61,    43,    59,    44,
      -1,    43,    59,    44,    -1,    41,    61,    42,    -1,    61,
      16,     7,    -1,    61,    16,     7,    14,    61,    -1,    61,
      16,     7,    41,    63,    42,    -1,    17,    -1,    20,    41,
      42,    -1,     8,    41,    61,    42,    61,     9,    61,    -1,
       8,    41,    61,    42,    43,    59,    44,     9,    43,    59,
      44,    -1,     8,    41,    61,    42,    43,    59,    44,    -1,
      -1,    61,    -1,    63,    45,    61,    -1,    15,     7,    41,
      60,    42,    19,     7,    41,    63,    42,    43,    65,    44,
      -1,    15,     7,    19,     7,    41,    63,    42,    43,    65,
      44,    -1,    15,     7,    19,     7,    43,    65,    44,    -1,
      15,     7,    41,    60,    42,    43,    65,    44,    -1,    15,
       7,    43,    65,    44,    -1,    -1,    65,    66,    -1,    67,
      46,    -1,    68,    46,    -1,     7,    13,    61,    -1,     7,
      41,    60,    42,    12,    61,    -1,     7,    41,    60,    42,
      43,    59,    44,    -1,     7,    41,    42,    12,    61,    -1,
      -1,     7,    -1,    -1,    71,    -1,    70,    45,    71,    -1,
       7,    -1,     7,    47,    69,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   155,   155,   159,   160,   165,   173,   178,   186,   193,
     196,   219,   241,   259,   271,   277,   278,   282,   289,   290,
     291,   295,   296,   297,   298,   299,   300,   301,   302,   303,
     304,   305,   306,   307,   308,   309,   310,   311,   312,   313,
     314,   315,   316,   317,   326,   327,   336,   340,   349,   353,
     361,   364,   373,   375,   380,   384,   385,   389,   393,   398,
     401,   410,   413,   420,   429,   430,   435,   443,   455,   467,
     479,   489,   501,   508,   520,   525,   533,   540,   546,   555,
     562,   563,   581,   582,   587,   595,   602
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "LET", "IN", "WHILE", "FOR", "IDENT",
  "IF", "ELSE", "ELIF", "FUNCTION", "ARROW", "ASSIGN", "ASSIGN_DESTRUCT",
  "TYPE", "DOT", "SELF", "NEW", "INHERITS", "BASE", "TRUE", "FALSE",
  "NUMBER", "STRING", "PLUS", "MINUS", "MULT", "DIV", "MOD", "POW",
  "CONCAT", "CONCAT_WS", "LE", "GE", "EQ", "NEQ", "LESS_THAN",
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
     305,   306,   307
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    53,    54,    55,    55,    55,    56,    56,    57,    58,
      58,    58,    58,    58,    58,    59,    59,    59,    60,    60,
      60,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    62,    62,    62,    63,    63,    63,    64,    64,    64,
      64,    64,    65,    65,    66,    66,    67,    68,    68,    68,
      69,    69,    70,    70,    70,    71,    71
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     3,     3,     1,     3,     3,     1,
      10,     8,     7,     6,     1,     0,     3,     3,     0,     1,
       3,     1,     1,     1,     1,     1,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     4,     3,     5,     4,     6,     5,     4,
       3,     4,     1,     5,     3,     3,     3,     5,     6,     1,
       3,     7,    11,     7,     0,     1,     3,    13,    10,     7,
       8,     5,     0,     2,     2,     2,     3,     6,     7,     5,
       0,     1,     0,     1,     3,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     2,     1,     0,     0,    25,     0,     0,     0,
      59,     0,     0,    23,    24,    21,    22,     0,     0,    15,
       0,     0,    52,    14,     0,     0,     6,     0,    64,     0,
       0,     0,     0,    64,    42,     0,     0,     4,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     5,     0,     0,     0,    15,
      44,    65,     0,     0,    82,     0,    18,    72,    64,    50,
       0,    55,    54,     0,     0,    56,    26,    27,    28,    29,
      30,    31,    32,    33,    36,    37,    38,    39,    34,    35,
      41,    40,     8,    51,     7,     0,    43,     0,     0,    85,
       0,     0,    83,     0,    19,     0,     0,    46,     0,    49,
      16,    17,     0,    64,    53,    66,    15,     0,    80,     0,
       0,     0,    64,    72,     0,     0,     0,    71,    73,     0,
       0,    45,    57,    48,     0,     0,     0,    81,    86,    13,
       0,    15,    80,    84,     0,     0,     0,    72,    20,     0,
      18,    74,    75,    47,    54,    61,    12,     0,     0,     0,
      69,     0,     0,    76,     0,     0,     0,    11,    15,    72,
      64,    70,     0,     0,    15,     0,     0,     0,    79,     0,
      15,     0,    10,    68,     0,    77,     0,    62,    72,    78,
       0,    67
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    25,    26,    73,    36,   105,    74,    22,
      62,    23,   106,   128,   129,   130,   138,   101,   102
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -114
static const yytype_int16 yypact[] =
{
    -114,     1,   357,  -114,     0,   492,   -28,    53,    82,    84,
    -114,    96,    63,  -114,  -114,  -114,  -114,   492,   492,  -114,
     -26,   563,  -114,  -114,    92,    13,  -114,   613,   384,   492,
      65,    58,    67,   411,    93,   638,   128,  -114,   105,   492,
     492,   492,   492,   492,   492,   492,   492,   492,   492,   492,
     492,   492,   492,   492,   492,  -114,   492,   492,     0,  -114,
    -114,   688,   -36,   663,    22,   107,   109,  -114,   438,  -114,
     -34,  -114,  -114,    72,   588,    56,    99,    99,   -11,   -11,
     -11,   -11,   778,   778,   137,   137,   759,   759,   137,   137,
     713,   736,   688,   688,  -114,   171,  -114,   492,   519,    74,
     110,   -21,  -114,   -29,  -114,    36,    15,  -114,    37,  -114,
    -114,  -114,   492,   465,  -114,   688,  -114,   538,   116,   492,
      20,   117,   492,  -114,    57,   123,    61,  -114,  -114,    86,
      91,  -114,   688,  -114,    38,   202,   492,  -114,  -114,   688,
     492,  -114,   116,  -114,    42,    16,   133,  -114,  -114,   492,
      50,  -114,  -114,  -114,    26,   688,   688,   233,    98,   101,
    -114,   115,    18,   688,   130,    43,   114,  -114,  -114,  -114,
     492,  -114,   492,   -10,  -114,   264,    21,    48,   688,   492,
    -114,   295,  -114,  -114,   118,   688,   326,  -114,  -114,  -114,
      29,  -114
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -114,  -114,  -114,  -114,   100,   157,   -55,    10,    -2,  -114,
     -15,  -114,  -113,  -114,  -114,  -114,    31,  -114,    54
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -64
static const yytype_int16 yytable[] =
{
      21,     3,   179,    27,    95,    38,    96,    24,   109,    97,
     145,    97,   122,    28,   123,    34,    35,    57,    70,    44,
      37,   120,   126,   126,   121,   126,    61,    63,   126,    99,
     -63,    61,   140,   180,   162,   166,   126,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,   108,    92,    93,   176,   104,    58,   127,
     160,   135,   171,   141,   100,   183,    61,   142,   -63,   -63,
     112,   -63,   -63,   191,   149,   190,   146,    65,   124,   131,
     153,   125,    97,    97,   159,   173,   157,    97,   125,    30,
     184,    31,   164,    97,    29,   115,   117,   113,   134,    66,
     147,    67,   150,    32,    33,    56,    64,   144,    68,    38,
     132,    61,    75,   175,   103,    38,   104,   139,   110,   181,
      61,   118,   119,   137,    99,   186,    41,    42,    43,    44,
     148,     4,   151,     5,   155,     6,     7,   152,   156,     8,
     161,   168,   172,     9,   169,    10,    11,   163,    12,    13,
      14,    15,    16,    38,    17,   177,   170,   174,    94,    20,
     165,   188,    39,    40,    41,    42,    43,    44,    61,    18,
     178,    19,    72,   158,     4,   143,     5,   185,     6,     7,
       0,     0,     8,     0,     0,     0,     9,     0,    10,    11,
       0,    12,    13,    14,    15,    16,     0,    17,     0,     0,
       0,     0,     0,     0,     0,     4,     0,     5,     0,     6,
       7,     0,    18,     8,    19,   114,     0,     9,     0,    10,
      11,     0,    12,    13,    14,    15,    16,     0,    17,     0,
       0,     0,     0,     0,     0,     0,     4,     0,     5,     0,
       6,     7,     0,    18,     8,    19,   154,     0,     9,     0,
      10,    11,     0,    12,    13,    14,    15,    16,     0,    17,
       0,     0,     0,     0,     0,     0,     0,     4,     0,     5,
       0,     6,     7,     0,    18,     8,    19,   167,     0,     9,
       0,    10,    11,     0,    12,    13,    14,    15,    16,     0,
      17,     0,     0,     0,     0,     0,     0,     0,     4,     0,
       5,     0,     6,     7,     0,    18,     8,    19,   182,     0,
       9,     0,    10,    11,     0,    12,    13,    14,    15,    16,
       0,    17,     0,     0,     0,     0,     0,     0,     0,     4,
       0,     5,     0,     6,     7,     0,    18,     8,    19,   187,
       0,     9,     0,    10,    11,     0,    12,    13,    14,    15,
      16,     0,    17,     0,     0,     0,     0,     0,     0,     0,
       4,     0,     5,     0,     6,     7,     0,    18,     8,    19,
     189,     0,     9,     0,    10,    11,     0,    12,    13,    14,
      15,    16,     0,    17,     0,     0,     0,     4,     0,     5,
       0,     6,     7,     0,     0,     0,     0,     0,    18,     0,
      19,    10,    11,     0,    12,    13,    14,    15,    16,     0,
      17,     0,     0,     0,     4,     0,     5,     0,     6,     7,
       0,     0,     0,     0,     0,    18,    60,    19,    10,    11,
       0,    12,    13,    14,    15,    16,     0,    17,     0,     0,
       0,     4,     0,     5,     0,     6,     7,     0,     0,     0,
       0,     0,    18,    69,    19,    10,    11,     0,    12,    13,
      14,    15,    16,     0,    17,     0,     0,     0,     4,     0,
       5,     0,     6,     7,     0,     0,     0,     0,     0,    18,
     107,    19,    10,    11,     0,    12,    13,    14,    15,    16,
       0,    17,     0,     0,     0,     4,     0,     5,     0,     6,
       7,     0,     0,     0,     0,     0,    18,   133,    19,    10,
      11,     0,    12,    13,    14,    15,    16,     0,    17,     0,
       0,     0,     4,     0,     5,     0,     6,     7,     0,     0,
       0,     0,     0,    18,     0,    19,    10,    11,     0,    12,
      13,    14,    15,    16,     0,    17,     0,   136,     0,     0,
       0,     0,     0,     0,    38,     0,     0,     0,     0,     0,
      18,     0,   116,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    38,
       0,     0,     0,     0,     0,     0,     0,     0,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    38,     0,     0,     0,     0,    55,
       0,     0,     0,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    38,
       0,     0,     0,     0,   111,     0,     0,     0,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    38,     0,    59,     0,     0,     0,
       0,     0,     0,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    38,
      71,     0,     0,     0,     0,     0,     0,     0,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    38,    98,     0,     0,     0,     0,
       0,     0,     0,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    38,
       0,     0,     0,     0,     0,     0,     0,     0,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    38,    54,     0,     0,     0,     0,     0,     0,
       0,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    38,     0,     0,     0,     0,
       0,     0,     0,     0,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    38,     0,    51,    52,     0,     0,
       0,     0,     0,    39,    40,    41,    42,    43,    44,     0,
       0,    47,    48,     0,     0,    51,    52
};

static const yytype_int16 yycheck[] =
{
       2,     0,    12,     5,    59,    16,    42,     7,    42,    45,
     123,    45,    41,    41,    43,    17,    18,     4,    33,    30,
      46,    42,     7,     7,    45,     7,    28,    29,     7,     7,
       4,    33,    12,    43,   147,     9,     7,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    68,    56,    57,   169,     7,    45,    44,
      44,   116,    44,    43,    42,    44,    68,    47,    42,    43,
      14,    45,    46,    44,    13,   188,    19,    19,    42,    42,
      42,    45,    45,    45,    42,    42,   141,    45,    45,     7,
      42,     7,    42,    45,    41,    97,    98,    41,   113,    41,
      43,    43,    41,     7,    41,    13,    41,   122,    41,    16,
     112,   113,     7,   168,     7,    16,     7,   119,    46,   174,
     122,    47,    12,     7,     7,   180,    27,    28,    29,    30,
       7,     3,    46,     5,   136,     7,     8,    46,   140,    11,
       7,    43,    12,    15,    43,    17,    18,   149,    20,    21,
      22,    23,    24,    16,    26,   170,    41,    43,    58,     2,
     150,    43,    25,    26,    27,    28,    29,    30,   170,    41,
     172,    43,    44,   142,     3,   121,     5,   179,     7,     8,
      -1,    -1,    11,    -1,    -1,    -1,    15,    -1,    17,    18,
      -1,    20,    21,    22,    23,    24,    -1,    26,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,    -1,     5,    -1,     7,
       8,    -1,    41,    11,    43,    44,    -1,    15,    -1,    17,
      18,    -1,    20,    21,    22,    23,    24,    -1,    26,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,    -1,     5,    -1,
       7,     8,    -1,    41,    11,    43,    44,    -1,    15,    -1,
      17,    18,    -1,    20,    21,    22,    23,    24,    -1,    26,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,    -1,     5,
      -1,     7,     8,    -1,    41,    11,    43,    44,    -1,    15,
      -1,    17,    18,    -1,    20,    21,    22,    23,    24,    -1,
      26,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,    -1,
       5,    -1,     7,     8,    -1,    41,    11,    43,    44,    -1,
      15,    -1,    17,    18,    -1,    20,    21,    22,    23,    24,
      -1,    26,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
      -1,     5,    -1,     7,     8,    -1,    41,    11,    43,    44,
      -1,    15,    -1,    17,    18,    -1,    20,    21,    22,    23,
      24,    -1,    26,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,    -1,     5,    -1,     7,     8,    -1,    41,    11,    43,
      44,    -1,    15,    -1,    17,    18,    -1,    20,    21,    22,
      23,    24,    -1,    26,    -1,    -1,    -1,     3,    -1,     5,
      -1,     7,     8,    -1,    -1,    -1,    -1,    -1,    41,    -1,
      43,    17,    18,    -1,    20,    21,    22,    23,    24,    -1,
      26,    -1,    -1,    -1,     3,    -1,     5,    -1,     7,     8,
      -1,    -1,    -1,    -1,    -1,    41,    42,    43,    17,    18,
      -1,    20,    21,    22,    23,    24,    -1,    26,    -1,    -1,
      -1,     3,    -1,     5,    -1,     7,     8,    -1,    -1,    -1,
      -1,    -1,    41,    42,    43,    17,    18,    -1,    20,    21,
      22,    23,    24,    -1,    26,    -1,    -1,    -1,     3,    -1,
       5,    -1,     7,     8,    -1,    -1,    -1,    -1,    -1,    41,
      42,    43,    17,    18,    -1,    20,    21,    22,    23,    24,
      -1,    26,    -1,    -1,    -1,     3,    -1,     5,    -1,     7,
       8,    -1,    -1,    -1,    -1,    -1,    41,    42,    43,    17,
      18,    -1,    20,    21,    22,    23,    24,    -1,    26,    -1,
      -1,    -1,     3,    -1,     5,    -1,     7,     8,    -1,    -1,
      -1,    -1,    -1,    41,    -1,    43,    17,    18,    -1,    20,
      21,    22,    23,    24,    -1,    26,    -1,     9,    -1,    -1,
      -1,    -1,    -1,    -1,    16,    -1,    -1,    -1,    -1,    -1,
      41,    -1,    43,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    16,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    16,    -1,    -1,    -1,    -1,    46,
      -1,    -1,    -1,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    16,
      -1,    -1,    -1,    -1,    46,    -1,    -1,    -1,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    16,    -1,    43,    -1,    -1,    -1,
      -1,    -1,    -1,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    16,
      42,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    16,    42,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    16,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    16,    40,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    16,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    16,    -1,    37,    38,    -1,    -1,
      -1,    -1,    -1,    25,    26,    27,    28,    29,    30,    -1,
      -1,    33,    34,    -1,    -1,    37,    38
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    54,    55,     0,     3,     5,     7,     8,    11,    15,
      17,    18,    20,    21,    22,    23,    24,    26,    41,    43,
      58,    61,    62,    64,     7,    56,    57,    61,    41,    41,
       7,     7,     7,    41,    61,    61,    59,    46,    16,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    46,    13,     4,    45,    43,
      42,    61,    63,    61,    41,    19,    41,    43,    41,    42,
      63,    42,    44,    58,    61,     7,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    57,    59,    42,    45,    42,     7,
      42,    70,    71,     7,     7,    60,    65,    42,    63,    42,
      46,    46,    14,    41,    44,    61,    43,    61,    47,    12,
      42,    45,    41,    43,    42,    45,     7,    44,    66,    67,
      68,    42,    61,    42,    63,    59,     9,     7,    69,    61,
      12,    43,    47,    71,    63,    65,    19,    43,     7,    13,
      41,    46,    46,    42,    44,    61,    61,    59,    69,    42,
      44,     7,    65,    61,    42,    60,     9,    44,    43,    43,
      41,    44,    12,    42,    43,    59,    65,    63,    61,    12,
      43,    59,    44,    44,    42,    61,    59,    44,    43,    44,
      65,    44
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
#line 155 "src/Parser/parser.y"
    { rootAST = (yyvsp[(1) - (1)].prog); ;}
    break;

  case 3:
#line 159 "src/Parser/parser.y"
    { (yyval.prog) = new Program(); ;}
    break;

  case 4:
#line 161 "src/Parser/parser.y"
    {
      (yyvsp[(1) - (3)].prog)->stmts.emplace_back( StmtPtr((yyvsp[(2) - (3)].stmt)) );
      (yyval.prog) = (yyvsp[(1) - (3)].prog);
    ;}
    break;

  case 5:
#line 166 "src/Parser/parser.y"
    {
      (yyvsp[(1) - (3)].prog)->stmts.emplace_back( StmtPtr(new ExprStmt(ExprPtr((yyvsp[(2) - (3)].expr)))) );
      (yyval.prog) = (yyvsp[(1) - (3)].prog);
    ;}
    break;

  case 6:
#line 173 "src/Parser/parser.y"
    {
          (yyval.bindings) = new std::vector<std::pair<std::string, Expr*>>();
          (yyval.bindings)->push_back(*(yyvsp[(1) - (1)].binding));
          delete (yyvsp[(1) - (1)].binding);
      ;}
    break;

  case 7:
#line 178 "src/Parser/parser.y"
    {
          (yyvsp[(1) - (3)].bindings)->push_back(*(yyvsp[(3) - (3)].binding));
          delete (yyvsp[(3) - (3)].binding);
          (yyval.bindings) = (yyvsp[(1) - (3)].bindings);
      ;}
    break;

  case 8:
#line 186 "src/Parser/parser.y"
    {
          (yyval.binding) = new std::pair<std::string, Expr*>(std::string((yyvsp[(1) - (3)].str)), (yyvsp[(3) - (3)].expr));
          free((yyvsp[(1) - (3)].str));
      ;}
    break;

  case 9:
#line 193 "src/Parser/parser.y"
    {
        (yyval.stmt) = (new ExprStmt(ExprPtr((yyvsp[(1) - (1)].expr))));
    ;}
    break;

  case 10:
#line 196 "src/Parser/parser.y"
    {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        
        for (const auto& param : *(yyvsp[(4) - (10)].param_list)) {
            params.push_back(param.first);
            param_types.push_back(param.second);
        }
        
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*(yyvsp[(9) - (10)].stmts));
        delete (yyvsp[(9) - (10)].stmts);
        
        (yyval.stmt) = static_cast<Stmt*>(new FunctionDecl(
            std::string((yyvsp[(2) - (10)].str)), 
            std::move(params), 
            std::move(block), 
            std::move(param_types)
        ));
        delete (yyvsp[(4) - (10)].param_list);
        delete (yyvsp[(7) - (10)].type_info);
        free((yyvsp[(2) - (10)].str));
    ;}
    break;

  case 11:
#line 219 "src/Parser/parser.y"
    {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        
        for (const auto& param : *(yyvsp[(4) - (8)].param_list)) {
            params.push_back(param.first);
            param_types.push_back(param.second);
        }
        
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*(yyvsp[(7) - (8)].stmts));
        delete (yyvsp[(7) - (8)].stmts);
        
        (yyval.stmt) = static_cast<Stmt*>(new FunctionDecl(
            std::string((yyvsp[(2) - (8)].str)), 
            std::move(params), 
            std::move(block), 
            std::move(param_types)
        ));
        delete (yyvsp[(4) - (8)].param_list);
        free((yyvsp[(2) - (8)].str));
    ;}
    break;

  case 12:
#line 241 "src/Parser/parser.y"
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

  case 13:
#line 259 "src/Parser/parser.y"
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

  case 15:
#line 277 "src/Parser/parser.y"
    { (yyval.stmts) = new std::vector<StmtPtr>(); ;}
    break;

  case 16:
#line 278 "src/Parser/parser.y"
    { 
      (yyvsp[(1) - (3)].stmts)->push_back(StmtPtr((yyvsp[(2) - (3)].stmt))); 
      (yyval.stmts) = (yyvsp[(1) - (3)].stmts); 
    ;}
    break;

  case 17:
#line 282 "src/Parser/parser.y"
    { 
      (yyvsp[(1) - (3)].stmts)->push_back(StmtPtr(new ExprStmt(ExprPtr((yyvsp[(2) - (3)].expr))))); 
      (yyval.stmts) = (yyvsp[(1) - (3)].stmts); 
    ;}
    break;

  case 18:
#line 289 "src/Parser/parser.y"
    { (yyval.str_list) = new std::vector<std::string>(); ;}
    break;

  case 19:
#line 290 "src/Parser/parser.y"
    { (yyval.str_list) = new std::vector<std::string>(); (yyval.str_list)->push_back((yyvsp[(1) - (1)].str)); free((yyvsp[(1) - (1)].str)); ;}
    break;

  case 20:
#line 291 "src/Parser/parser.y"
    { (yyvsp[(1) - (3)].str_list)->push_back((yyvsp[(3) - (3)].str)); free((yyvsp[(3) - (3)].str)); (yyval.str_list) = (yyvsp[(1) - (3)].str_list); ;}
    break;

  case 21:
#line 295 "src/Parser/parser.y"
    { std::cout << "Evaluated expression statement" << std::endl; (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 22:
#line 296 "src/Parser/parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 23:
#line 297 "src/Parser/parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 24:
#line 298 "src/Parser/parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 25:
#line 299 "src/Parser/parser.y"
    { (yyval.expr) = new VariableExpr((yyvsp[(1) - (1)].str)); free((yyvsp[(1) - (1)].str)); ;}
    break;

  case 26:
#line 300 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_ADD, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 27:
#line 301 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_SUB, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 28:
#line 302 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_MUL, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 29:
#line 303 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_DIV, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 30:
#line 304 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_MOD, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 31:
#line 305 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_POW, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 32:
#line 306 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_CONCAT, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 33:
#line 307 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_CONCAT_WS, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 34:
#line 308 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_LT, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 35:
#line 309 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_GT, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 36:
#line 310 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_LE, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 37:
#line 311 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_GE, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 38:
#line 312 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_EQ, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 39:
#line 313 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_NEQ, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 40:
#line 314 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_AND, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 41:
#line 315 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_OR, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 42:
#line 316 "src/Parser/parser.y"
    { (yyval.expr) = new UnaryExpr(UnaryExpr::OP_NEG, ExprPtr((yyvsp[(2) - (2)].expr))); ;}
    break;

  case 43:
#line 317 "src/Parser/parser.y"
    { 
      std::vector<ExprPtr> args;
      for (auto& arg : *(yyvsp[(3) - (4)].expr_list)) {
          args.push_back(std::move(arg));
      }
      (yyval.expr) = new CallExpr((yyvsp[(1) - (4)].str), std::move(args)); 
      free((yyvsp[(1) - (4)].str)); 
      delete (yyvsp[(3) - (4)].expr_list); 
  ;}
    break;

  case 44:
#line 326 "src/Parser/parser.y"
    { (yyval.expr) = new CallExpr((yyvsp[(1) - (3)].str), std::vector<ExprPtr>()); free((yyvsp[(1) - (3)].str)); ;}
    break;

  case 45:
#line 327 "src/Parser/parser.y"
    {
      std::vector<ExprPtr> args;
      for (auto& arg : *(yyvsp[(4) - (5)].expr_list)) {
          args.push_back(std::move(arg));
      }
      (yyval.expr) = new NewExpr((yyvsp[(2) - (5)].str), std::move(args));
      free((yyvsp[(2) - (5)].str));
      delete (yyvsp[(4) - (5)].expr_list);
  ;}
    break;

  case 46:
#line 336 "src/Parser/parser.y"
    {
      (yyval.expr) = new NewExpr((yyvsp[(2) - (4)].str), std::vector<ExprPtr>());
      free((yyvsp[(2) - (4)].str));
  ;}
    break;

  case 47:
#line 340 "src/Parser/parser.y"
    {
      std::vector<ExprPtr> args;
      for (auto& arg : *(yyvsp[(5) - (6)].expr_list)) {
          args.push_back(std::move(arg));
      }
      (yyval.expr) = new MethodCallExpr(ExprPtr((yyvsp[(1) - (6)].expr)), (yyvsp[(3) - (6)].str), std::move(args));
      free((yyvsp[(3) - (6)].str));
      delete (yyvsp[(5) - (6)].expr_list);
  ;}
    break;

  case 48:
#line 349 "src/Parser/parser.y"
    {
      (yyval.expr) = new MethodCallExpr(ExprPtr((yyvsp[(1) - (5)].expr)), (yyvsp[(3) - (5)].str), std::vector<ExprPtr>());
      free((yyvsp[(3) - (5)].str));
  ;}
    break;

  case 49:
#line 353 "src/Parser/parser.y"
    {
      std::vector<ExprPtr> args;
      for (auto& arg : *(yyvsp[(3) - (4)].expr_list)) {
          args.push_back(std::move(arg));
      }
      (yyval.expr) = new BaseCallExpr(std::move(args));
      delete (yyvsp[(3) - (4)].expr_list);
  ;}
    break;

  case 50:
#line 361 "src/Parser/parser.y"
    {
      (yyval.expr) = new BaseCallExpr(std::vector<ExprPtr>());
  ;}
    break;

  case 51:
#line 364 "src/Parser/parser.y"
    { 
      std::vector<std::pair<std::string, ExprPtr>> bindings;
      for (auto& binding : *(yyvsp[(2) - (4)].bindings)) {
          bindings.push_back({binding.first, ExprPtr(binding.second)});
      }
      (yyval.expr) = new LetExpr(bindings[0].first, std::move(bindings[0].second), 
                      std::make_unique<ExprStmt>(ExprPtr((yyvsp[(4) - (4)].expr))));
      delete (yyvsp[(2) - (4)].bindings); 
  ;}
    break;

  case 53:
#line 375 "src/Parser/parser.y"
    { 
      auto block = ExprPtr(new ExprBlock(std::move(*(yyvsp[(4) - (5)].stmts))));
      (yyval.expr) = new WhileExpr(ExprPtr((yyvsp[(2) - (5)].expr)), std::move(block)); 
      delete (yyvsp[(4) - (5)].stmts);
  ;}
    break;

  case 54:
#line 380 "src/Parser/parser.y"
    { 
      (yyval.expr) = new ExprBlock(std::move(*(yyvsp[(2) - (3)].stmts))); 
      delete (yyvsp[(2) - (3)].stmts);
  ;}
    break;

  case 55:
#line 384 "src/Parser/parser.y"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); ;}
    break;

  case 56:
#line 385 "src/Parser/parser.y"
    {
        (yyval.expr) = new GetAttrExpr(ExprPtr((yyvsp[(1) - (3)].expr)), (yyvsp[(3) - (3)].str));
        free((yyvsp[(3) - (3)].str));
    ;}
    break;

  case 57:
#line 389 "src/Parser/parser.y"
    {
        (yyval.expr) = new SetAttrExpr(ExprPtr((yyvsp[(1) - (5)].expr)), (yyvsp[(3) - (5)].str), ExprPtr((yyvsp[(5) - (5)].expr)));
        free((yyvsp[(3) - (5)].str));
    ;}
    break;

  case 58:
#line 393 "src/Parser/parser.y"
    {
        (yyval.expr) = new MethodCallExpr(ExprPtr((yyvsp[(1) - (6)].expr)), (yyvsp[(3) - (6)].str), std::move(*(yyvsp[(5) - (6)].expr_list)));
        delete (yyvsp[(5) - (6)].expr_list);
        free((yyvsp[(3) - (6)].str));
    ;}
    break;

  case 59:
#line 398 "src/Parser/parser.y"
    {
        (yyval.expr) = new SelfExpr();
    ;}
    break;

  case 60:
#line 401 "src/Parser/parser.y"
    {
        (yyval.expr) = new BaseCallExpr({});
    ;}
    break;

  case 61:
#line 410 "src/Parser/parser.y"
    {
        (yyval.expr) = new IfExpr(ExprPtr((yyvsp[(3) - (7)].expr)), ExprPtr((yyvsp[(5) - (7)].expr)), ExprPtr((yyvsp[(7) - (7)].expr)));
    ;}
    break;

  case 62:
#line 413 "src/Parser/parser.y"
    { 
      auto ifBlock = ExprPtr(new ExprBlock(std::move(*(yyvsp[(6) - (11)].stmts))));
      auto elseBlock = ExprPtr(new ExprBlock(std::move(*(yyvsp[(10) - (11)].stmts))));
      (yyval.expr) = new IfExpr(ExprPtr((yyvsp[(3) - (11)].expr)), std::move(ifBlock), std::move(elseBlock)); 
      delete (yyvsp[(6) - (11)].stmts);
      delete (yyvsp[(10) - (11)].stmts);
  ;}
    break;

  case 63:
#line 420 "src/Parser/parser.y"
    { 
      auto ifBlock = ExprPtr(new ExprBlock(std::move(*(yyvsp[(6) - (7)].stmts))));
      (yyval.expr) = new IfExpr(ExprPtr((yyvsp[(3) - (7)].expr)), std::move(ifBlock), nullptr); 
      delete (yyvsp[(6) - (7)].stmts);
  ;}
    break;

  case 64:
#line 429 "src/Parser/parser.y"
    { (yyval.expr_list) = new std::vector<ExprPtr>(); ;}
    break;

  case 65:
#line 431 "src/Parser/parser.y"
    {
        (yyval.expr_list) = new std::vector<ExprPtr>();
        (yyval.expr_list)->emplace_back(ExprPtr((yyvsp[(1) - (1)].expr)));
      ;}
    break;

  case 66:
#line 436 "src/Parser/parser.y"
    {
        (yyvsp[(1) - (3)].expr_list)->emplace_back(ExprPtr((yyvsp[(3) - (3)].expr)));
        (yyval.expr_list) = (yyvsp[(1) - (3)].expr_list);
      ;}
    break;

  case 67:
#line 443 "src/Parser/parser.y"
    {
        (yyval.stmt) = static_cast<Stmt*>(new TypeDecl(
            std::string((yyvsp[(2) - (13)].str)),
            std::move(*(yyvsp[(4) - (13)].str_list)),
            std::move((yyvsp[(12) - (13)].mems)->first),
            std::move((yyvsp[(12) - (13)].mems)->second),
            std::string((yyvsp[(7) - (13)].str)),
            std::move(*(yyvsp[(9) - (13)].expr_list))
        ));
        delete (yyvsp[(4) - (13)].str_list); delete (yyvsp[(9) - (13)].expr_list); delete (yyvsp[(12) - (13)].mems);
        free((yyvsp[(2) - (13)].str)); free((yyvsp[(7) - (13)].str));
    ;}
    break;

  case 68:
#line 455 "src/Parser/parser.y"
    {
        (yyval.stmt) = static_cast<Stmt*>(new TypeDecl(
            std::string((yyvsp[(2) - (10)].str)),
            {},
            std::move((yyvsp[(9) - (10)].mems)->first),
            std::move((yyvsp[(9) - (10)].mems)->second),
            std::string((yyvsp[(4) - (10)].str)),
            std::move(*(yyvsp[(6) - (10)].expr_list))
        ));
        delete (yyvsp[(6) - (10)].expr_list); delete (yyvsp[(9) - (10)].mems);
        free((yyvsp[(2) - (10)].str)); free((yyvsp[(4) - (10)].str));
    ;}
    break;

  case 69:
#line 467 "src/Parser/parser.y"
    {
        (yyval.stmt) = static_cast<Stmt*>(new TypeDecl(
            std::string((yyvsp[(2) - (7)].str)),
            {}, // sin parámetros propios
            std::move((yyvsp[(6) - (7)].mems)->first),
            std::move((yyvsp[(6) - (7)].mems)->second),
            std::string((yyvsp[(4) - (7)].str)), // tipo base
            {} // sin baseArgs explícitos: herencia implícita
        ));
        delete (yyvsp[(6) - (7)].mems);
        free((yyvsp[(2) - (7)].str)); free((yyvsp[(4) - (7)].str));
    ;}
    break;

  case 70:
#line 479 "src/Parser/parser.y"
    {
        (yyval.stmt) = static_cast<Stmt*>(new TypeDecl(
            std::string((yyvsp[(2) - (8)].str)),
            std::move(*(yyvsp[(4) - (8)].str_list)),
            std::move((yyvsp[(7) - (8)].mems)->first),
            std::move((yyvsp[(7) - (8)].mems)->second)
        ));
        delete (yyvsp[(4) - (8)].str_list); delete (yyvsp[(7) - (8)].mems);
        free((yyvsp[(2) - (8)].str));
    ;}
    break;

  case 71:
#line 489 "src/Parser/parser.y"
    {
        (yyval.stmt) = static_cast<Stmt*>(new TypeDecl(
            std::string((yyvsp[(2) - (5)].str)),
            {},
            std::move((yyvsp[(4) - (5)].mems)->first),
            std::move((yyvsp[(4) - (5)].mems)->second)
        ));
        delete (yyvsp[(4) - (5)].mems); free((yyvsp[(2) - (5)].str));
    ;}
    break;

  case 72:
#line 501 "src/Parser/parser.y"
    { 
        auto* pair = new std::pair<
            std::vector<std::unique_ptr<AttributeDecl>>,
            std::vector<std::unique_ptr<MethodDecl>>
        >();
        (yyval.mems) = pair;
    ;}
    break;

  case 73:
#line 508 "src/Parser/parser.y"
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

  case 74:
#line 520 "src/Parser/parser.y"
    {
        auto* pair = new std::pair<AttributeDecl*, MethodDecl*>();
        pair->first = (yyvsp[(1) - (2)].attribute_decl);
        (yyval.member_pair) = pair;
    ;}
    break;

  case 75:
#line 525 "src/Parser/parser.y"
    {
        auto* pair = new std::pair<AttributeDecl*, MethodDecl*>();
        pair->second = (yyvsp[(1) - (2)].method_decl);
        (yyval.member_pair) = pair;
    ;}
    break;

  case 76:
#line 533 "src/Parser/parser.y"
    {
        (yyval.attribute_decl) = static_cast<AttributeDecl*>(new AttributeDecl((yyvsp[(1) - (3)].str), ExprPtr((yyvsp[(3) - (3)].expr))));
        free((yyvsp[(1) - (3)].str));
    ;}
    break;

  case 77:
#line 540 "src/Parser/parser.y"
    {
        std::vector<std::string> args = (yyvsp[(3) - (6)].str_list) ? std::move(*(yyvsp[(3) - (6)].str_list)) : std::vector<std::string>();
        delete (yyvsp[(3) - (6)].str_list);
        (yyval.method_decl) = static_cast<MethodDecl*>(new MethodDecl((yyvsp[(1) - (6)].str), std::move(args), StmtPtr(new ExprStmt(ExprPtr((yyvsp[(6) - (6)].expr))))));
        free((yyvsp[(1) - (6)].str));
    ;}
    break;

  case 78:
#line 546 "src/Parser/parser.y"
    {
        std::vector<std::string> args = (yyvsp[(3) - (7)].str_list) ? std::move(*(yyvsp[(3) - (7)].str_list)) : std::vector<std::string>();
        delete (yyvsp[(3) - (7)].str_list);
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*(yyvsp[(6) - (7)].stmts));
        delete (yyvsp[(6) - (7)].stmts);
        (yyval.method_decl) = static_cast<MethodDecl*>(new MethodDecl((yyvsp[(1) - (7)].str), std::move(args), StmtPtr(std::move(block))));
        free((yyvsp[(1) - (7)].str));
    ;}
    break;

  case 79:
#line 555 "src/Parser/parser.y"
    {
        std::vector<std::string> args;
        (yyval.method_decl) = static_cast<MethodDecl*>(new MethodDecl((yyvsp[(1) - (5)].str), std::move(args), StmtPtr(new ExprStmt(ExprPtr((yyvsp[(5) - (5)].expr))))));
        free((yyvsp[(1) - (5)].str));
    ;}
    break;

  case 81:
#line 563 "src/Parser/parser.y"
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

  case 82:
#line 581 "src/Parser/parser.y"
    { (yyval.param_list) = new std::vector<std::pair<std::string, std::shared_ptr<TypeInfo>>>(); ;}
    break;

  case 83:
#line 582 "src/Parser/parser.y"
    { 
        (yyval.param_list) = new std::vector<std::pair<std::string, std::shared_ptr<TypeInfo>>>();
        (yyval.param_list)->push_back(*(yyvsp[(1) - (1)].param_with_type));
        delete (yyvsp[(1) - (1)].param_with_type);
    ;}
    break;

  case 84:
#line 587 "src/Parser/parser.y"
    {
        (yyvsp[(1) - (3)].param_list)->push_back(*(yyvsp[(3) - (3)].param_with_type));
        delete (yyvsp[(3) - (3)].param_with_type);
        (yyval.param_list) = (yyvsp[(1) - (3)].param_list);
    ;}
    break;

  case 85:
#line 595 "src/Parser/parser.y"
    { 
        (yyval.param_with_type) = new std::pair<std::string, std::shared_ptr<TypeInfo>>(
            std::string((yyvsp[(1) - (1)].str)), 
            std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown)
        );
        free((yyvsp[(1) - (1)].str));
    ;}
    break;

  case 86:
#line 602 "src/Parser/parser.y"
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
#line 2553 "src/Parser/parser.tab.cpp"
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


#line 612 "src/Parser/parser.y"

