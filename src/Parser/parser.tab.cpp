/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
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

#line 154 "src/Parser/parser.tab.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.tab.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_LET = 3,                        /* LET  */
  YYSYMBOL_IN = 4,                         /* IN  */
  YYSYMBOL_WHILE = 5,                      /* WHILE  */
  YYSYMBOL_FOR = 6,                        /* FOR  */
  YYSYMBOL_IDENT = 7,                      /* IDENT  */
  YYSYMBOL_IF = 8,                         /* IF  */
  YYSYMBOL_ELSE = 9,                       /* ELSE  */
  YYSYMBOL_ELIF = 10,                      /* ELIF  */
  YYSYMBOL_FUNCTION = 11,                  /* FUNCTION  */
  YYSYMBOL_ARROW = 12,                     /* ARROW  */
  YYSYMBOL_ASSIGN = 13,                    /* ASSIGN  */
  YYSYMBOL_ASSIGN_DESTRUCT = 14,           /* ASSIGN_DESTRUCT  */
  YYSYMBOL_TYPE = 15,                      /* TYPE  */
  YYSYMBOL_DOT = 16,                       /* DOT  */
  YYSYMBOL_SELF = 17,                      /* SELF  */
  YYSYMBOL_NEW = 18,                       /* NEW  */
  YYSYMBOL_INHERITS = 19,                  /* INHERITS  */
  YYSYMBOL_BASE = 20,                      /* BASE  */
  YYSYMBOL_IS = 21,                        /* IS  */
  YYSYMBOL_AS = 22,                        /* AS  */
  YYSYMBOL_TRUE = 23,                      /* TRUE  */
  YYSYMBOL_FALSE = 24,                     /* FALSE  */
  YYSYMBOL_NUMBER = 25,                    /* NUMBER  */
  YYSYMBOL_STRING = 26,                    /* STRING  */
  YYSYMBOL_PLUS = 27,                      /* PLUS  */
  YYSYMBOL_MINUS = 28,                     /* MINUS  */
  YYSYMBOL_MULT = 29,                      /* MULT  */
  YYSYMBOL_DIV = 30,                       /* DIV  */
  YYSYMBOL_MOD = 31,                       /* MOD  */
  YYSYMBOL_POW = 32,                       /* POW  */
  YYSYMBOL_CONCAT = 33,                    /* CONCAT  */
  YYSYMBOL_CONCAT_WS = 34,                 /* CONCAT_WS  */
  YYSYMBOL_LE = 35,                        /* LE  */
  YYSYMBOL_GE = 36,                        /* GE  */
  YYSYMBOL_EQ = 37,                        /* EQ  */
  YYSYMBOL_NEQ = 38,                       /* NEQ  */
  YYSYMBOL_LESS_THAN = 39,                 /* LESS_THAN  */
  YYSYMBOL_GREATER_THAN = 40,              /* GREATER_THAN  */
  YYSYMBOL_OR = 41,                        /* OR  */
  YYSYMBOL_AND = 42,                       /* AND  */
  YYSYMBOL_LPAREN = 43,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 44,                    /* RPAREN  */
  YYSYMBOL_LBRACE = 45,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 46,                    /* RBRACE  */
  YYSYMBOL_COMMA = 47,                     /* COMMA  */
  YYSYMBOL_SEMICOLON = 48,                 /* SEMICOLON  */
  YYSYMBOL_COLON = 49,                     /* COLON  */
  YYSYMBOL_INT = 50,                       /* INT  */
  YYSYMBOL_FLOAT = 51,                     /* FLOAT  */
  YYSYMBOL_BOOL = 52,                      /* BOOL  */
  YYSYMBOL_VOID = 53,                      /* VOID  */
  YYSYMBOL_UMINUS = 54,                    /* UMINUS  */
  YYSYMBOL_YYACCEPT = 55,                  /* $accept  */
  YYSYMBOL_input = 56,                     /* input  */
  YYSYMBOL_program = 57,                   /* program  */
  YYSYMBOL_binding_list = 58,              /* binding_list  */
  YYSYMBOL_binding = 59,                   /* binding  */
  YYSYMBOL_stmt = 60,                      /* stmt  */
  YYSYMBOL_stmt_list = 61,                 /* stmt_list  */
  YYSYMBOL_ident_list = 62,                /* ident_list  */
  YYSYMBOL_expr = 63,                      /* expr  */
  YYSYMBOL_if_expr = 64,                   /* if_expr  */
  YYSYMBOL_argument_list = 65,             /* argument_list  */
  YYSYMBOL_type_decl = 66,                 /* type_decl  */
  YYSYMBOL_member_list = 67,               /* member_list  */
  YYSYMBOL_member = 68,                    /* member  */
  YYSYMBOL_attribute = 69,                 /* attribute  */
  YYSYMBOL_method = 70,                    /* method  */
  YYSYMBOL_type = 71,                      /* type  */
  YYSYMBOL_param_list = 72,                /* param_list  */
  YYSYMBOL_param = 73                      /* param  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1281

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  55
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  19
/* YYNRULES -- Number of rules.  */
#define YYNRULES  109
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  279

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   309


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
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
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   166,   166,   170,   171,   177,   188,   193,   201,   208,
     219,   224,   248,   271,   291,   310,   326,   344,   357,   378,
     384,   385,   389,   396,   397,   398,   402,   407,   408,   412,
     416,   422,   427,   432,   437,   442,   447,   452,   457,   462,
     467,   472,   477,   482,   487,   492,   497,   502,   507,   518,
     524,   535,   541,   552,   558,   568,   573,   600,   608,   615,
     617,   624,   630,   638,   644,   648,   654,   660,   667,   672,
     677,   682,   694,   699,   707,   715,   724,   731,   746,   763,
     764,   769,   777,   798,   813,   828,   847,   868,   883,   890,
     902,   907,   915,   921,   931,   944,   957,   971,   987,  1003,
    1020,  1028,  1037,  1048,  1063,  1081,  1082,  1087,  1095,  1102
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "LET", "IN", "WHILE",
  "FOR", "IDENT", "IF", "ELSE", "ELIF", "FUNCTION", "ARROW", "ASSIGN",
  "ASSIGN_DESTRUCT", "TYPE", "DOT", "SELF", "NEW", "INHERITS", "BASE",
  "IS", "AS", "TRUE", "FALSE", "NUMBER", "STRING", "PLUS", "MINUS", "MULT",
  "DIV", "MOD", "POW", "CONCAT", "CONCAT_WS", "LE", "GE", "EQ", "NEQ",
  "LESS_THAN", "GREATER_THAN", "OR", "AND", "LPAREN", "RPAREN", "LBRACE",
  "RBRACE", "COMMA", "SEMICOLON", "COLON", "INT", "FLOAT", "BOOL", "VOID",
  "UMINUS", "$accept", "input", "program", "binding_list", "binding",
  "stmt", "stmt_list", "ident_list", "expr", "if_expr", "argument_list",
  "type_decl", "member_list", "member", "attribute", "method", "type",
  "param_list", "param", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-139)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-77)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -139,     9,   603,  -139,     6,   743,   -20,   103,    35,    82,
     137,    77,   143,   111,  -139,  -139,  -139,  -139,   743,   743,
    -139,   109,   948,  -139,  -139,    -9,    10,  -139,  1002,   151,
     743,   627,   743,   128,   116,   743,   130,   656,   161,  1029,
     234,  -139,   171,   743,   743,   743,   743,   743,   743,   743,
     743,   743,   743,   743,   743,   743,   743,   743,   743,  -139,
     743,   172,   743,   176,  -139,   180,  1164,  -139,  1164,   -37,
    1056,    72,   179,   194,  -139,  1164,   685,  -139,   -36,  -139,
    -139,   154,   975,   120,   135,   135,   115,   115,   115,   115,
    1241,  1241,   198,   198,   160,   160,   198,   198,  1191,  1216,
     814,  -139,   190,  1164,    79,  -139,   267,   743,  -139,   743,
     767,   155,    60,   -29,  -139,   -24,    39,    13,  -139,    40,
    -139,  -139,  -139,   743,   714,   743,   743,   743,   172,  -139,
    1083,  1164,  -139,   887,   172,   743,   172,    57,   194,   743,
    -139,    49,    52,  -139,  -139,   158,   159,  -139,  1164,  -139,
      41,  1164,   853,  1164,   196,   791,   291,   743,   167,  -139,
    1164,    -7,   743,  -139,   172,  -139,    46,    15,   208,  -139,
     743,    74,   172,  -139,  -139,  -139,   743,   743,  -139,  1164,
      94,  1164,   743,   743,  -139,  1164,   315,    -6,   173,  -139,
     127,    17,  1164,   155,    65,    96,   101,   203,  1164,  1164,
     339,   174,   189,  1110,  1164,   363,  -139,   743,  -139,  -139,
     743,  -139,  -139,   743,  -139,   172,    55,   215,    70,   743,
    -139,  -139,   743,   743,  -139,  1164,   387,    20,   105,    29,
    1164,   411,    84,   743,  -139,  -139,   743,  -139,   172,  1164,
     435,  1137,   921,  -139,  -139,   178,  -139,  -139,   743,  -139,
    1164,   459,  1164,   483,    85,  -139,   188,   743,  -139,  1164,
     507,  -139,  -139,   743,  -139,  -139,  1164,    30,  -139,  1164,
     531,   555,  -139,  -139,   226,   191,  -139,   579,  -139
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       3,     0,     2,     1,     0,     0,     0,    30,     0,     0,
       0,    68,     0,     0,    28,    29,    26,    27,     0,     0,
      20,     0,     0,    59,    19,     0,     0,     6,     0,     0,
       0,    79,     0,     0,     0,     0,     0,    79,    47,     0,
       0,     4,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     5,
       0,     0,     0,     0,    20,     0,    71,    49,    80,     0,
       0,   105,     0,   105,    88,    69,    79,    55,     0,    64,
      63,     0,     0,    65,    31,    32,    33,    34,    35,    36,
      37,    38,    41,    42,    43,    44,    39,    40,    46,    45,
       8,   104,     0,    56,     0,     7,     0,     0,    48,     0,
       0,   108,     0,     0,   106,     0,     0,     0,    51,     0,
      54,    21,    22,     0,    79,     0,     0,     0,     0,    60,
       0,    81,    20,     0,     0,     0,     0,     0,     0,    79,
      88,     0,     0,    87,    89,     0,     0,    50,    66,    53,
       0,    58,     9,     8,     0,     0,     0,     0,     0,   109,
      17,     0,     0,    20,     0,   107,     0,     0,     0,    88,
       0,    23,     0,    90,    91,    52,     0,     0,    20,    61,
      63,    72,     0,     0,    20,    14,     0,     0,     0,    84,
       0,     0,    92,    24,     0,     0,     0,     0,    57,     9,
       0,     0,     0,     0,    15,     0,    12,     0,    20,    88,
      79,    88,    85,     0,    20,     0,     0,     0,     0,     0,
      62,    20,     0,     0,    18,    13,     0,     0,     0,     0,
     100,     0,     0,     0,    20,    25,     0,    20,     0,    93,
       0,     0,    74,    11,    83,     0,    86,   102,     0,    20,
      94,     0,    95,     0,     0,    75,     0,     0,    88,   101,
       0,    97,    98,     0,    20,    20,    73,     0,   103,    96,
       0,     0,    82,    99,    78,     0,    20,     0,    77
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -139,  -139,  -139,  -139,   175,   241,   -52,  -139,    -2,  -139,
     -12,  -139,  -138,  -139,  -139,  -139,  -102,   -72,   106
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     1,     2,    26,    27,    81,    40,   195,    82,    23,
      69,    24,   117,   144,   145,   146,   102,   113,   114
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      22,   116,   167,    28,    60,   183,   207,   108,   120,     3,
     109,   109,   106,    25,    62,   137,    38,    39,   138,   139,
     142,   140,   142,    29,   142,    78,   154,   142,    66,    68,
      70,   191,   159,    75,   161,    68,   142,   142,   184,   208,
      61,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,    63,   100,   143,
     103,   189,   187,   212,   119,   170,   244,   233,   168,   162,
     197,   227,   135,   229,    68,   246,   272,   213,    32,   111,
     156,   193,   236,   141,   147,   175,   138,   109,   109,    33,
     188,    35,   127,   109,   169,   171,   248,   263,   -76,   196,
     234,   172,   163,   201,   202,   130,   164,   131,   133,   136,
     214,   186,   150,   232,   215,   237,   112,    30,   194,   238,
     267,   148,    68,   151,   152,   153,   200,   166,   128,   249,
     264,    42,   205,   160,   123,    72,   254,    68,   -76,   -76,
     216,   -76,   -76,   217,    34,   218,    31,    48,   138,   245,
      36,    42,   109,   179,    37,   181,   226,    41,    65,    73,
     185,    74,   231,   124,    45,    46,    47,    48,   192,   240,
     210,    71,   211,    76,   198,   199,    42,    42,    83,   101,
     203,   204,   251,   104,   107,   253,   115,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,   260,   228,    55,
      56,   111,   121,   126,   134,   225,   173,   174,    68,   177,
     182,   230,   270,   271,    42,   190,   219,   239,   209,   221,
     241,   242,   235,   258,   277,    43,    44,    45,    46,    47,
      48,   250,   222,   265,   252,   275,   276,     4,   105,     5,
       6,     7,     8,    21,   165,     9,   259,     0,     0,    10,
       0,    11,    12,     0,    13,   266,     0,    14,    15,    16,
      17,   269,    18,     0,     0,     0,     0,     0,     0,     0,
       4,     0,     5,     6,     7,     8,     0,    19,     9,    20,
      80,     0,    10,     0,    11,    12,     0,    13,     0,     0,
      14,    15,    16,    17,     4,    18,     5,     6,     7,     8,
       0,     0,     9,     0,     0,     0,    10,     0,    11,    12,
      19,    13,    20,   129,    14,    15,    16,    17,     4,    18,
       5,     6,     7,     8,     0,     0,     9,     0,     0,     0,
      10,     0,    11,    12,    19,    13,    20,   180,    14,    15,
      16,    17,     4,    18,     5,     6,     7,     8,     0,     0,
       9,     0,     0,     0,    10,     0,    11,    12,    19,    13,
      20,   206,    14,    15,    16,    17,     4,    18,     5,     6,
       7,     8,     0,     0,     9,     0,     0,     0,    10,     0,
      11,    12,    19,    13,    20,   220,    14,    15,    16,    17,
       4,    18,     5,     6,     7,     8,     0,     0,     9,     0,
       0,     0,    10,     0,    11,    12,    19,    13,    20,   224,
      14,    15,    16,    17,     4,    18,     5,     6,     7,     8,
       0,     0,     9,     0,     0,     0,    10,     0,    11,    12,
      19,    13,    20,   243,    14,    15,    16,    17,     4,    18,
       5,     6,     7,     8,     0,     0,     9,     0,     0,     0,
      10,     0,    11,    12,    19,    13,    20,   247,    14,    15,
      16,    17,     4,    18,     5,     6,     7,     8,     0,     0,
       9,     0,     0,     0,    10,     0,    11,    12,    19,    13,
      20,   255,    14,    15,    16,    17,     4,    18,     5,     6,
       7,     8,     0,     0,     9,     0,     0,     0,    10,     0,
      11,    12,    19,    13,    20,   261,    14,    15,    16,    17,
       4,    18,     5,     6,     7,     8,     0,     0,     9,     0,
       0,     0,    10,     0,    11,    12,    19,    13,    20,   262,
      14,    15,    16,    17,     4,    18,     5,     6,     7,     8,
       0,     0,     9,     0,     0,     0,    10,     0,    11,    12,
      19,    13,    20,   268,    14,    15,    16,    17,     4,    18,
       5,     6,     7,     8,     0,     0,     9,     0,     0,     0,
      10,     0,    11,    12,    19,    13,    20,   273,    14,    15,
      16,    17,     4,    18,     5,     6,     7,     8,     0,     0,
       9,     0,     0,     0,    10,     0,    11,    12,    19,    13,
      20,   274,    14,    15,    16,    17,     4,    18,     5,     6,
       7,     8,     0,     0,     9,     0,     0,     0,    10,     0,
      11,    12,    19,    13,    20,   278,    14,    15,    16,    17,
       4,    18,     5,     6,     7,     8,     0,     0,     0,     0,
       0,     0,     0,     0,    11,    12,    19,    13,    20,     0,
      14,    15,    16,    17,     0,    18,     0,     0,     0,     4,
       0,     5,     6,     7,     8,     0,     0,     0,     0,     0,
      19,    67,    20,    11,    12,     0,    13,     0,     0,    14,
      15,    16,    17,     0,    18,     0,     0,     0,     4,     0,
       5,     6,     7,     8,     0,     0,     0,     0,     0,    19,
      77,    20,    11,    12,     0,    13,     0,     0,    14,    15,
      16,    17,     0,    18,     0,     0,     0,     4,     0,     5,
       6,     7,     8,     0,     0,     0,     0,     0,    19,   118,
      20,    11,    12,     0,    13,     0,     0,    14,    15,    16,
      17,     0,    18,     0,     0,     0,     4,     0,     5,     6,
       7,     8,     0,     0,     0,     0,     0,    19,   149,    20,
      11,    12,     0,    13,     0,     0,    14,    15,    16,    17,
       4,    18,     5,     6,     7,     8,     0,     0,     0,     0,
       0,     0,     0,     0,    11,    12,    19,    13,    20,     0,
      14,    15,    16,    17,     4,    18,     5,     6,     7,     8,
       0,     0,     0,     0,     0,     0,     0,     0,    11,    12,
      19,    13,   132,     0,    14,    15,    16,    17,   125,    18,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      42,     0,     0,     0,    19,     0,   178,     0,     0,     0,
       0,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,   176,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    42,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,   157,   158,     0,     0,
       0,     0,     0,    42,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
     257,     0,     0,     0,     0,     0,     0,    42,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    42,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    42,     0,     0,     0,     0,    59,     0,     0,     0,
       0,     0,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    42,     0,
       0,     0,     0,   122,     0,     0,     0,     0,     0,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    42,     0,    64,     0,     0,
       0,     0,     0,     0,     0,     0,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    42,    79,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    42,
     110,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      43,    44,    45,    46,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    42,   155,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    42,   223,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      42,   256,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    42,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    42,    58,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    43,    44,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    42,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    43,    44,
      45,    46,    47,    48,     0,     0,    51,    52,     0,     0,
      55,    56
};

