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
#line 299 "src/Parser/parser.tab.cpp"
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
#line 324 "src/Parser/parser.tab.cpp"

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
#define YYLAST   1019

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  53
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  19
/* YYNRULES -- Number of rules.  */
#define YYNRULES  95
/* YYNRULES -- Number of states.  */
#define YYNSTATES  230

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
      30,    32,    43,    52,    60,    70,    77,    86,    88,    89,
      93,    97,    98,   100,   104,   106,   108,   110,   112,   114,
     118,   122,   126,   130,   134,   138,   142,   146,   150,   154,
     158,   162,   166,   170,   174,   178,   181,   186,   190,   196,
     201,   208,   214,   219,   223,   228,   237,   244,   246,   252,
     260,   270,   274,   278,   282,   288,   295,   297,   301,   305,
     309,   317,   329,   337,   338,   340,   344,   358,   369,   377,
     386,   392,   393,   396,   399,   402,   406,   413,   421,   427,
     434,   436,   437,   439,   443,   445
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      54,     0,    -1,    55,    -1,    -1,    55,    58,    46,    -1,
      55,    61,    46,    -1,    57,    -1,    56,    45,    57,    -1,
       7,    13,    61,    -1,     7,    47,    69,    13,    61,    -1,
      61,    -1,    11,     7,    41,    70,    42,    47,    69,    43,
      59,    44,    -1,    11,     7,    41,    70,    42,    43,    59,
      44,    -1,    11,     7,    41,    70,    42,    12,    61,    -1,
      11,     7,    41,    70,    42,    47,    69,    12,    61,    -1,
      11,     7,    41,    42,    12,    61,    -1,    11,     7,    41,
      42,    47,    69,    12,    61,    -1,    64,    -1,    -1,    59,
      58,    46,    -1,    59,    61,    46,    -1,    -1,     7,    -1,
      60,    45,     7,    -1,    23,    -1,    24,    -1,    21,    -1,
      22,    -1,     7,    -1,    61,    25,    61,    -1,    61,    26,
      61,    -1,    61,    27,    61,    -1,    61,    28,    61,    -1,
      61,    29,    61,    -1,    61,    30,    61,    -1,    61,    31,
      61,    -1,    61,    32,    61,    -1,    61,    37,    61,    -1,
      61,    38,    61,    -1,    61,    33,    61,    -1,    61,    34,
      61,    -1,    61,    35,    61,    -1,    61,    36,    61,    -1,
      61,    40,    61,    -1,    61,    39,    61,    -1,    26,    61,
      -1,     7,    41,    63,    42,    -1,     7,    41,    42,    -1,
      18,     7,    41,    63,    42,    -1,    18,     7,    41,    42,
      -1,    61,    16,     7,    41,    63,    42,    -1,    61,    16,
       7,    41,    42,    -1,    20,    41,    63,    42,    -1,    20,
      41,    42,    -1,     3,    56,     4,    61,    -1,     3,     7,
      47,    69,    13,    61,     4,    61,    -1,     3,     7,    13,
      61,     4,    61,    -1,    62,    -1,     5,    61,    43,    59,
      44,    -1,     6,    41,     7,     4,    61,    42,    61,    -1,
       6,    41,     7,     4,    61,    42,    43,    59,    44,    -1,
      43,    59,    44,    -1,    41,    61,    42,    -1,    61,    16,
       7,    -1,    61,    16,     7,    14,    61,    -1,    61,    16,
       7,    41,    63,    42,    -1,    17,    -1,    17,    14,    61,
      -1,    20,    41,    42,    -1,     7,    14,    61,    -1,     8,
      41,    61,    42,    61,     9,    61,    -1,     8,    41,    61,
      42,    43,    59,    44,     9,    43,    59,    44,    -1,     8,
      41,    61,    42,    43,    59,    44,    -1,    -1,    61,    -1,
      63,    45,    61,    -1,    15,     7,    41,    60,    42,    19,
       7,    41,    63,    42,    43,    65,    44,    -1,    15,     7,
      19,     7,    41,    63,    42,    43,    65,    44,    -1,    15,
       7,    19,     7,    43,    65,    44,    -1,    15,     7,    41,
      60,    42,    43,    65,    44,    -1,    15,     7,    43,    65,
      44,    -1,    -1,    65,    66,    -1,    67,    46,    -1,    68,
      46,    -1,     7,    13,    61,    -1,     7,    41,    60,    42,
      12,    61,    -1,     7,    41,    60,    42,    43,    59,    44,
      -1,     7,    41,    42,    12,    61,    -1,     7,    41,    42,
      43,    59,    44,    -1,     7,    -1,    -1,    71,    -1,    70,
      45,    71,    -1,     7,    -1,     7,    47,    69,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   157,   157,   161,   162,   167,   175,   180,   188,   195,
     206,   209,   233,   255,   273,   293,   305,   319,   325,   326,
     330,   337,   338,   339,   343,   344,   345,   346,   347,   348,
     349,   350,   351,   352,   353,   354,   355,   356,   357,   358,
     359,   360,   361,   362,   363,   364,   365,   374,   375,   384,
     388,   397,   401,   409,   412,   438,   444,   449,   451,   456,
     460,   466,   470,   471,   475,   479,   484,   487,   490,   493,
     502,   505,   512,   521,   522,   527,   535,   547,   559,   571,
     581,   593,   600,   612,   617,   625,   632,   638,   647,   652,
     663,   681,   682,   687,   695,   702
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
       0,    53,    54,    55,    55,    55,    56,    56,    57,    57,
      58,    58,    58,    58,    58,    58,    58,    58,    59,    59,
      59,    60,    60,    60,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      62,    62,    62,    63,    63,    63,    64,    64,    64,    64,
      64,    65,    65,    66,    66,    67,    68,    68,    68,    68,
      69,    70,    70,    70,    71,    71
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     3,     3,     1,     3,     3,     5,
       1,    10,     8,     7,     9,     6,     8,     1,     0,     3,
       3,     0,     1,     3,     1,     1,     1,     1,     1,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     2,     4,     3,     5,     4,
       6,     5,     4,     3,     4,     8,     6,     1,     5,     7,
       9,     3,     3,     3,     5,     6,     1,     3,     3,     3,
       7,    11,     7,     0,     1,     3,    13,    10,     7,     8,
       5,     0,     2,     2,     2,     3,     6,     7,     5,     6,
       1,     0,     1,     3,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     2,     1,     0,     0,     0,    28,     0,     0,
       0,    66,     0,     0,    26,    27,    24,    25,     0,     0,
      18,     0,     0,    57,    17,     0,     0,     6,     0,     0,
       0,    73,     0,     0,     0,     0,     0,    73,    45,     0,
       0,     4,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     5,
       0,     0,     0,     0,    18,     0,    69,    47,    74,     0,
       0,    91,     0,    21,    81,    67,    73,    53,     0,    62,
      61,     0,     0,    63,    29,    30,    31,    32,    33,    34,
      35,    36,    39,    40,    41,    42,    37,    38,    44,    43,
       8,    90,     0,    54,     0,     7,     0,     0,    46,     0,
       0,    94,     0,     0,    92,     0,    22,     0,     0,    49,
       0,    52,    19,    20,     0,    73,     0,     0,     0,     0,
      58,     0,    75,    18,     0,     0,     0,     0,     0,     0,
      73,    81,     0,     0,     0,    80,    82,     0,     0,    48,
      64,    51,     0,    56,     9,     8,     0,     0,     0,     0,
      95,    15,     0,     0,    18,     0,    93,     0,     0,     0,
      81,    23,     0,    21,    83,    84,    50,     0,     0,    18,
      59,    61,    70,     0,    13,     0,     0,     0,    78,     0,
       0,    85,     0,     0,    55,     9,     0,     0,    16,    12,
       0,    18,    81,    73,    79,     0,    18,     0,    60,    18,
      14,     0,     0,     0,    88,     0,     0,    18,     0,    11,
      77,     0,    89,    86,     0,    71,    81,    87,     0,    76
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    26,    27,    81,    40,   117,    82,    23,
      69,    24,   118,   146,   147,   148,   102,   113,   114
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -140
static const yytype_int16 yypact[] =
{
    -140,    21,   437,  -140,     7,   572,    45,    89,    51,    15,
     108,   113,   124,   117,  -140,  -140,  -140,  -140,   572,   572,
    -140,    99,   741,  -140,  -140,    72,    14,  -140,   791,   147,
     572,   464,   572,   119,   101,   572,   121,   491,    95,   816,
      73,  -140,   156,   572,   572,   572,   572,   572,   572,   572,
     572,   572,   572,   572,   572,   572,   572,   572,   572,  -140,
     572,   158,   572,   161,  -140,   160,   891,  -140,   891,   -34,
     841,    -3,   162,   164,  -140,   891,   518,  -140,   -32,  -140,
    -140,   126,   766,    96,   150,   150,   -11,   -11,   -11,   -11,
     981,   981,   157,   157,   962,   962,   157,   157,   916,   939,
     647,  -140,   175,   891,    88,  -140,   189,   572,  -140,   572,
     599,   127,    65,   -30,  -140,    98,  -140,   -19,    13,  -140,
      87,  -140,  -140,  -140,   572,   545,   572,   572,   572,   158,
    -140,   866,   891,  -140,   716,   158,   572,   158,    55,   182,
     572,  -140,   109,   183,    -4,  -140,  -140,   145,   153,  -140,
     891,  -140,    91,   891,   684,   891,   180,   626,   220,   572,
    -140,   891,   190,   572,  -140,   158,  -140,   104,    17,   198,
    -140,  -140,   572,    76,  -140,  -140,  -140,   572,   572,  -140,
     891,    23,   891,   572,   891,   251,    -5,   165,  -140,   176,
      18,   891,    70,   105,   891,   891,   282,   173,   891,  -140,
     572,  -140,  -140,   572,  -140,   572,  -140,    97,  -140,  -140,
     891,   313,    27,   111,   891,   344,   572,  -140,   375,  -140,
    -140,   177,  -140,   891,   406,  -140,  -140,  -140,    29,  -140
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -140,  -140,  -140,  -140,   155,   217,   -58,    48,    -2,  -140,
     -36,  -140,  -139,  -140,  -140,  -140,   -65,  -140,    83
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -73
static const yytype_int16 yytable[] =
{
      22,    78,   168,    28,   111,    42,   106,   200,   108,   172,
     121,   109,   138,   109,    25,   139,    38,    39,    62,    48,
     144,     3,    33,   142,   144,   144,   143,   -72,    66,    68,
      70,   190,   197,    75,   144,    68,   144,   173,   201,   112,
     120,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   145,   100,    63,
     103,   188,   204,   212,   156,   -72,   -72,   163,   -72,   -72,
     160,   220,   162,   229,    68,   158,     4,   136,     5,     6,
       7,     8,   205,   116,     9,    60,    29,   228,    10,   152,
      11,    12,    32,    13,    14,    15,    16,    17,   164,    18,
     186,   128,   165,    30,   167,   131,   185,   132,   134,   216,
     124,    42,   137,   206,    19,    34,    20,    80,   192,    61,
      72,   196,   150,    68,   153,   154,   155,    35,   169,   149,
      31,    36,   109,   176,   161,   129,   109,   125,    68,   140,
     217,   141,    73,   211,    74,    41,   187,   207,   215,   109,
     143,   218,   170,   221,    65,   180,   109,   182,    37,   224,
      71,   184,    76,    83,   107,   101,    42,   213,   104,   115,
     191,   116,   122,    42,   135,   194,   195,    45,    46,    47,
      48,   198,    43,    44,    45,    46,    47,    48,   127,   111,
     171,   174,     4,   178,     5,     6,     7,     8,   210,   175,
       9,    68,   183,   214,    10,   189,    11,    12,   202,    13,
      14,    15,    16,    17,   223,    18,   209,   203,   105,    21,
     226,   193,   166,     4,     0,     5,     6,     7,     8,     0,
      19,     9,    20,   130,     0,    10,     0,    11,    12,     0,
      13,    14,    15,    16,    17,     0,    18,     0,     0,     0,
       0,     0,     0,     0,     4,     0,     5,     6,     7,     8,
       0,    19,     9,    20,   181,     0,    10,     0,    11,    12,
       0,    13,    14,    15,    16,    17,     0,    18,     0,     0,
       0,     0,     0,     0,     0,     4,     0,     5,     6,     7,
       8,     0,    19,     9,    20,   199,     0,    10,     0,    11,
      12,     0,    13,    14,    15,    16,    17,     0,    18,     0,
       0,     0,     0,     0,     0,     0,     4,     0,     5,     6,
       7,     8,     0,    19,     9,    20,   208,     0,    10,     0,
      11,    12,     0,    13,    14,    15,    16,    17,     0,    18,
       0,     0,     0,     0,     0,     0,     0,     4,     0,     5,
       6,     7,     8,     0,    19,     9,    20,   219,     0,    10,
       0,    11,    12,     0,    13,    14,    15,    16,    17,     0,
      18,     0,     0,     0,     0,     0,     0,     0,     4,     0,
       5,     6,     7,     8,     0,    19,     9,    20,   222,     0,
      10,     0,    11,    12,     0,    13,    14,    15,    16,    17,
       0,    18,     0,     0,     0,     0,     0,     0,     0,     4,
       0,     5,     6,     7,     8,     0,    19,     9,    20,   225,
       0,    10,     0,    11,    12,     0,    13,    14,    15,    16,
      17,     0,    18,     0,     0,     0,     0,     0,     0,     0,
       4,     0,     5,     6,     7,     8,     0,    19,     9,    20,
     227,     0,    10,     0,    11,    12,     0,    13,    14,    15,
      16,    17,     0,    18,     0,     0,     0,     4,     0,     5,
       6,     7,     8,     0,     0,     0,     0,     0,    19,     0,
      20,    11,    12,     0,    13,    14,    15,    16,    17,     0,
      18,     0,     0,     0,     4,     0,     5,     6,     7,     8,
       0,     0,     0,     0,     0,    19,    67,    20,    11,    12,
       0,    13,    14,    15,    16,    17,     0,    18,     0,     0,
       0,     4,     0,     5,     6,     7,     8,     0,     0,     0,
       0,     0,    19,    77,    20,    11,    12,     0,    13,    14,
      15,    16,    17,     0,    18,     0,     0,     0,     4,     0,
       5,     6,     7,     8,     0,     0,     0,     0,     0,    19,
     119,    20,    11,    12,     0,    13,    14,    15,    16,    17,
       0,    18,     0,     0,     0,     4,     0,     5,     6,     7,
       8,     0,     0,     0,     0,     0,    19,   151,    20,    11,
      12,     0,    13,    14,    15,    16,    17,     0,    18,     0,
       0,     0,     4,     0,     5,     6,     7,     8,     0,     0,
       0,     0,     0,    19,     0,    20,    11,    12,     0,    13,
      14,    15,    16,    17,     0,    18,     0,     0,     0,     4,
       0,     5,     6,     7,     8,     0,     0,     0,     0,     0,
      19,     0,   133,    11,    12,     0,    13,    14,    15,    16,
      17,   126,    18,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    42,     0,     0,     0,    19,     0,   179,
       0,     0,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,   177,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      42,     0,     0,     0,     0,     0,     0,     0,     0,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,   159,     0,     0,     0,     0,
       0,     0,    42,     0,     0,     0,     0,     0,     0,     0,
       0,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    42,     0,     0,
       0,     0,     0,     0,     0,     0,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    42,     0,     0,     0,     0,    59,     0,     0,
       0,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    42,     0,     0,
       0,     0,   123,     0,     0,     0,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    42,     0,    64,     0,     0,     0,     0,     0,
       0,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    42,    79,     0,
       0,     0,     0,     0,     0,     0,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    42,   110,     0,     0,     0,     0,     0,     0,
       0,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    42,   157,     0,
       0,     0,     0,     0,     0,     0,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    42,     0,     0,     0,     0,     0,     0,     0,
       0,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    42,    58,     0,     0,     0,
       0,     0,     0,     0,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    42,     0,
       0,     0,     0,     0,     0,     0,     0,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    42,     0,    55,
      56,     0,     0,     0,     0,     0,    43,    44,    45,    46,
      47,    48,     0,     0,    51,    52,     0,     0,    55,    56
};

static const yytype_int16 yycheck[] =
{
       2,    37,   141,     5,     7,    16,    64,    12,    42,    13,
      42,    45,    42,    45,     7,    45,    18,    19,     4,    30,
       7,     0,     7,    42,     7,     7,    45,     4,    30,    31,
      32,   170,     9,    35,     7,    37,     7,    41,    43,    42,
      76,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    44,    60,    45,
      62,    44,    44,   202,   129,    42,    43,    12,    45,    46,
     135,    44,   137,    44,    76,   133,     3,    12,     5,     6,
       7,     8,    12,     7,    11,    13,    41,   226,    15,   125,
      17,    18,    41,    20,    21,    22,    23,    24,    43,    26,
     165,    13,    47,    14,   140,   107,   164,   109,   110,    12,
      14,    16,    47,    43,    41,     7,    43,    44,    42,    47,
      19,   179,   124,   125,   126,   127,   128,    14,    19,    42,
      41,     7,    45,    42,   136,    47,    45,    41,   140,    41,
      43,    43,    41,   201,    43,    46,    42,    42,   206,    45,
      45,   209,    43,    42,     7,   157,    45,   159,    41,   217,
      41,   163,    41,     7,     4,     7,    16,   203,     7,     7,
     172,     7,    46,    16,    47,   177,   178,    27,    28,    29,
      30,   183,    25,    26,    27,    28,    29,    30,    13,     7,
       7,    46,     3,    13,     5,     6,     7,     8,   200,    46,
      11,   203,    12,   205,    15,     7,    17,    18,    43,    20,
      21,    22,    23,    24,   216,    26,    43,    41,    63,     2,
      43,   173,   139,     3,    -1,     5,     6,     7,     8,    -1,
      41,    11,    43,    44,    -1,    15,    -1,    17,    18,    -1,
      20,    21,    22,    23,    24,    -1,    26,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,    -1,     5,     6,     7,     8,
      -1,    41,    11,    43,    44,    -1,    15,    -1,    17,    18,
      -1,    20,    21,    22,    23,    24,    -1,    26,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,     3,    -1,     5,     6,     7,
       8,    -1,    41,    11,    43,    44,    -1,    15,    -1,    17,
      18,    -1,    20,    21,    22,    23,    24,    -1,    26,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,    -1,     5,     6,
       7,     8,    -1,    41,    11,    43,    44,    -1,    15,    -1,
      17,    18,    -1,    20,    21,    22,    23,    24,    -1,    26,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,    -1,     5,
       6,     7,     8,    -1,    41,    11,    43,    44,    -1,    15,
      -1,    17,    18,    -1,    20,    21,    22,    23,    24,    -1,
      26,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,    -1,
       5,     6,     7,     8,    -1,    41,    11,    43,    44,    -1,
      15,    -1,    17,    18,    -1,    20,    21,    22,    23,    24,
      -1,    26,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
      -1,     5,     6,     7,     8,    -1,    41,    11,    43,    44,
      -1,    15,    -1,    17,    18,    -1,    20,    21,    22,    23,
      24,    -1,    26,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,    -1,     5,     6,     7,     8,    -1,    41,    11,    43,
      44,    -1,    15,    -1,    17,    18,    -1,    20,    21,    22,
      23,    24,    -1,    26,    -1,    -1,    -1,     3,    -1,     5,
       6,     7,     8,    -1,    -1,    -1,    -1,    -1,    41,    -1,
      43,    17,    18,    -1,    20,    21,    22,    23,    24,    -1,
      26,    -1,    -1,    -1,     3,    -1,     5,     6,     7,     8,
      -1,    -1,    -1,    -1,    -1,    41,    42,    43,    17,    18,
      -1,    20,    21,    22,    23,    24,    -1,    26,    -1,    -1,
      -1,     3,    -1,     5,     6,     7,     8,    -1,    -1,    -1,
      -1,    -1,    41,    42,    43,    17,    18,    -1,    20,    21,
      22,    23,    24,    -1,    26,    -1,    -1,    -1,     3,    -1,
       5,     6,     7,     8,    -1,    -1,    -1,    -1,    -1,    41,
      42,    43,    17,    18,    -1,    20,    21,    22,    23,    24,
      -1,    26,    -1,    -1,    -1,     3,    -1,     5,     6,     7,
       8,    -1,    -1,    -1,    -1,    -1,    41,    42,    43,    17,
      18,    -1,    20,    21,    22,    23,    24,    -1,    26,    -1,
      -1,    -1,     3,    -1,     5,     6,     7,     8,    -1,    -1,
      -1,    -1,    -1,    41,    -1,    43,    17,    18,    -1,    20,
      21,    22,    23,    24,    -1,    26,    -1,    -1,    -1,     3,
      -1,     5,     6,     7,     8,    -1,    -1,    -1,    -1,    -1,
      41,    -1,    43,    17,    18,    -1,    20,    21,    22,    23,
      24,     4,    26,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    16,    -1,    -1,    -1,    41,    -1,    43,
      -1,    -1,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,     4,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      16,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,     9,    -1,    -1,    -1,    -1,
      -1,    -1,    16,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    16,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    16,    -1,    -1,    -1,    -1,    46,    -1,    -1,
      -1,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    16,    -1,    -1,
      -1,    -1,    46,    -1,    -1,    -1,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    16,    -1,    43,    -1,    -1,    -1,    -1,    -1,
      -1,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    16,    42,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    16,    42,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    16,    42,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    25,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    16,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    16,    40,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    16,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    16,    -1,    37,
      38,    -1,    -1,    -1,    -1,    -1,    25,    26,    27,    28,
      29,    30,    -1,    -1,    33,    34,    -1,    -1,    37,    38
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    54,    55,     0,     3,     5,     6,     7,     8,    11,
      15,    17,    18,    20,    21,    22,    23,    24,    26,    41,
      43,    58,    61,    62,    64,     7,    56,    57,    61,    41,
      14,    41,    41,     7,     7,    14,     7,    41,    61,    61,
      59,    46,    16,    25,    26,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    46,
      13,    47,     4,    45,    43,     7,    61,    42,    61,    63,
      61,    41,    19,    41,    43,    61,    41,    42,    63,    42,
      44,    58,    61,     7,    61,    61,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    61,    61,    61,    61,    61,
      61,     7,    69,    61,     7,    57,    59,     4,    42,    45,
      42,     7,    42,    70,    71,     7,     7,    60,    65,    42,
      63,    42,    46,    46,    14,    41,     4,    13,    13,    47,
      44,    61,    61,    43,    61,    47,    12,    47,    42,    45,
      41,    43,    42,    45,     7,    44,    66,    67,    68,    42,
      61,    42,    63,    61,    61,    61,    69,    42,    59,     9,
      69,    61,    69,    12,    43,    47,    71,    63,    65,    19,
      43,     7,    13,    41,    46,    46,    42,     4,    13,    43,
      61,    44,    61,    12,    61,    59,    69,    42,    44,     7,
      65,    61,    42,    60,    61,    61,    59,     9,    61,    44,
      12,    43,    43,    41,    44,    12,    43,    42,    44,    43,
      61,    59,    65,    63,    61,    59,    12,    43,    59,    44,
      44,    42,    44,    61,    59,    44,    43,    44,    65,    44
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
#line 157 "src/Parser/parser.y"
    { rootAST = (yyvsp[(1) - (1)].prog); ;}
    break;

  case 3:
#line 161 "src/Parser/parser.y"
    { (yyval.prog) = new Program(); ;}
    break;

  case 4:
#line 163 "src/Parser/parser.y"
    {
      (yyvsp[(1) - (3)].prog)->stmts.emplace_back( StmtPtr((yyvsp[(2) - (3)].stmt)) );
      (yyval.prog) = (yyvsp[(1) - (3)].prog);
    ;}
    break;

  case 5:
#line 168 "src/Parser/parser.y"
    {
      (yyvsp[(1) - (3)].prog)->stmts.emplace_back( StmtPtr(new ExprStmt(ExprPtr((yyvsp[(2) - (3)].expr)))) );
      (yyval.prog) = (yyvsp[(1) - (3)].prog);
    ;}
    break;

  case 6:
#line 175 "src/Parser/parser.y"
    {
          (yyval.bindings_with_types) = new std::vector<std::pair<std::string, std::pair<Expr*, std::shared_ptr<TypeInfo>>>>();
          (yyval.bindings_with_types)->push_back(std::make_pair((yyvsp[(1) - (1)].binding_with_type)->first, std::make_pair((yyvsp[(1) - (1)].binding_with_type)->second.first, (yyvsp[(1) - (1)].binding_with_type)->second.second)));
          delete (yyvsp[(1) - (1)].binding_with_type);
      ;}
    break;

  case 7:
#line 180 "src/Parser/parser.y"
    {
          (yyvsp[(1) - (3)].bindings_with_types)->push_back(std::make_pair((yyvsp[(3) - (3)].binding_with_type)->first, std::make_pair((yyvsp[(3) - (3)].binding_with_type)->second.first, (yyvsp[(3) - (3)].binding_with_type)->second.second)));
          delete (yyvsp[(3) - (3)].binding_with_type);
          (yyval.bindings_with_types) = (yyvsp[(1) - (3)].bindings_with_types);
      ;}
    break;

  case 8:
#line 188 "src/Parser/parser.y"
    {
          (yyval.binding_with_type) = new std::pair<std::string, std::pair<Expr*, std::shared_ptr<TypeInfo>>>(
              std::string((yyvsp[(1) - (3)].str)), 
              std::make_pair((yyvsp[(3) - (3)].expr), std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown))
          );
          free((yyvsp[(1) - (3)].str));
      ;}
    break;

  case 9:
#line 195 "src/Parser/parser.y"
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
#line 206 "src/Parser/parser.y"
    {
        (yyval.stmt) = (new ExprStmt(ExprPtr((yyvsp[(1) - (1)].expr))));
    ;}
    break;

  case 11:
#line 209 "src/Parser/parser.y"
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
            std::move(param_types),
            *(yyvsp[(7) - (10)].type_info)
        ));
        delete (yyvsp[(4) - (10)].param_list);
        delete (yyvsp[(7) - (10)].type_info);
        free((yyvsp[(2) - (10)].str));
    ;}
    break;

  case 12:
#line 233 "src/Parser/parser.y"
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

  case 13:
#line 255 "src/Parser/parser.y"
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

  case 14:
#line 273 "src/Parser/parser.y"
    {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        
        for (const auto& param : *(yyvsp[(4) - (9)].param_list)) {
            params.push_back(param.first);
            param_types.push_back(param.second);
        }
        
        (yyval.stmt) = static_cast<Stmt*>(new FunctionDecl(
            std::string((yyvsp[(2) - (9)].str)), 
            std::move(params), 
            std::make_unique<ExprStmt>(ExprPtr((yyvsp[(9) - (9)].expr))), 
            std::move(param_types),
            *(yyvsp[(7) - (9)].type_info)
        ));
        delete (yyvsp[(4) - (9)].param_list);
        delete (yyvsp[(7) - (9)].type_info);
        free((yyvsp[(2) - (9)].str));
    ;}
    break;

  case 15:
#line 293 "src/Parser/parser.y"
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

  case 16:
#line 305 "src/Parser/parser.y"
    {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        
        (yyval.stmt) = static_cast<Stmt*>(new FunctionDecl(
            std::string((yyvsp[(2) - (8)].str)), 
            std::move(params), 
            std::make_unique<ExprStmt>(ExprPtr((yyvsp[(8) - (8)].expr))), 
            std::move(param_types),
            *(yyvsp[(6) - (8)].type_info)
        ));
        delete (yyvsp[(6) - (8)].type_info);
        free((yyvsp[(2) - (8)].str));
    ;}
    break;

  case 18:
#line 325 "src/Parser/parser.y"
    { (yyval.stmts) = new std::vector<StmtPtr>(); ;}
    break;

  case 19:
#line 326 "src/Parser/parser.y"
    { 
      (yyvsp[(1) - (3)].stmts)->push_back(StmtPtr((yyvsp[(2) - (3)].stmt))); 
      (yyval.stmts) = (yyvsp[(1) - (3)].stmts); 
    ;}
    break;

  case 20:
#line 330 "src/Parser/parser.y"
    { 
      (yyvsp[(1) - (3)].stmts)->push_back(StmtPtr(new ExprStmt(ExprPtr((yyvsp[(2) - (3)].expr))))); 
      (yyval.stmts) = (yyvsp[(1) - (3)].stmts); 
    ;}
    break;

  case 21:
#line 337 "src/Parser/parser.y"
    { (yyval.str_list) = new std::vector<std::string>(); ;}
    break;

  case 22:
#line 338 "src/Parser/parser.y"
    { (yyval.str_list) = new std::vector<std::string>(); (yyval.str_list)->push_back((yyvsp[(1) - (1)].str)); free((yyvsp[(1) - (1)].str)); ;}
    break;

  case 23:
#line 339 "src/Parser/parser.y"
    { (yyvsp[(1) - (3)].str_list)->push_back((yyvsp[(3) - (3)].str)); free((yyvsp[(3) - (3)].str)); (yyval.str_list) = (yyvsp[(1) - (3)].str_list); ;}
    break;

  case 24:
#line 343 "src/Parser/parser.y"
    { std::cout << "Evaluated expression statement" << std::endl; (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 25:
#line 344 "src/Parser/parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 26:
#line 345 "src/Parser/parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 27:
#line 346 "src/Parser/parser.y"
    { (yyval.expr) = (yyvsp[(1) - (1)].expr); ;}
    break;

  case 28:
#line 347 "src/Parser/parser.y"
    { (yyval.expr) = new VariableExpr((yyvsp[(1) - (1)].str)); free((yyvsp[(1) - (1)].str)); ;}
    break;

  case 29:
#line 348 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_ADD, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 30:
#line 349 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_SUB, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 31:
#line 350 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_MUL, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 32:
#line 351 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_DIV, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 33:
#line 352 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_MOD, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 34:
#line 353 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_POW, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 35:
#line 354 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_CONCAT, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 36:
#line 355 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_CONCAT_WS, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 37:
#line 356 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_LT, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 38:
#line 357 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_GT, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 39:
#line 358 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_LE, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 40:
#line 359 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_GE, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 41:
#line 360 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_EQ, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 42:
#line 361 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_NEQ, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 43:
#line 362 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_AND, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 44:
#line 363 "src/Parser/parser.y"
    { (yyval.expr) = new BinaryExpr(BinaryExpr::OP_OR, ExprPtr((yyvsp[(1) - (3)].expr)), ExprPtr((yyvsp[(3) - (3)].expr))); ;}
    break;

  case 45:
#line 364 "src/Parser/parser.y"
    { (yyval.expr) = new UnaryExpr(UnaryExpr::OP_NEG, ExprPtr((yyvsp[(2) - (2)].expr))); ;}
    break;

  case 46:
#line 365 "src/Parser/parser.y"
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

  case 47:
#line 374 "src/Parser/parser.y"
    { (yyval.expr) = new CallExpr((yyvsp[(1) - (3)].str), std::vector<ExprPtr>()); free((yyvsp[(1) - (3)].str)); ;}
    break;

  case 48:
#line 375 "src/Parser/parser.y"
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

  case 49:
#line 384 "src/Parser/parser.y"
    {
      (yyval.expr) = new NewExpr((yyvsp[(2) - (4)].str), std::vector<ExprPtr>());
      free((yyvsp[(2) - (4)].str));
  ;}
    break;

  case 50:
#line 388 "src/Parser/parser.y"
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

  case 51:
#line 397 "src/Parser/parser.y"
    {
      (yyval.expr) = new MethodCallExpr(ExprPtr((yyvsp[(1) - (5)].expr)), (yyvsp[(3) - (5)].str), std::vector<ExprPtr>());
      free((yyvsp[(3) - (5)].str));
  ;}
    break;

  case 52:
#line 401 "src/Parser/parser.y"
    {
      std::vector<ExprPtr> args;
      for (auto& arg : *(yyvsp[(3) - (4)].expr_list)) {
          args.push_back(std::move(arg));
      }
      (yyval.expr) = new BaseCallExpr(std::move(args));
      delete (yyvsp[(3) - (4)].expr_list);
  ;}
    break;

  case 53:
#line 409 "src/Parser/parser.y"
    {
      (yyval.expr) = new BaseCallExpr({});
  ;}
    break;

  case 54:
#line 412 "src/Parser/parser.y"
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
      
      (yyval.expr) = result;
      delete (yyvsp[(2) - (4)].bindings_with_types); 
  ;}
    break;

  case 55:
#line 438 "src/Parser/parser.y"
    {
      (yyval.expr) = new LetExpr(std::string((yyvsp[(2) - (8)].str)), ExprPtr((yyvsp[(6) - (8)].expr)), 
                      std::make_unique<ExprStmt>(ExprPtr((yyvsp[(8) - (8)].expr))), *(yyvsp[(4) - (8)].type_info));
      delete (yyvsp[(4) - (8)].type_info);
      free((yyvsp[(2) - (8)].str));
  ;}
    break;

  case 56:
#line 444 "src/Parser/parser.y"
    {
      (yyval.expr) = new LetExpr(std::string((yyvsp[(2) - (6)].str)), ExprPtr((yyvsp[(4) - (6)].expr)), 
                      std::make_unique<ExprStmt>(ExprPtr((yyvsp[(6) - (6)].expr))));
      free((yyvsp[(2) - (6)].str));
  ;}
    break;

  case 58:
#line 451 "src/Parser/parser.y"
    { 
      auto block = ExprPtr(new ExprBlock(std::move(*(yyvsp[(4) - (5)].stmts))));
      (yyval.expr) = new WhileExpr(ExprPtr((yyvsp[(2) - (5)].expr)), std::move(block)); 
      delete (yyvsp[(4) - (5)].stmts);
  ;}
    break;

  case 59:
#line 456 "src/Parser/parser.y"
    {
      (yyval.expr) = new ForExpr(std::string((yyvsp[(3) - (7)].str)), ExprPtr((yyvsp[(5) - (7)].expr)), ExprPtr((yyvsp[(7) - (7)].expr)));
      free((yyvsp[(3) - (7)].str));
  ;}
    break;

  case 60:
#line 460 "src/Parser/parser.y"
    {
      auto block = ExprPtr(new ExprBlock(std::move(*(yyvsp[(8) - (9)].stmts))));
      (yyval.expr) = new ForExpr(std::string((yyvsp[(3) - (9)].str)), ExprPtr((yyvsp[(5) - (9)].expr)), std::move(block));
      free((yyvsp[(3) - (9)].str));
      delete (yyvsp[(8) - (9)].stmts);
  ;}
    break;

  case 61:
#line 466 "src/Parser/parser.y"
    { 
      (yyval.expr) = new ExprBlock(std::move(*(yyvsp[(2) - (3)].stmts))); 
      delete (yyvsp[(2) - (3)].stmts);
  ;}
    break;

  case 62:
#line 470 "src/Parser/parser.y"
    { (yyval.expr) = (yyvsp[(2) - (3)].expr); ;}
    break;

  case 63:
#line 471 "src/Parser/parser.y"
    {
        (yyval.expr) = new GetAttrExpr(ExprPtr((yyvsp[(1) - (3)].expr)), (yyvsp[(3) - (3)].str));
        free((yyvsp[(3) - (3)].str));
    ;}
    break;

  case 64:
#line 475 "src/Parser/parser.y"
    {
        (yyval.expr) = new SetAttrExpr(ExprPtr((yyvsp[(1) - (5)].expr)), (yyvsp[(3) - (5)].str), ExprPtr((yyvsp[(5) - (5)].expr)));
        free((yyvsp[(3) - (5)].str));
    ;}
    break;

  case 65:
#line 479 "src/Parser/parser.y"
    {
        (yyval.expr) = new MethodCallExpr(ExprPtr((yyvsp[(1) - (6)].expr)), (yyvsp[(3) - (6)].str), std::move(*(yyvsp[(5) - (6)].expr_list)));
        delete (yyvsp[(5) - (6)].expr_list);
        free((yyvsp[(3) - (6)].str));
    ;}
    break;

  case 66:
#line 484 "src/Parser/parser.y"
    {
        (yyval.expr) = new SelfExpr();
    ;}
    break;

  case 67:
#line 487 "src/Parser/parser.y"
    {
        (yyval.expr) = new AssignExpr("self", ExprPtr((yyvsp[(3) - (3)].expr)));
    ;}
    break;

  case 68:
#line 490 "src/Parser/parser.y"
    {
        (yyval.expr) = new BaseCallExpr({});
    ;}
    break;

  case 69:
#line 493 "src/Parser/parser.y"
    {
        (yyval.expr) = new AssignExpr(std::string((yyvsp[(1) - (3)].str)), ExprPtr((yyvsp[(3) - (3)].expr)));
        free((yyvsp[(1) - (3)].str));
    ;}
    break;

  case 70:
#line 502 "src/Parser/parser.y"
    {
        (yyval.expr) = new IfExpr(ExprPtr((yyvsp[(3) - (7)].expr)), ExprPtr((yyvsp[(5) - (7)].expr)), ExprPtr((yyvsp[(7) - (7)].expr)));
    ;}
    break;

  case 71:
#line 505 "src/Parser/parser.y"
    { 
      auto ifBlock = ExprPtr(new ExprBlock(std::move(*(yyvsp[(6) - (11)].stmts))));
      auto elseBlock = ExprPtr(new ExprBlock(std::move(*(yyvsp[(10) - (11)].stmts))));
      (yyval.expr) = new IfExpr(ExprPtr((yyvsp[(3) - (11)].expr)), std::move(ifBlock), std::move(elseBlock)); 
      delete (yyvsp[(6) - (11)].stmts);
      delete (yyvsp[(10) - (11)].stmts);
  ;}
    break;

  case 72:
#line 512 "src/Parser/parser.y"
    { 
      auto ifBlock = ExprPtr(new ExprBlock(std::move(*(yyvsp[(6) - (7)].stmts))));
      (yyval.expr) = new IfExpr(ExprPtr((yyvsp[(3) - (7)].expr)), std::move(ifBlock), nullptr); 
      delete (yyvsp[(6) - (7)].stmts);
  ;}
    break;

  case 73:
#line 521 "src/Parser/parser.y"
    { (yyval.expr_list) = new std::vector<ExprPtr>(); ;}
    break;

  case 74:
#line 523 "src/Parser/parser.y"
    {
        (yyval.expr_list) = new std::vector<ExprPtr>();
        (yyval.expr_list)->emplace_back(ExprPtr((yyvsp[(1) - (1)].expr)));
      ;}
    break;

  case 75:
#line 528 "src/Parser/parser.y"
    {
        (yyvsp[(1) - (3)].expr_list)->emplace_back(ExprPtr((yyvsp[(3) - (3)].expr)));
        (yyval.expr_list) = (yyvsp[(1) - (3)].expr_list);
      ;}
    break;

  case 76:
#line 535 "src/Parser/parser.y"
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

  case 77:
#line 547 "src/Parser/parser.y"
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

  case 78:
#line 559 "src/Parser/parser.y"
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

  case 79:
#line 571 "src/Parser/parser.y"
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

  case 80:
#line 581 "src/Parser/parser.y"
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

  case 81:
#line 593 "src/Parser/parser.y"
    { 
        auto* pair = new std::pair<
            std::vector<std::unique_ptr<AttributeDecl>>,
            std::vector<std::unique_ptr<MethodDecl>>
        >();
        (yyval.mems) = pair;
    ;}
    break;

  case 82:
#line 600 "src/Parser/parser.y"
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

  case 83:
#line 612 "src/Parser/parser.y"
    {
        auto* pair = new std::pair<AttributeDecl*, MethodDecl*>();
        pair->first = (yyvsp[(1) - (2)].attribute_decl);
        (yyval.member_pair) = pair;
    ;}
    break;

  case 84:
#line 617 "src/Parser/parser.y"
    {
        auto* pair = new std::pair<AttributeDecl*, MethodDecl*>();
        pair->second = (yyvsp[(1) - (2)].method_decl);
        (yyval.member_pair) = pair;
    ;}
    break;

  case 85:
#line 625 "src/Parser/parser.y"
    {
        (yyval.attribute_decl) = static_cast<AttributeDecl*>(new AttributeDecl((yyvsp[(1) - (3)].str), ExprPtr((yyvsp[(3) - (3)].expr))));
        free((yyvsp[(1) - (3)].str));
    ;}
    break;

  case 86:
#line 632 "src/Parser/parser.y"
    {
        std::vector<std::string> args = (yyvsp[(3) - (6)].str_list) ? std::move(*(yyvsp[(3) - (6)].str_list)) : std::vector<std::string>();
        delete (yyvsp[(3) - (6)].str_list);
        (yyval.method_decl) = static_cast<MethodDecl*>(new MethodDecl((yyvsp[(1) - (6)].str), std::move(args), StmtPtr(new ExprStmt(ExprPtr((yyvsp[(6) - (6)].expr))))));
        free((yyvsp[(1) - (6)].str));
    ;}
    break;

  case 87:
#line 638 "src/Parser/parser.y"
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

  case 88:
#line 647 "src/Parser/parser.y"
    {
        std::vector<std::string> args;
        (yyval.method_decl) = static_cast<MethodDecl*>(new MethodDecl((yyvsp[(1) - (5)].str), std::move(args), StmtPtr(new ExprStmt(ExprPtr((yyvsp[(5) - (5)].expr))))));
        free((yyvsp[(1) - (5)].str));
    ;}
    break;

  case 89:
#line 652 "src/Parser/parser.y"
    {
        std::vector<std::string> args;
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*(yyvsp[(5) - (6)].stmts));
        delete (yyvsp[(5) - (6)].stmts);
        (yyval.method_decl) = static_cast<MethodDecl*>(new MethodDecl((yyvsp[(1) - (6)].str), std::move(args), StmtPtr(std::move(block))));
        free((yyvsp[(1) - (6)].str));
    ;}
    break;

  case 90:
#line 663 "src/Parser/parser.y"
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

  case 91:
#line 681 "src/Parser/parser.y"
    { (yyval.param_list) = new std::vector<std::pair<std::string, std::shared_ptr<TypeInfo>>>(); ;}
    break;

  case 92:
#line 682 "src/Parser/parser.y"
    { 
        (yyval.param_list) = new std::vector<std::pair<std::string, std::shared_ptr<TypeInfo>>>();
        (yyval.param_list)->push_back(*(yyvsp[(1) - (1)].param_with_type));
        delete (yyvsp[(1) - (1)].param_with_type);
    ;}
    break;

  case 93:
#line 687 "src/Parser/parser.y"
    {
        (yyvsp[(1) - (3)].param_list)->push_back(*(yyvsp[(3) - (3)].param_with_type));
        delete (yyvsp[(3) - (3)].param_with_type);
        (yyval.param_list) = (yyvsp[(1) - (3)].param_list);
    ;}
    break;

  case 94:
#line 695 "src/Parser/parser.y"
    { 
        (yyval.param_with_type) = new std::pair<std::string, std::shared_ptr<TypeInfo>>(
            std::string((yyvsp[(1) - (1)].str)), 
            std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown)
        );
        free((yyvsp[(1) - (1)].str));
    ;}
    break;

  case 95:
#line 702 "src/Parser/parser.y"
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
#line 2754 "src/Parser/parser.tab.cpp"
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


#line 712 "src/Parser/parser.y"