static const yytype_int16 yycheck[] =
{
       2,    73,   140,     5,    13,    12,    12,    44,    44,     0,
      47,    47,    64,     7,     4,    44,    18,    19,    47,    43,
       7,    45,     7,    43,     7,    37,   128,     7,    30,    31,
      32,   169,   134,    35,   136,    37,     7,     7,    45,    45,
      49,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    47,    60,    46,
      62,    46,   164,    46,    76,    13,    46,    12,    19,    12,
     172,   209,    12,   211,    76,    46,    46,    12,    43,     7,
     132,     7,    12,    44,    44,    44,    47,    47,    47,     7,
      44,    14,    13,    47,    45,    43,    12,    12,     4,   171,
      45,    49,    45,     9,    10,   107,    49,   109,   110,    49,
      45,   163,   124,   215,    49,    45,    44,    14,    44,    49,
     258,   123,   124,   125,   126,   127,   178,   139,    49,    45,
      45,    16,   184,   135,    14,    19,   238,   139,    44,    45,
      44,    47,    48,    47,     7,    44,    43,    32,    47,    44,
       7,    16,    47,   155,    43,   157,   208,    48,     7,    43,
     162,    45,   214,    43,    29,    30,    31,    32,   170,   221,
      43,    43,    45,    43,   176,   177,    16,    16,     7,     7,
     182,   183,   234,     7,     4,   237,     7,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,   249,   210,    39,
      40,     7,    48,    13,    49,   207,    48,    48,   210,    13,
      43,   213,   264,   265,    16,     7,    13,   219,    45,    45,
     222,   223,     7,    45,   276,    27,    28,    29,    30,    31,
      32,   233,    43,    45,   236,     9,    45,     3,    63,     5,
       6,     7,     8,     2,   138,    11,   248,    -1,    -1,    15,
      -1,    17,    18,    -1,    20,   257,    -1,    23,    24,    25,
      26,   263,    28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,    -1,     5,     6,     7,     8,    -1,    43,    11,    45,
      46,    -1,    15,    -1,    17,    18,    -1,    20,    -1,    -1,
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
       3,    28,     5,     6,     7,     8,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    17,    18,    43,    20,    45,    -1,
      23,    24,    25,    26,    -1,    28,    -1,    -1,    -1,     3,
      -1,     5,     6,     7,     8,    -1,    -1,    -1,    -1,    -1,
      43,    44,    45,    17,    18,    -1,    20,    -1,    -1,    23,
      24,    25,    26,    -1,    28,    -1,    -1,    -1,     3,    -1,
       5,     6,     7,     8,    -1,    -1,    -1,    -1,    -1,    43,
      44,    45,    17,    18,    -1,    20,    -1,    -1,    23,    24,
      25,    26,    -1,    28,    -1,    -1,    -1,     3,    -1,     5,
       6,     7,     8,    -1,    -1,    -1,    -1,    -1,    43,    44,
      45,    17,    18,    -1,    20,    -1,    -1,    23,    24,    25,
      26,    -1,    28,    -1,    -1,    -1,     3,    -1,     5,     6,
       7,     8,    -1,    -1,    -1,    -1,    -1,    43,    44,    45,
      17,    18,    -1,    20,    -1,    -1,    23,    24,    25,    26,
       3,    28,     5,     6,     7,     8,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    17,    18,    43,    20,    45,    -1,
      23,    24,    25,    26,     3,    28,     5,     6,     7,     8,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    17,    18,
      43,    20,    45,    -1,    23,    24,    25,    26,     4,    28,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      16,    -1,    -1,    -1,    43,    -1,    45,    -1,    -1,    -1,
      -1,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,     4,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    16,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,     9,    10,    -1,    -1,
      -1,    -1,    -1,    16,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
       9,    -1,    -1,    -1,    -1,    -1,    -1,    16,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    16,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    27,    28,    29,    30,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    16,    -1,    -1,    -1,    -1,    48,    -1,    -1,    -1,
      -1,    -1,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    16,    -1,
      -1,    -1,    -1,    48,    -1,    -1,    -1,    -1,    -1,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    16,    -1,    45,    -1,    -1,
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
      40,    41,    42,    16,    44,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      16,    44,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    16,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    16,    42,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    16,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    27,    28,
      29,    30,    31,    32,    -1,    -1,    35,    36,    -1,    -1,
      39,    40
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
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
      63,    71,    12,    45,    49,    73,    65,    67,    19,    45,
      13,    43,    49,    48,    48,    44,     4,    13,    45,    63,
      46,    63,    43,    12,    45,    63,    61,    71,    44,    46,
       7,    67,    63,     7,    44,    62,    72,    71,    63,    63,
      61,     9,    10,    63,    63,    61,    46,    12,    45,    45,
      43,    45,    46,    12,    45,    49,    44,    47,    44,    13,
      46,    45,    43,    44,    46,    63,    61,    67,    65,    67,
      63,    61,    71,    12,    45,     7,    12,    45,    49,    63,
      61,    63,    63,    46,    46,    44,    46,    46,    12,    45,
      63,    61,    63,    61,    71,    46,    44,     9,    45,    63,
      61,    46,    46,    12,    45,    45,    63,    67,    46,    63,
      61,    61,    46,    46,    46,     9,    45,    61,    46
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    55,    56,    57,    57,    57,    58,    58,    59,    59,
      60,    60,    60,    60,    60,    60,    60,    60,    60,    60,
      61,    61,    61,    62,    62,    62,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
      63,    63,    64,    64,    64,    64,    64,    64,    64,    65,
      65,    65,    66,    66,    66,    66,    66,    66,    67,    67,
      68,    68,    69,    69,    70,    70,    70,    70,    70,    70,
      70,    70,    70,    70,    71,    72,    72,    72,    73,    73
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     3,     3,     1,     3,     3,     5,
       1,    10,     8,     9,     7,     8,     7,     6,     9,     1,
       0,     3,     3,     0,     1,     3,     1,     1,     1,     1,
       1,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     4,     3,
       5,     4,     6,     5,     4,     3,     4,     8,     6,     1,
       5,     7,     9,     3,     3,     3,     5,     6,     1,     3,
       3,     3,     7,    12,    10,    11,     7,    18,    14,     0,
       1,     3,    13,    10,     7,     8,    10,     5,     0,     2,
       2,     2,     3,     5,     6,     6,     8,     7,     7,     9,
       5,     7,     6,     8,     1,     0,     1,     3,     1,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]));
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
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


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
  YYLTYPE *yylloc;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
  if (!yypact_value_is_default (yyn))
    {
      /* Start YYX at -YYN if negative to avoid negative indexes in
         YYCHECK.  In other words, skip the first -YYN actions for
         this state because they are default actions.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;
      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yyx;
      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
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
            else
              goto append;

          append:
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

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
      case N:                               \
        yyformat = S;                       \
        break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
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
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* input: program  */
#line 166 "src/Parser/parser.y"
            { rootAST = (yyvsp[0].prog); }
#line 1970 "src/Parser/parser.tab.cpp"
    break;

  case 3: /* program: %empty  */
#line 170 "src/Parser/parser.y"
                      { (yyval.prog) = new Program(); }
#line 1976 "src/Parser/parser.tab.cpp"
    break;

  case 4: /* program: program stmt SEMICOLON  */
#line 172 "src/Parser/parser.y"
    {
      setStmtLocation((yyvsp[-1].stmt), (yylsp[-1]));
      (yyvsp[-2].prog)->stmts.emplace_back( StmtPtr((yyvsp[-1].stmt)) );
      (yyval.prog) = (yyvsp[-2].prog);
    }
#line 1986 "src/Parser/parser.tab.cpp"
    break;

  case 5: /* program: program expr SEMICOLON  */
#line 178 "src/Parser/parser.y"
    {
      auto exprStmt = new ExprStmt(ExprPtr((yyvsp[-1].expr)));
      setStmtLocation(exprStmt, (yylsp[-1]));
      setExprLocation((yyvsp[-1].expr), (yylsp[-1]));
      (yyvsp[-2].prog)->stmts.emplace_back( StmtPtr(exprStmt) );
      (yyval.prog) = (yyvsp[-2].prog);
    }
#line 1998 "src/Parser/parser.tab.cpp"
    break;

  case 6: /* binding_list: binding  */
#line 188 "src/Parser/parser.y"
            {
          (yyval.bindings_with_types) = new std::vector<std::pair<std::string, std::pair<Expr*, std::shared_ptr<TypeInfo>>>>();
          (yyval.bindings_with_types)->push_back(std::make_pair((yyvsp[0].binding_with_type)->first, std::make_pair((yyvsp[0].binding_with_type)->second.first, (yyvsp[0].binding_with_type)->second.second)));
          delete (yyvsp[0].binding_with_type);
      }
#line 2008 "src/Parser/parser.tab.cpp"
    break;

  case 7: /* binding_list: binding_list COMMA binding  */
#line 193 "src/Parser/parser.y"
                                 {
          (yyvsp[-2].bindings_with_types)->push_back(std::make_pair((yyvsp[0].binding_with_type)->first, std::make_pair((yyvsp[0].binding_with_type)->second.first, (yyvsp[0].binding_with_type)->second.second)));
          delete (yyvsp[0].binding_with_type);
          (yyval.bindings_with_types) = (yyvsp[-2].bindings_with_types);
      }
#line 2018 "src/Parser/parser.tab.cpp"
    break;

  case 8: /* binding: IDENT ASSIGN expr  */
#line 201 "src/Parser/parser.y"
                        {
          (yyval.binding_with_type) = new std::pair<std::string, std::pair<Expr*, std::shared_ptr<TypeInfo>>>(
              std::string((yyvsp[-2].str)), 
              std::make_pair((yyvsp[0].expr), std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown))
          );
          free((yyvsp[-2].str));
      }
#line 2030 "src/Parser/parser.tab.cpp"
    break;

  case 9: /* binding: IDENT COLON type ASSIGN expr  */
#line 208 "src/Parser/parser.y"
                                     {
          (yyval.binding_with_type) = new std::pair<std::string, std::pair<Expr*, std::shared_ptr<TypeInfo>>>(
              std::string((yyvsp[-4].str)), 
              std::make_pair((yyvsp[0].expr), *(yyvsp[-2].type_info))
          );
          free((yyvsp[-4].str));
          delete (yyvsp[-2].type_info);
      }
#line 2043 "src/Parser/parser.tab.cpp"
    break;

  case 10: /* stmt: expr  */
#line 219 "src/Parser/parser.y"
          {
        auto exprStmt = new ExprStmt(ExprPtr((yyvsp[(1) - (1)].expr)));
        setStmtLocation(exprStmt, (yylsp[0]));
        (yyval.stmt) = exprStmt;
    }
#line 2053 "src/Parser/parser.tab.cpp"
    break;

  case 11: /* stmt: FUNCTION IDENT LPAREN param_list RPAREN COLON type LBRACE stmt_list RBRACE  */
#line 224 "src/Parser/parser.y"
                                                                                 { 
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *(yyvsp[-6].param_list)) {
            args.push_back(param.first);
            param_types.push_back(param.second);
        }
        
        // Crear un bloque de expresiones y envolverlo en un ExprStmt
        auto exprBlock = new ExprBlock(std::move(*(yyvsp[-1].stmts)));
        auto exprStmt = new ExprStmt(ExprPtr(exprBlock));
        
        auto funcDecl = static_cast<Stmt*>(new FunctionDecl(
            std::string((yyvsp[-8].str)),
            std::move(args),
            StmtPtr(exprStmt),
            std::move(param_types),
            *(yyvsp[-3].type_info)
        ));
        setStmtLocation(funcDecl, (yylsp[-8]));
        (yyval.stmt) = funcDecl;
        delete (yyvsp[-6].param_list); delete (yyvsp[-3].type_info); delete (yyvsp[-1].stmts);
        free((yyvsp[-8].str));
    }
#line 2082 "src/Parser/parser.tab.cpp"
    break;

  case 12: /* stmt: FUNCTION IDENT LPAREN param_list RPAREN LBRACE stmt_list RBRACE  */
#line 248 "src/Parser/parser.y"
                                                                      {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        
        for (const auto& param : *(yyvsp[-4].param_list)) {
            params.push_back(param.first);
            param_types.push_back(param.second);
        }
        
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*(yyvsp[-1].stmts));
        delete (yyvsp[-1].stmts);
        
        (yyval.stmt) = static_cast<Stmt*>(new FunctionDecl(
            std::string((yyvsp[-6].str)), 
            std::move(params), 
            std::move(block), 
            std::move(param_types)
        ));
        delete (yyvsp[-4].param_list);
        free((yyvsp[-6].str));
    }
#line 2109 "src/Parser/parser.tab.cpp"
    break;

  case 13: /* stmt: FUNCTION IDENT LPAREN param_list RPAREN COLON type ARROW expr  */
#line 271 "src/Parser/parser.y"
                                                                    {
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *(yyvsp[-5].param_list)) {
            args.push_back(param.first);
            param_types.push_back(param.second);
        }
        
        auto funcDecl = static_cast<Stmt*>(new FunctionDecl(
            std::string((yyvsp[-7].str)), 
            std::move(args), 
            std::make_unique<ExprStmt>(ExprPtr((yyvsp[0].expr))), 
            std::move(param_types),
            *(yyvsp[-2].type_info)
        ));
        setStmtLocation(funcDecl, (yylsp[-7]));
        (yyval.stmt) = funcDecl;
        delete (yyvsp[-5].param_list); delete (yyvsp[-2].type_info);
        free((yyvsp[-7].str));
    }
#line 2134 "src/Parser/parser.tab.cpp"
    break;

  case 14: /* stmt: FUNCTION IDENT LPAREN param_list RPAREN ARROW expr  */
#line 291 "src/Parser/parser.y"
                                                         {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        
        for (const auto& param : *(yyvsp[-3].param_list)) {
            params.push_back(param.first);
            param_types.push_back(param.second);
        }
        
        (yyval.stmt) = static_cast<Stmt*>(new FunctionDecl(
            std::string((yyvsp[-5].str)), 
            std::move(params), 
            std::make_unique<ExprStmt>(ExprPtr((yyvsp[0].expr))), 
            std::move(param_types)
        ));
        delete (yyvsp[-3].param_list);
        free((yyvsp[-5].str));
    }
#line 2157 "src/Parser/parser.tab.cpp"
    break;

  case 15: /* stmt: FUNCTION IDENT LPAREN RPAREN COLON type ARROW expr  */
#line 310 "src/Parser/parser.y"
                                                         {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        
        auto funcDecl = static_cast<Stmt*>(new FunctionDecl(
            std::string((yyvsp[-6].str)), 
            std::move(params), 
            std::make_unique<ExprStmt>(ExprPtr((yyvsp[0].expr))), 
            std::move(param_types),
            *(yyvsp[-2].type_info)
        ));
        setStmtLocation(funcDecl, (yylsp[-6]));
        (yyval.stmt) = funcDecl;
        delete (yyvsp[-2].type_info);
        free((yyvsp[-6].str));
    }
#line 2178 "src/Parser/parser.tab.cpp"
    break;

  case 16: /* stmt: FUNCTION IDENT LPAREN param_list RPAREN ARROW expr  */
#line 326 "src/Parser/parser.y"
                                                         {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        
        for (const auto& param : *(yyvsp[-3].param_list)) {
            params.push_back(param.first);
            param_types.push_back(param.second);
        }
        
        (yyval.stmt) = static_cast<Stmt*>(new FunctionDecl(
            std::string((yyvsp[-5].str)), 
            std::move(params), 
            std::make_unique<ExprStmt>(ExprPtr((yyvsp[0].expr))), 
            std::move(param_types)
        ));
        delete (yyvsp[-3].param_list);
        free((yyvsp[-5].str));
    }
#line 2201 "src/Parser/parser.tab.cpp"
    break;

  case 17: /* stmt: FUNCTION IDENT LPAREN RPAREN ARROW expr  */
#line 344 "src/Parser/parser.y"
                                              {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        
        (yyval.stmt) = static_cast<Stmt*>(new FunctionDecl(
            std::string((yyvsp[-4].str)), 
            std::move(params), 
            std::make_unique<ExprStmt>(ExprPtr((yyvsp[0].expr))), 
            std::move(param_types)
        ));
        free((yyvsp[-4].str));
    }
#line 2218 "src/Parser/parser.tab.cpp"
    break;

  case 18: /* stmt: FUNCTION IDENT LPAREN RPAREN COLON type LBRACE stmt_list RBRACE  */
#line 357 "src/Parser/parser.y"
                                                                      {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*(yyvsp[-1].stmts));
        delete (yyvsp[-1].stmts);
        
        auto funcDecl = static_cast<Stmt*>(new FunctionDecl(
            std::string((yyvsp[-7].str)), 
            std::move(params), 
            std::move(block), 
            std::move(param_types),
            *(yyvsp[-3].type_info)
        ));
        setStmtLocation(funcDecl, (yylsp[-7]));
        (yyval.stmt) = funcDecl;
        delete (yyvsp[-3].type_info);
        free((yyvsp[-7].str));
    }
#line 2243 "src/Parser/parser.tab.cpp"
    break;

  case 20: /* stmt_list: %empty  */
#line 384 "src/Parser/parser.y"
                 { (yyval.stmts) = new std::vector<StmtPtr>(); }
#line 2249 "src/Parser/parser.tab.cpp"
    break;

  case 21: /* stmt_list: stmt_list stmt SEMICOLON  */
#line 385 "src/Parser/parser.y"
                             { 
      (yyvsp[-2].stmts)->push_back(StmtPtr((yyvsp[-1].stmt))); 
      (yyval.stmts) = (yyvsp[-2].stmts); 
    }
#line 2258 "src/Parser/parser.tab.cpp"
    break;

  case 22: /* stmt_list: stmt_list expr SEMICOLON  */
#line 389 "src/Parser/parser.y"
                             { 
      (yyvsp[-2].stmts)->push_back(StmtPtr(new ExprStmt(ExprPtr((yyvsp[-1].expr))))); 
      (yyval.stmts) = (yyvsp[-2].stmts); 
    }
#line 2267 "src/Parser/parser.tab.cpp"
    break;

  case 23: /* ident_list: %empty  */
#line 396 "src/Parser/parser.y"
                   { (yyval.str_list) = new std::vector<std::string>(); }
#line 2273 "src/Parser/parser.tab.cpp"
    break;

  case 24: /* ident_list: IDENT  */
#line 397 "src/Parser/parser.y"
            { (yyval.str_list) = new std::vector<std::string>(); (yyval.str_list)->push_back((yyvsp[0].str)); free((yyvsp[0].str)); }
#line 2279 "src/Parser/parser.tab.cpp"
    break;

  case 25: /* ident_list: ident_list COMMA IDENT  */
#line 398 "src/Parser/parser.y"
                             { (yyvsp[-2].str_list)->push_back((yyvsp[0].str)); free((yyvsp[0].str)); (yyval.str_list) = (yyvsp[-2].str_list); }
#line 2285 "src/Parser/parser.tab.cpp"
    break;

  case 26: /* expr: NUMBER  */
#line 402 "src/Parser/parser.y"
           { 
        setExprLocation((yyvsp[0].expr), (yylsp[0]));
        std::cout << "Evaluated expression statement" << std::endl; 
        (yyval.expr) = (yyvsp[0].expr); 
    }
#line 2295 "src/Parser/parser.tab.cpp"
    break;

  case 27: /* expr: STRING  */
#line 407 "src/Parser/parser.y"
           { (yyval.expr) = (yyvsp[0].expr); }
#line 2301 "src/Parser/parser.tab.cpp"
    break;

  case 28: /* expr: TRUE  */
#line 408 "src/Parser/parser.y"
         { 
        setExprLocation((yyvsp[0].expr), (yylsp[0]));
        (yyval.expr) = (yyvsp[0].expr); 
    }
#line 2310 "src/Parser/parser.tab.cpp"
    break;

  case 29: /* expr: FALSE  */
#line 412 "src/Parser/parser.y"
          { 
        setExprLocation((yyvsp[0].expr), (yylsp[0]));
        (yyval.expr) = (yyvsp[0].expr); 
    }
#line 2319 "src/Parser/parser.tab.cpp"
    break;

  case 30: /* expr: IDENT  */
#line 416 "src/Parser/parser.y"
          { 
        auto varExpr = new VariableExpr((yyvsp[0].str));
        setExprLocation(varExpr, (yylsp[0]));
        (yyval.expr) = varExpr; 
        free((yyvsp[0].str)); 
    }
#line 2330 "src/Parser/parser.tab.cpp"
    break;

  case 31: /* expr: expr PLUS expr  */
#line 422 "src/Parser/parser.y"
                   { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_ADD, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
        setExprLocation(binExpr, (yylsp[-1]));
        (yyval.expr) = binExpr; 
    }
#line 2340 "src/Parser/parser.tab.cpp"
    break;

  case 32: /* expr: expr MINUS expr  */
#line 427 "src/Parser/parser.y"
                    { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_SUB, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
        setExprLocation(binExpr, (yylsp[-1]));
        (yyval.expr) = binExpr; 
    }
#line 2350 "src/Parser/parser.tab.cpp"
    break;

  case 33: /* expr: expr MULT expr  */
#line 432 "src/Parser/parser.y"
                   { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_MUL, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
        setExprLocation(binExpr, (yylsp[-1]));
        (yyval.expr) = binExpr; 
    }
#line 2360 "src/Parser/parser.tab.cpp"
    break;

  case 34: /* expr: expr DIV expr  */
#line 437 "src/Parser/parser.y"
                  { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_DIV, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
        setExprLocation(binExpr, (yylsp[-1]));
        (yyval.expr) = binExpr; 
    }
#line 2370 "src/Parser/parser.tab.cpp"
    break;

  case 35: /* expr: expr MOD expr  */
#line 442 "src/Parser/parser.y"
                  { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_MOD, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
        setExprLocation(binExpr, (yylsp[-1]));
        (yyval.expr) = binExpr; 
    }
#line 2380 "src/Parser/parser.tab.cpp"
    break;

  case 36: /* expr: expr POW expr  */
#line 447 "src/Parser/parser.y"
                  { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_POW, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
        setExprLocation(binExpr, (yylsp[-1]));
        (yyval.expr) = binExpr; 
    }
#line 2390 "src/Parser/parser.tab.cpp"
    break;

  case 37: /* expr: expr CONCAT expr  */
#line 452 "src/Parser/parser.y"
                     { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_CONCAT, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
        setExprLocation(binExpr, (yylsp[-1]));
        (yyval.expr) = binExpr; 
    }
#line 2400 "src/Parser/parser.tab.cpp"
    break;

  case 38: /* expr: expr CONCAT_WS expr  */
#line 457 "src/Parser/parser.y"
                        { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_CONCAT_WS, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
        setExprLocation(binExpr, (yylsp[-1]));
        (yyval.expr) = binExpr; 
    }
#line 2410 "src/Parser/parser.tab.cpp"
    break;

  case 39: /* expr: expr LESS_THAN expr  */
#line 462 "src/Parser/parser.y"
                        { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_LT, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
        setExprLocation(binExpr, (yylsp[-1]));
        (yyval.expr) = binExpr; 
    }
#line 2420 "src/Parser/parser.tab.cpp"
    break;

  case 40: /* expr: expr GREATER_THAN expr  */
#line 467 "src/Parser/parser.y"
                           { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_GT, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
        setExprLocation(binExpr, (yylsp[-1]));
        (yyval.expr) = binExpr; 
    }
#line 2430 "src/Parser/parser.tab.cpp"
    break;

  case 41: /* expr: expr LE expr  */
#line 472 "src/Parser/parser.y"
                 { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_LE, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
        setExprLocation(binExpr, (yylsp[-1]));
        (yyval.expr) = binExpr; 
    }
#line 2440 "src/Parser/parser.tab.cpp"
    break;

  case 42: /* expr: expr GE expr  */
#line 477 "src/Parser/parser.y"
                 { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_GE, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
        setExprLocation(binExpr, (yylsp[-1]));
        (yyval.expr) = binExpr; 
    }
#line 2450 "src/Parser/parser.tab.cpp"
    break;

  case 43: /* expr: expr EQ expr  */
#line 482 "src/Parser/parser.y"
                 { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_EQ, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
        setExprLocation(binExpr, (yylsp[-1]));
        (yyval.expr) = binExpr; 
    }
#line 2460 "src/Parser/parser.tab.cpp"
    break;

  case 44: /* expr: expr NEQ expr  */
#line 487 "src/Parser/parser.y"
                  { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_NEQ, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
        setExprLocation(binExpr, (yylsp[-1]));
        (yyval.expr) = binExpr; 
    }
#line 2470 "src/Parser/parser.tab.cpp"
    break;

  case 45: /* expr: expr AND expr  */
#line 492 "src/Parser/parser.y"
                  { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_AND, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
        setExprLocation(binExpr, (yylsp[-1]));
        (yyval.expr) = binExpr; 
    }
#line 2480 "src/Parser/parser.tab.cpp"
    break;

  case 46: /* expr: expr OR expr  */
#line 497 "src/Parser/parser.y"
                 { 
        auto binExpr = new BinaryExpr(BinaryExpr::OP_OR, ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
        setExprLocation(binExpr, (yylsp[-1]));
        (yyval.expr) = binExpr; 
    }
#line 2490 "src/Parser/parser.tab.cpp"
    break;

  case 47: /* expr: MINUS expr  */
#line 502 "src/Parser/parser.y"
                            { 
        auto unaryExpr = new UnaryExpr(UnaryExpr::OP_NEG, ExprPtr((yyvsp[0].expr)));
        setExprLocation(unaryExpr, (yylsp[-1]));
        (yyval.expr) = unaryExpr; 
    }
#line 2500 "src/Parser/parser.tab.cpp"
    break;

  case 48: /* expr: IDENT LPAREN argument_list RPAREN  */
#line 507 "src/Parser/parser.y"
                                      { 
      std::vector<ExprPtr> args;
      for (auto& arg : *(yyvsp[-1].expr_list)) {
          args.push_back(std::move(arg));
      }
      auto callExpr = new CallExpr((yyvsp[-3].str), std::move(args));
      setExprLocation(callExpr, (yylsp[-3]));
      (yyval.expr) = callExpr; 
      free((yyvsp[-3].str)); 
      delete (yyvsp[-1].expr_list); 
  }
#line 2516 "src/Parser/parser.tab.cpp"
    break;

  case 49: /* expr: IDENT LPAREN RPAREN  */
#line 518 "src/Parser/parser.y"
                        { 
      auto callExpr = new CallExpr((yyvsp[-2].str), std::vector<ExprPtr>());
      setExprLocation(callExpr, (yylsp[-2]));
      (yyval.expr) = callExpr; 
      free((yyvsp[-2].str)); 
  }
#line 2527 "src/Parser/parser.tab.cpp"
    break;

  case 50: /* expr: NEW IDENT LPAREN argument_list RPAREN  */
#line 524 "src/Parser/parser.y"
                                          {
      std::vector<ExprPtr> args;
      for (auto& arg : *(yyvsp[-1].expr_list)) {
          args.push_back(std::move(arg));
      }
      auto newExpr = new NewExpr((yyvsp[-3].str), std::move(args));
      setExprLocation(newExpr, (yylsp[-3]));
      (yyval.expr) = newExpr;
      free((yyvsp[-3].str));
      delete (yyvsp[-1].expr_list);
  }
#line 2543 "src/Parser/parser.tab.cpp"
    break;

  case 51: /* expr: NEW IDENT LPAREN RPAREN  */
#line 535 "src/Parser/parser.y"
                            {
      auto newExpr = new NewExpr((yyvsp[-2].str), std::vector<ExprPtr>());
      setExprLocation(newExpr, (yylsp[-2]));
      (yyval.expr) = newExpr;
      free((yyvsp[-2].str));
  }
#line 2554 "src/Parser/parser.tab.cpp"
    break;

  case 52: /* expr: expr DOT IDENT LPAREN argument_list RPAREN  */
#line 541 "src/Parser/parser.y"
                                               {
      std::vector<ExprPtr> args;
      for (auto& arg : *(yyvsp[-1].expr_list)) {
          args.push_back(std::move(arg));
      }
      auto methodCall = new MethodCallExpr(ExprPtr((yyvsp[-5].expr)), (yyvsp[-3].str), std::move(args));
      setExprLocation(methodCall, (yylsp[-3]));
      (yyval.expr) = methodCall;
      free((yyvsp[-3].str));
      delete (yyvsp[-1].expr_list);
  }
#line 2570 "src/Parser/parser.tab.cpp"
    break;

  case 53: /* expr: expr DOT IDENT LPAREN RPAREN  */
#line 552 "src/Parser/parser.y"
                                 {
      auto methodCall = new MethodCallExpr(ExprPtr((yyvsp[-4].expr)), (yyvsp[-2].str), std::vector<ExprPtr>());
      setExprLocation(methodCall, (yylsp[-2]));
      (yyval.expr) = methodCall;
      free((yyvsp[-2].str));
  }
#line 2581 "src/Parser/parser.tab.cpp"
    break;

  case 54: /* expr: BASE LPAREN argument_list RPAREN  */
#line 558 "src/Parser/parser.y"
                                     {
      std::vector<ExprPtr> args;
      for (auto& arg : *(yyvsp[-1].expr_list)) {
          args.push_back(std::move(arg));
      }
      auto baseCall = new BaseCallExpr(std::move(args));
      setExprLocation(baseCall, (yylsp[-3]));
      (yyval.expr) = baseCall;
      delete (yyvsp[-1].expr_list);
  }
#line 2596 "src/Parser/parser.tab.cpp"
    break;

  case 55: /* expr: BASE LPAREN RPAREN  */
#line 568 "src/Parser/parser.y"
                       {
      auto baseCall = new BaseCallExpr({});
      setExprLocation(baseCall, (yylsp[-2]));
      (yyval.expr) = baseCall;
  }
#line 2606 "src/Parser/parser.tab.cpp"
    break;

  case 56: /* expr: LET binding_list IN expr  */
#line 573 "src/Parser/parser.y"
                             { 
      // Desugar multiple bindings into nested LetExpr
      // Start from the last binding and work backwards
      Expr* result = (yyvsp[0].expr);
      
      // Process bindings in reverse order to maintain correct nesting
      for (int i = (yyvsp[-2].bindings_with_types)->size() - 1; i >= 0; --i) {
          const auto& binding = (*(yyvsp[-2].bindings_with_types))[i];
          std::string name = binding.first;
          Expr* init = binding.second.first;
          std::shared_ptr<TypeInfo> type = binding.second.second;
          
          if (i == (yyvsp[-2].bindings_with_types)->size() - 1) {
              // Last binding: body is the original expression
              result = new LetExpr(name, ExprPtr(init), 
                                  std::make_unique<ExprStmt>(ExprPtr(result)), type);
          } else {
              // Other bindings: body is the nested LetExpr
              result = new LetExpr(name, ExprPtr(init), 
                                  std::make_unique<ExprStmt>(ExprPtr(result)), type);
          }
      }
      
      setExprLocation(result, (yylsp[-3]));
      (yyval.expr) = result;
      delete (yyvsp[-2].bindings_with_types); 
  }
#line 2638 "src/Parser/parser.tab.cpp"
    break;

  case 57: /* expr: LET IDENT COLON type ASSIGN expr IN expr  */
#line 600 "src/Parser/parser.y"
                                             {
      auto letExpr = new LetExpr(std::string((yyvsp[-6].str)), ExprPtr((yyvsp[-2].expr)), 
                      std::make_unique<ExprStmt>(ExprPtr((yyvsp[0].expr))), *(yyvsp[-4].type_info));
      setExprLocation(letExpr, (yylsp[-7]));
      (yyval.expr) = letExpr;
      delete (yyvsp[-4].type_info);
      free((yyvsp[-6].str));
  }
#line 2651 "src/Parser/parser.tab.cpp"
    break;

  case 58: /* expr: LET IDENT ASSIGN expr IN expr  */
#line 608 "src/Parser/parser.y"
                                  {
      auto letExpr = new LetExpr(std::string((yyvsp[-4].str)), ExprPtr((yyvsp[-2].expr)), 
                      std::make_unique<ExprStmt>(ExprPtr((yyvsp[0].expr))));
      setExprLocation(letExpr, (yylsp[-5]));
      (yyval.expr) = letExpr;
      free((yyvsp[-4].str));
  }
#line 2663 "src/Parser/parser.tab.cpp"
    break;

  case 60: /* expr: WHILE expr LBRACE stmt_list RBRACE  */
#line 617 "src/Parser/parser.y"
                                       { 
      auto block = ExprPtr(new ExprBlock(std::move(*(yyvsp[-1].stmts))));
      auto whileExpr = new WhileExpr(ExprPtr((yyvsp[-3].expr)), std::move(block));
      setExprLocation(whileExpr, (yylsp[-4]));
      (yyval.expr) = whileExpr; 
      delete (yyvsp[-1].stmts);
  }
#line 2675 "src/Parser/parser.tab.cpp"
    break;

  case 61: /* expr: FOR LPAREN IDENT IN expr RPAREN expr  */
#line 624 "src/Parser/parser.y"
                                         {
      auto forExpr = new ForExpr(std::string((yyvsp[-4].str)), ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
      setExprLocation(forExpr, (yylsp[-6]));
      (yyval.expr) = forExpr;
      free((yyvsp[-4].str));
  }
#line 2686 "src/Parser/parser.tab.cpp"
    break;

  case 62: /* expr: FOR LPAREN IDENT IN expr RPAREN LBRACE stmt_list RBRACE  */
#line 630 "src/Parser/parser.y"
                                                            {
      auto block = ExprPtr(new ExprBlock(std::move(*(yyvsp[-1].stmts))));
      auto forExpr = new ForExpr(std::string((yyvsp[-6].str)), ExprPtr((yyvsp[-4].expr)), std::move(block));
      setExprLocation(forExpr, (yylsp[-8]));
      (yyval.expr) = forExpr;
      free((yyvsp[-6].str));
      delete (yyvsp[-1].stmts);
  }
#line 2699 "src/Parser/parser.tab.cpp"
    break;

  case 63: /* expr: LBRACE stmt_list RBRACE  */
#line 638 "src/Parser/parser.y"
                            { 
      auto block = new ExprBlock(std::move(*(yyvsp[-1].stmts)));
      setExprLocation(block, (yylsp[-2]));
      (yyval.expr) = block; 
      delete (yyvsp[-1].stmts);
  }
#line 2710 "src/Parser/parser.tab.cpp"
    break;

  case 64: /* expr: LPAREN expr RPAREN  */
#line 644 "src/Parser/parser.y"
                       { 
      setExprLocation((yyvsp[-1].expr), (yylsp[-1]));
      (yyval.expr) = (yyvsp[-1].expr); 
  }
#line 2719 "src/Parser/parser.tab.cpp"
    break;

  case 65: /* expr: expr DOT IDENT  */
#line 648 "src/Parser/parser.y"
                     {
        auto getAttr = new GetAttrExpr(ExprPtr((yyvsp[-2].expr)), (yyvsp[0].str));
        setExprLocation(getAttr, (yylsp[-1]));
        (yyval.expr) = getAttr;
        free((yyvsp[0].str));
    }
#line 2730 "src/Parser/parser.tab.cpp"
    break;

  case 66: /* expr: expr DOT IDENT ASSIGN_DESTRUCT expr  */
#line 654 "src/Parser/parser.y"
                                        {
        auto setAttr = new SetAttrExpr(ExprPtr((yyvsp[-4].expr)), (yyvsp[-2].str), ExprPtr((yyvsp[0].expr)));
        setExprLocation(setAttr, (yylsp[-3]));
        (yyval.expr) = setAttr;
        free((yyvsp[-2].str));
    }
#line 2741 "src/Parser/parser.tab.cpp"
    break;

  case 67: /* expr: expr DOT IDENT LPAREN argument_list RPAREN  */
#line 660 "src/Parser/parser.y"
                                               {
        auto methodCall = new MethodCallExpr(ExprPtr((yyvsp[-5].expr)), (yyvsp[-3].str), std::move(*(yyvsp[-1].expr_list)));
        setExprLocation(methodCall, (yylsp[-4]));
        (yyval.expr) = methodCall;
        delete (yyvsp[-1].expr_list);
        free((yyvsp[-3].str));
    }
#line 2753 "src/Parser/parser.tab.cpp"
    break;

  case 68: /* expr: SELF  */
#line 667 "src/Parser/parser.y"
         {
        auto selfExpr = new SelfExpr();
        setExprLocation(selfExpr, (yylsp[0]));
        (yyval.expr) = selfExpr;
    }
#line 2763 "src/Parser/parser.tab.cpp"
    break;

  case 69: /* expr: SELF ASSIGN_DESTRUCT expr  */
#line 672 "src/Parser/parser.y"
                              {
        auto assignExpr = new AssignExpr("self", ExprPtr((yyvsp[0].expr)));
        setExprLocation(assignExpr, (yylsp[-2]));
        (yyval.expr) = assignExpr;
    }
#line 2773 "src/Parser/parser.tab.cpp"
    break;

  case 70: /* expr: BASE LPAREN RPAREN  */
#line 677 "src/Parser/parser.y"
                       {
        auto baseCall = new BaseCallExpr({});
        setExprLocation(baseCall, (yylsp[-2]));
        (yyval.expr) = baseCall;
    }
#line 2783 "src/Parser/parser.tab.cpp"
    break;

  case 71: /* expr: IDENT ASSIGN_DESTRUCT expr  */
#line 682 "src/Parser/parser.y"
                               {
        auto assignExpr = new AssignExpr(std::string((yyvsp[-2].str)), ExprPtr((yyvsp[0].expr)));
        setExprLocation(assignExpr, (yylsp[-2]));
        (yyval.expr) = assignExpr;
        free((yyvsp[-2].str));
    }
#line 2794 "src/Parser/parser.tab.cpp"
    break;

  case 72: /* if_expr: IF LPAREN expr RPAREN expr ELSE expr  */
#line 694 "src/Parser/parser.y"
                                         {
        auto ifExpr = new IfExpr(ExprPtr((yyvsp[-4].expr)), ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
        setExprLocation(ifExpr, (yylsp[-6]));
        (yyval.expr) = ifExpr;
    }
#line 2804 "src/Parser/parser.tab.cpp"
    break;

  case 73: /* if_expr: IF LPAREN expr RPAREN expr ELIF LPAREN expr RPAREN expr ELSE expr  */
#line 699 "src/Parser/parser.y"
                                                                        {
        // Transformar: if (cond1) expr1 elif (cond2) expr2 else expr3
        // A: if (cond1) expr1 else if (cond2) expr2 else expr3
        auto nestedIf = new IfExpr(ExprPtr((yyvsp[-4].expr)), ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)));
        auto ifExpr = new IfExpr(ExprPtr((yyvsp[-9].expr)), ExprPtr((yyvsp[-7].expr)), ExprPtr(nestedIf));
        setExprLocation(ifExpr, (yylsp[-11]));
        (yyval.expr) = ifExpr;
    }
#line 2817 "src/Parser/parser.tab.cpp"
    break;

  case 74: /* if_expr: IF LPAREN expr RPAREN expr ELIF LPAREN expr RPAREN expr  */
#line 707 "src/Parser/parser.y"
                                                              {
        // Transformar: if (cond1) expr1 elif (cond2) expr2
        // A: if (cond1) expr1 else if (cond2) expr2
        auto nestedIf = new IfExpr(ExprPtr((yyvsp[-2].expr)), ExprPtr((yyvsp[0].expr)), nullptr);
        auto ifExpr = new IfExpr(ExprPtr((yyvsp[-7].expr)), ExprPtr((yyvsp[-5].expr)), ExprPtr(nestedIf));
        setExprLocation(ifExpr, (yylsp[-9]));
        (yyval.expr) = ifExpr;
    }
#line 2830 "src/Parser/parser.tab.cpp"
    break;

  case 75: /* if_expr: IF LPAREN expr RPAREN LBRACE stmt_list RBRACE ELSE LBRACE stmt_list RBRACE  */
#line 715 "src/Parser/parser.y"
                                                                                 { 
      auto ifBlock = ExprPtr(new ExprBlock(std::move(*(yyvsp[-5].stmts))));
      auto elseBlock = ExprPtr(new ExprBlock(std::move(*(yyvsp[-1].stmts))));
      auto ifExpr = new IfExpr(ExprPtr((yyvsp[-8].expr)), std::move(ifBlock), std::move(elseBlock));
      setExprLocation(ifExpr, (yylsp[-10]));
      (yyval.expr) = ifExpr; 
      delete (yyvsp[-5].stmts);
      delete (yyvsp[-1].stmts);
  }
#line 2844 "src/Parser/parser.tab.cpp"
    break;

  case 76: /* if_expr: IF LPAREN expr RPAREN LBRACE stmt_list RBRACE  */
#line 724 "src/Parser/parser.y"
                                                  { 
      auto ifBlock = ExprPtr(new ExprBlock(std::move(*(yyvsp[-1].stmts))));
      auto ifExpr = new IfExpr(ExprPtr((yyvsp[-4].expr)), std::move(ifBlock), nullptr);
      setExprLocation(ifExpr, (yylsp[-6]));
      (yyval.expr) = ifExpr; 
      delete (yyvsp[-1].stmts);
  }
#line 2856 "src/Parser/parser.tab.cpp"
    break;

  case 77: /* if_expr: IF LPAREN expr RPAREN LBRACE stmt_list RBRACE ELIF LPAREN expr RPAREN LBRACE stmt_list RBRACE ELSE LBRACE stmt_list RBRACE  */
#line 731 "src/Parser/parser.y"
                                                                                                                               {
      // Transformar: if (cond1) { body1 } elif (cond2) { body2 } else { body3 }
      // A: if (cond1) { body1 } else if (cond2) { body2 } else { body3 }
      auto elifBlock = ExprPtr(new ExprBlock(std::move(*(yyvsp[-5].stmts))));
      auto elseBlock = ExprPtr(new ExprBlock(std::move(*(yyvsp[-1].stmts))));
      auto nestedIf = new IfExpr(ExprPtr((yyvsp[-8].expr)), std::move(elifBlock), std::move(elseBlock));
      
      auto ifBlock = ExprPtr(new ExprBlock(std::move(*(yyvsp[-12].stmts))));
      auto ifExpr = new IfExpr(ExprPtr((yyvsp[-15].expr)), std::move(ifBlock), ExprPtr(nestedIf));
      setExprLocation(ifExpr, (yylsp[-17]));
      (yyval.expr) = ifExpr;
      delete (yyvsp[-12].stmts);
      delete (yyvsp[-5].stmts);
      delete (yyvsp[-1].stmts);
  }
#line 2876 "src/Parser/parser.tab.cpp"
    break;

  case 78: /* if_expr: IF LPAREN expr RPAREN LBRACE stmt_list RBRACE ELIF LPAREN expr RPAREN LBRACE stmt_list RBRACE  */
#line 746 "src/Parser/parser.y"
                                                                                                  {
      // Transformar: if (cond1) { body1 } elif (cond2) { body2 }
      // A: if (cond1) { body1 } else if (cond2) { body2 }
      auto elifBlock = ExprPtr(new ExprBlock(std::move(*(yyvsp[-1].stmts))));
      auto nestedIf = new IfExpr(ExprPtr((yyvsp[-4].expr)), std::move(elifBlock), nullptr);
      
      auto ifBlock = ExprPtr(new ExprBlock(std::move(*(yyvsp[-8].stmts))));
      auto ifExpr = new IfExpr(ExprPtr((yyvsp[-11].expr)), std::move(ifBlock), ExprPtr(nestedIf));
      setExprLocation(ifExpr, (yylsp[-13]));
      (yyval.expr) = ifExpr;
      delete (yyvsp[-8].stmts);
      delete (yyvsp[-1].stmts);
  }
#line 2894 "src/Parser/parser.tab.cpp"
    break;

  case 79: /* argument_list: %empty  */
#line 763 "src/Parser/parser.y"
    { (yyval.expr_list) = new std::vector<ExprPtr>(); }
#line 2900 "src/Parser/parser.tab.cpp"
    break;

  case 80: /* argument_list: expr  */
#line 765 "src/Parser/parser.y"
    {
        (yyval.expr_list) = new std::vector<ExprPtr>();
        (yyval.expr_list)->emplace_back(ExprPtr((yyvsp[(1) - (1)].expr)));
      }
#line 2909 "src/Parser/parser.tab.cpp"
    break;

  case 81: /* argument_list: argument_list COMMA expr  */
#line 770 "src/Parser/parser.y"
    {
        (yyvsp[(1) - (3)].expr_list)->emplace_back(ExprPtr((yyvsp[(3) - (3)].expr)));
        (yyval.expr_list) = (yyvsp[(1) - (3)].expr_list);
      }
#line 2918 "src/Parser/parser.tab.cpp"
    break;

  case 82: /* type_decl: TYPE IDENT LPAREN param_list RPAREN INHERITS IDENT LPAREN argument_list RPAREN LBRACE member_list RBRACE  */
#line 777 "src/Parser/parser.y"
                                                                                                             {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *(yyvsp[-9].param_list)) {
            params.push_back(param.first);
            param_types.push_back(param.second);
        }
        auto typeDecl = static_cast<Stmt*>(new TypeDecl(
            std::string((yyvsp[-11].str)),
            std::move(params),
            std::move(param_types),
            std::move((yyvsp[-1].mems)->first),
            std::move((yyvsp[-1].mems)->second),
            std::string((yyvsp[-6].str)),
            std::move(*(yyvsp[-4].expr_list))
        ));
        setStmtLocation(typeDecl, (yylsp[-12]));
        (yyval.stmt) = typeDecl;
        delete (yyvsp[-9].param_list); delete (yyvsp[-4].expr_list); delete (yyvsp[-1].mems);
        free((yyvsp[-11].str)); free((yyvsp[-6].str));
    }
#line 2944 "src/Parser/parser.tab.cpp"
    break;

  case 83: /* type_decl: TYPE IDENT INHERITS IDENT LPAREN argument_list RPAREN LBRACE member_list RBRACE  */
#line 798 "src/Parser/parser.y"
                                                                                    {
        auto typeDecl = static_cast<Stmt*>(new TypeDecl(
            std::string((yyvsp[-8].str)),
            {},
            {},
            std::move((yyvsp[-1].mems)->first),
            std::move((yyvsp[-1].mems)->second),
            std::string((yyvsp[-6].str)),
            std::move(*(yyvsp[-4].expr_list))
        ));
        setStmtLocation(typeDecl, (yylsp[-9]));
        (yyval.stmt) = typeDecl;
        delete (yyvsp[-4].expr_list); delete (yyvsp[-1].mems);
        free((yyvsp[-8].str)); free((yyvsp[-6].str));
    }
#line 2964 "src/Parser/parser.tab.cpp"
    break;

  case 84: /* type_decl: TYPE IDENT INHERITS IDENT LBRACE member_list RBRACE  */
#line 813 "src/Parser/parser.y"
                                                      {
        auto typeDecl = static_cast<Stmt*>(new TypeDecl(
            std::string((yyvsp[-5].str)),
            {}, // sin parámetros propios
            {},
            std::move((yyvsp[-1].mems)->first),
            std::move((yyvsp[-1].mems)->second),
            std::string((yyvsp[-3].str)), // tipo base
            {} // sin baseArgs explícitos: herencia implícita
        ));
        setStmtLocation(typeDecl, (yylsp[-6]));
        (yyval.stmt) = typeDecl;
        delete (yyvsp[-1].mems);
        free((yyvsp[-5].str)); free((yyvsp[-3].str));
    }
#line 2984 "src/Parser/parser.tab.cpp"
    break;

  case 85: /* type_decl: TYPE IDENT LPAREN param_list RPAREN LBRACE member_list RBRACE  */
#line 828 "src/Parser/parser.y"
                                                                  {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *(yyvsp[-4].param_list)) {
            params.push_back(param.first);
            param_types.push_back(param.second);
        }
        auto typeDecl = static_cast<Stmt*>(new TypeDecl(
            std::string((yyvsp[-6].str)),
            std::move(params),
            std::move(param_types),
            std::move((yyvsp[-1].mems)->first),
            std::move((yyvsp[-1].mems)->second)
        ));
        setStmtLocation(typeDecl, (yylsp[-7]));
        (yyval.stmt) = typeDecl;
        delete (yyvsp[-4].param_list); delete (yyvsp[-1].mems);
        free((yyvsp[-6].str));
    }
#line 3008 "src/Parser/parser.tab.cpp"
    break;

  case 86: /* type_decl: TYPE IDENT LPAREN param_list RPAREN INHERITS IDENT LBRACE member_list RBRACE  */
#line 847 "src/Parser/parser.y"
                                                                                 {
        std::vector<std::string> params;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *(yyvsp[-6].param_list)) {
            params.push_back(param.first);
            param_types.push_back(param.second);
        }
        auto typeDecl = static_cast<Stmt*>(new TypeDecl(
            std::string((yyvsp[-8].str)),
            std::move(params),
            std::move(param_types),
            std::move((yyvsp[-1].mems)->first),
            std::move((yyvsp[-1].mems)->second),
            std::string((yyvsp[-3].str)),
            {} // sin baseArgs
        ));
        setStmtLocation(typeDecl, (yylsp[-9]));
        (yyval.stmt) = typeDecl;
        delete (yyvsp[-6].param_list); delete (yyvsp[-1].mems);
        free((yyvsp[-8].str)); free((yyvsp[-3].str));
    }
#line 3034 "src/Parser/parser.tab.cpp"
    break;

  case 87: /* type_decl: TYPE IDENT LBRACE member_list RBRACE  */
#line 868 "src/Parser/parser.y"
                                         {
        auto typeDecl = static_cast<Stmt*>(new TypeDecl(
            std::string((yyvsp[-3].str)),
            {},
            {},
            std::move((yyvsp[-1].mems)->first),
            std::move((yyvsp[-1].mems)->second)
        ));
        setStmtLocation(typeDecl, (yylsp[-4]));
        (yyval.stmt) = typeDecl;
        delete (yyvsp[-1].mems); free((yyvsp[-3].str));
    }
#line 3051 "src/Parser/parser.tab.cpp"
    break;

  case 88: /* member_list: %empty  */
#line 883 "src/Parser/parser.y"
                { 
        auto* pair = new std::pair<
            std::vector<std::unique_ptr<AttributeDecl>>,
            std::vector<std::unique_ptr<MethodDecl>>
        >();
        (yyval.mems) = pair;
    }
#line 3063 "src/Parser/parser.tab.cpp"
    break;

  case 89: /* member_list: member_list member  */
#line 890 "src/Parser/parser.y"
                         {
        if ((yyvsp[0].member_pair)->first) {
            (yyvsp[-1].mems)->first.push_back(std::unique_ptr<AttributeDecl>((yyvsp[0].member_pair)->first));
        } else {
            (yyvsp[-1].mems)->second.push_back(std::unique_ptr<MethodDecl>((yyvsp[0].member_pair)->second));
        }
        delete (yyvsp[0].member_pair);
        (yyval.mems) = (yyvsp[-1].mems);
    }
#line 3077 "src/Parser/parser.tab.cpp"
    break;

  case 90: /* member: attribute SEMICOLON  */
#line 902 "src/Parser/parser.y"
                        {
        auto* pair = new std::pair<AttributeDecl*, MethodDecl*>();
        pair->first = (yyvsp[-1].attribute_decl);
        (yyval.member_pair) = pair;
    }
#line 3087 "src/Parser/parser.tab.cpp"
    break;

  case 91: /* member: method SEMICOLON  */
#line 907 "src/Parser/parser.y"
                       {
        auto* pair = new std::pair<AttributeDecl*, MethodDecl*>();
        pair->second = (yyvsp[-1].method_decl);
        (yyval.member_pair) = pair;
    }
#line 3097 "src/Parser/parser.tab.cpp"
    break;

  case 92: /* attribute: IDENT ASSIGN expr  */
#line 915 "src/Parser/parser.y"
                      {
        auto attrDecl = static_cast<AttributeDecl*>(new AttributeDecl((yyvsp[-2].str), ExprPtr((yyvsp[0].expr))));
        setStmtLocation(attrDecl, (yylsp[-2]));
        (yyval.attribute_decl) = attrDecl;
        free((yyvsp[-2].str));
    }
#line 3108 "src/Parser/parser.tab.cpp"
    break;

  case 93: /* attribute: IDENT COLON type ASSIGN expr  */
#line 921 "src/Parser/parser.y"
                                   {
        auto attrDecl = static_cast<AttributeDecl*>(new AttributeDecl((yyvsp[-4].str), ExprPtr((yyvsp[0].expr)), *(yyvsp[-2].type_info)));
        setStmtLocation(attrDecl, (yylsp[-4]));
        (yyval.attribute_decl) = attrDecl;
        free((yyvsp[-4].str));
        delete (yyvsp[-2].type_info);
    }
#line 3120 "src/Parser/parser.tab.cpp"
    break;

  case 94: /* method: IDENT LPAREN ident_list RPAREN ARROW expr  */
#line 931 "src/Parser/parser.y"
                                              {
        std::vector<std::string> args = (yyvsp[-3].str_list) ? std::move(*(yyvsp[-3].str_list)) : std::vector<std::string>();
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        // Create paramTypes with Unknown types for each parameter
        for (const auto& param : args) {
            param_types.push_back(std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown));
        }
        delete (yyvsp[-3].str_list);
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl((yyvsp[-5].str), std::move(args), StmtPtr(new ExprStmt(ExprPtr((yyvsp[0].expr)))), std::move(param_types)));
        setStmtLocation(methodDecl, (yylsp[-5]));
        (yyval.method_decl) = methodDecl;
        free((yyvsp[-5].str));
    }
#line 3138 "src/Parser/parser.tab.cpp"
    break;

  case 95: /* method: IDENT LPAREN param_list RPAREN ARROW expr  */
#line 944 "src/Parser/parser.y"
                                                {
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *(yyvsp[-3].param_list)) {
            args.push_back(param.first);
            param_types.push_back(param.second);
        }
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl((yyvsp[-5].str), std::move(args), StmtPtr(new ExprStmt(ExprPtr((yyvsp[0].expr)))), std::move(param_types)));
        setStmtLocation(methodDecl, (yylsp[-5]));
        (yyval.method_decl) = methodDecl;
        delete (yyvsp[-3].param_list);
        free((yyvsp[-5].str));
    }
#line 3156 "src/Parser/parser.tab.cpp"
    break;

  case 96: /* method: IDENT LPAREN param_list RPAREN COLON type ARROW expr  */
#line 957 "src/Parser/parser.y"
                                                           {
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *(yyvsp[-5].param_list)) {
            args.push_back(param.first);
            param_types.push_back(param.second);
        }
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl((yyvsp[-7].str), std::move(args), StmtPtr(new ExprStmt(ExprPtr((yyvsp[0].expr)))), std::move(param_types), *(yyvsp[-2].type_info)));
        setStmtLocation(methodDecl, (yylsp[-7]));
        (yyval.method_decl) = methodDecl;
        delete (yyvsp[-5].param_list);
        delete (yyvsp[-2].type_info);
        free((yyvsp[-7].str));
    }
#line 3175 "src/Parser/parser.tab.cpp"
    break;

  case 97: /* method: IDENT LPAREN ident_list RPAREN LBRACE stmt_list RBRACE  */
#line 971 "src/Parser/parser.y"
                                                             {
        std::vector<std::string> args = (yyvsp[-4].str_list) ? std::move(*(yyvsp[-4].str_list)) : std::vector<std::string>();
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        // Create paramTypes with Unknown types for each parameter
        for (const auto& param : args) {
            param_types.push_back(std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown));
        }
        delete (yyvsp[-4].str_list);
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*(yyvsp[-1].stmts));
        delete (yyvsp[-1].stmts);
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl((yyvsp[-6].str), std::move(args), StmtPtr(std::move(block)), std::move(param_types)));
        setStmtLocation(methodDecl, (yylsp[-6]));
        (yyval.method_decl) = methodDecl;
        free((yyvsp[-6].str));
    }
#line 3196 "src/Parser/parser.tab.cpp"
    break;

  case 98: /* method: IDENT LPAREN param_list RPAREN LBRACE stmt_list RBRACE  */
#line 987 "src/Parser/parser.y"
                                                             {
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *(yyvsp[-4].param_list)) {
            args.push_back(param.first);
            param_types.push_back(param.second);
        }
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*(yyvsp[-1].stmts));
        delete (yyvsp[-1].stmts);
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl((yyvsp[-6].str), std::move(args), StmtPtr(std::move(block)), std::move(param_types)));
        setStmtLocation(methodDecl, (yylsp[-6]));
        (yyval.method_decl) = methodDecl;
        delete (yyvsp[-4].param_list);
        free((yyvsp[-6].str));
    }
#line 3217 "src/Parser/parser.tab.cpp"
    break;

  case 99: /* method: IDENT LPAREN param_list RPAREN COLON type LBRACE stmt_list RBRACE  */
#line 1003 "src/Parser/parser.y"
                                                                        {
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        for (const auto& param : *(yyvsp[-6].param_list)) {
            args.push_back(param.first);
            param_types.push_back(param.second);
        }
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*(yyvsp[-1].stmts));
        delete (yyvsp[-1].stmts);
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl((yyvsp[-8].str), std::move(args), StmtPtr(std::move(block)), std::move(param_types), *(yyvsp[-3].type_info)));
        setStmtLocation(methodDecl, (yylsp[-8]));
        (yyval.method_decl) = methodDecl;
        delete (yyvsp[-6].param_list);
        delete (yyvsp[-3].type_info);
        free((yyvsp[-8].str));
    }
#line 3239 "src/Parser/parser.tab.cpp"
    break;

  case 100: /* method: IDENT LPAREN RPAREN ARROW expr  */
#line 1020 "src/Parser/parser.y"
                                     {
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl((yyvsp[-4].str), std::move(args), StmtPtr(new ExprStmt(ExprPtr((yyvsp[0].expr)))), std::move(param_types)));
        setStmtLocation(methodDecl, (yylsp[-4]));
        (yyval.method_decl) = methodDecl;
        free((yyvsp[-4].str));
    }
#line 3252 "src/Parser/parser.tab.cpp"
    break;

  case 101: /* method: IDENT LPAREN RPAREN COLON type ARROW expr  */
#line 1028 "src/Parser/parser.y"
                                                {
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl((yyvsp[-6].str), std::move(args), StmtPtr(new ExprStmt(ExprPtr((yyvsp[0].expr)))), std::move(param_types), *(yyvsp[-2].type_info)));
        setStmtLocation(methodDecl, (yylsp[-6]));
        (yyval.method_decl) = methodDecl;
        delete (yyvsp[-2].type_info);
        free((yyvsp[-6].str));
    }
#line 3266 "src/Parser/parser.tab.cpp"
    break;

  case 102: /* method: IDENT LPAREN RPAREN LBRACE stmt_list RBRACE  */
#line 1037 "src/Parser/parser.y"
                                                  {
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*(yyvsp[-1].stmts));
        delete (yyvsp[-1].stmts);
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl((yyvsp[-5].str), std::move(args), StmtPtr(std::move(block)), std::move(param_types)));
        setStmtLocation(methodDecl, (yylsp[-5]));
        (yyval.method_decl) = methodDecl;
        free((yyvsp[-5].str));
    }
#line 3282 "src/Parser/parser.tab.cpp"
    break;

  case 103: /* method: IDENT LPAREN RPAREN COLON type LBRACE stmt_list RBRACE  */
#line 1048 "src/Parser/parser.y"
                                                             {
        std::vector<std::string> args;
        std::vector<std::shared_ptr<TypeInfo>> param_types;
        auto block = std::make_unique<Program>();
        block->stmts = std::move(*(yyvsp[-1].stmts));
        delete (yyvsp[-1].stmts);
        auto methodDecl = static_cast<MethodDecl*>(new MethodDecl((yyvsp[-7].str), std::move(args), StmtPtr(std::move(block)), std::move(param_types), *(yyvsp[-3].type_info)));
        setStmtLocation(methodDecl, (yylsp[-7]));
        (yyval.method_decl) = methodDecl;
        delete (yyvsp[-3].type_info);
        free((yyvsp[-7].str));
    }
#line 3299 "src/Parser/parser.tab.cpp"
    break;

  case 104: /* type: IDENT  */
#line 1063 "src/Parser/parser.y"
          { 
        // Si es un tipo conocido, usarlo, sino usar Object
        std::string type_name((yyvsp[0].str));
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
        free((yyvsp[0].str));
    }
#line 3319 "src/Parser/parser.tab.cpp"
    break;

  case 105: /* param_list: %empty  */
#line 1081 "src/Parser/parser.y"
                { (yyval.param_list) = new std::vector<std::pair<std::string, std::shared_ptr<TypeInfo>>>(); }
#line 3325 "src/Parser/parser.tab.cpp"
    break;

  case 106: /* param_list: param  */
#line 1082 "src/Parser/parser.y"
            { 
        (yyval.param_list) = new std::vector<std::pair<std::string, std::shared_ptr<TypeInfo>>>();
        (yyval.param_list)->push_back(*(yyvsp[0].param_with_type));
        delete (yyvsp[0].param_with_type);
    }
#line 3335 "src/Parser/parser.tab.cpp"
    break;

  case 107: /* param_list: param_list COMMA param  */
#line 1087 "src/Parser/parser.y"
                             {
        (yyvsp[-2].param_list)->push_back(*(yyvsp[0].param_with_type));
        delete (yyvsp[0].param_with_type);
        (yyval.param_list) = (yyvsp[-2].param_list);
    }
#line 3345 "src/Parser/parser.tab.cpp"
    break;

  case 108: /* param: IDENT  */
#line 1095 "src/Parser/parser.y"
          { 
        (yyval.param_with_type) = new std::pair<std::string, std::shared_ptr<TypeInfo>>(
            std::string((yyvsp[0].str)), 
            std::make_shared<TypeInfo>(TypeInfo::Kind::Unknown)
        );
        free((yyvsp[0].str));
    }
#line 3357 "src/Parser/parser.tab.cpp"
    break;

  case 109: /* param: IDENT COLON type  */
#line 1102 "src/Parser/parser.y"
                       {
        (yyval.param_with_type) = new std::pair<std::string, std::shared_ptr<TypeInfo>>(
            std::string((yyvsp[-2].str)), 
            *(yyvsp[0].type_info)
        );
        free((yyvsp[-2].str));
        delete (yyvsp[0].type_info);
    }
#line 3370 "src/Parser/parser.tab.cpp"
    break;


#line 3374 "src/Parser/parser.tab.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken, &yylloc};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
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

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 1112 "src/Parser/parser.y"
