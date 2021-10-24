/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
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
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Substitute the type names.  */
#define YYSTYPE         ZSQL_STYPE
#define YYLTYPE         ZSQL_LTYPE
/* Substitute the variable and function names.  */
#define yyparse         zsql_parse
#define yylex           zsql_lex
#define yyerror         zsql_error
#define yydebug         zsql_debug
#define yynerrs         zsql_nerrs

/* First part of user prologue.  */
#line 1 "bison_parser.y"

/**
 * bison_parser.y
 * defines bison_parser.hpp
 * outputs bison_parser.cpp
 *
 * Grammar File Spec: http://dinosaur.compilertools.net/bison/bison_6.html
 *
 */
/*********************************
 ** Section 1: C Declarations
 *********************************/

#include "bison_parser.hpp"
#include "flex_lexer.hpp"

#include <stdio.h>
#include <string.h>

using namespace zsql;

int yyerror(YYLTYPE* llocp, SQLParserResult* result, yyscan_t scanner, const char *msg) {
	result->setIsValid(false);
	result->setErrorDetails(strdup(msg), llocp->first_line, llocp->first_column);
	return 0;
}


#line 106 "bison_parser.cpp"

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

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 1
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_ZSQL_BISON_PARSER_HPP_INCLUDED
# define YY_ZSQL_BISON_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef ZSQL_DEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define ZSQL_DEBUG 1
#  else
#   define ZSQL_DEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define ZSQL_DEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined ZSQL_DEBUG */
#if ZSQL_DEBUG
extern int zsql_debug;
#endif
/* "%code requires" blocks.  */
#line 35 "bison_parser.y"

// %code requires block

#include "statements.hpp"
#include "SQLParserResult.hpp"
#include "parser_typedef.hpp"

// Auto update column and line number
#define YY_USER_ACTION \
		yylloc->first_line = yylloc->last_line; \
		yylloc->first_column = yylloc->last_column; \
		for(int i = 0; yytext[i] != '\0'; i++) { \
			yylloc->total_column++; \
			yylloc->string_length++; \
				if(yytext[i] == '\n') { \
						yylloc->last_line++; \
						yylloc->last_column = 0; \
				} \
				else { \
						yylloc->last_column++; \
				} \
		}

#line 181 "bison_parser.cpp"

/* Token type.  */
#ifndef ZSQL_TOKENTYPE
# define ZSQL_TOKENTYPE
  enum zsql_tokentype
  {
    SQL_IDENTIFIER = 258,
    SQL_STRING = 259,
    SQL_FLOATVAL = 260,
    SQL_INTVAL = 261,
    SQL_DEALLOCATE = 262,
    SQL_PARAMETERS = 263,
    SQL_INTERSECT = 264,
    SQL_TEMPORARY = 265,
    SQL_TIMESTAMP = 266,
    SQL_DISTINCT = 267,
    SQL_NVARCHAR = 268,
    SQL_RESTRICT = 269,
    SQL_TRUNCATE = 270,
    SQL_ANALYZE = 271,
    SQL_BETWEEN = 272,
    SQL_CASCADE = 273,
    SQL_COLUMNS = 274,
    SQL_CONTROL = 275,
    SQL_DEFAULT = 276,
    SQL_EXECUTE = 277,
    SQL_EXPLAIN = 278,
    SQL_INTEGER = 279,
    SQL_NATURAL = 280,
    SQL_PREPARE = 281,
    SQL_PRIMARY = 282,
    SQL_SCHEMAS = 283,
    SQL_SPATIAL = 284,
    SQL_VARCHAR = 285,
    SQL_VIRTUAL = 286,
    SQL_DESCRIBE = 287,
    SQL_BEFORE = 288,
    SQL_COLUMN = 289,
    SQL_CREATE = 290,
    SQL_DELETE = 291,
    SQL_DIRECT = 292,
    SQL_DOUBLE = 293,
    SQL_ESCAPE = 294,
    SQL_EXCEPT = 295,
    SQL_EXISTS = 296,
    SQL_EXTRACT = 297,
    SQL_CAST = 298,
    SQL_FORMAT = 299,
    SQL_GLOBAL = 300,
    SQL_HAVING = 301,
    SQL_IMPORT = 302,
    SQL_INSERT = 303,
    SQL_ISNULL = 304,
    SQL_OFFSET = 305,
    SQL_RENAME = 306,
    SQL_SCHEMA = 307,
    SQL_SELECT = 308,
    SQL_SORTED = 309,
    SQL_TABLES = 310,
    SQL_UNIQUE = 311,
    SQL_UNLOAD = 312,
    SQL_UPDATE = 313,
    SQL_VALUES = 314,
    SQL_AFTER = 315,
    SQL_ALTER = 316,
    SQL_CROSS = 317,
    SQL_DELTA = 318,
    SQL_FLOAT = 319,
    SQL_GROUP = 320,
    SQL_INDEX = 321,
    SQL_INNER = 322,
    SQL_LIMIT = 323,
    SQL_LOCAL = 324,
    SQL_MERGE = 325,
    SQL_MINUS = 326,
    SQL_ORDER = 327,
    SQL_OUTER = 328,
    SQL_RIGHT = 329,
    SQL_TABLE = 330,
    SQL_UNION = 331,
    SQL_USING = 332,
    SQL_WHERE = 333,
    SQL_CALL = 334,
    SQL_CASE = 335,
    SQL_CHAR = 336,
    SQL_COPY = 337,
    SQL_DATE = 338,
    SQL_DATETIME = 339,
    SQL_DESC = 340,
    SQL_DROP = 341,
    SQL_ELSE = 342,
    SQL_FILE = 343,
    SQL_FROM = 344,
    SQL_FULL = 345,
    SQL_HASH = 346,
    SQL_HINT = 347,
    SQL_INTO = 348,
    SQL_JOIN = 349,
    SQL_LEFT = 350,
    SQL_LIKE = 351,
    SQL_LOAD = 352,
    SQL_LONG = 353,
    SQL_NULL = 354,
    SQL_PLAN = 355,
    SQL_SHOW = 356,
    SQL_TEXT = 357,
    SQL_THEN = 358,
    SQL_TIME = 359,
    SQL_VIEW = 360,
    SQL_WHEN = 361,
    SQL_WITH = 362,
    SQL_ADD = 363,
    SQL_ALL = 364,
    SQL_AND = 365,
    SQL_ASC = 366,
    SQL_END = 367,
    SQL_FOR = 368,
    SQL_INT = 369,
    SQL_KEY = 370,
    SQL_NOT = 371,
    SQL_OFF = 372,
    SQL_SET = 373,
    SQL_TOP = 374,
    SQL_AS = 375,
    SQL_BY = 376,
    SQL_IF = 377,
    SQL_IN = 378,
    SQL_IS = 379,
    SQL_OF = 380,
    SQL_ON = 381,
    SQL_OR = 382,
    SQL_XOR = 383,
    SQL_TO = 384,
    SQL_ARRAY = 385,
    SQL_CONCAT = 386,
    SQL_ILIKE = 387,
    SQL_SECOND = 388,
    SQL_MINUTE = 389,
    SQL_HOUR = 390,
    SQL_DAY = 391,
    SQL_MONTH = 392,
    SQL_YEAR = 393,
    SQL_TRUE = 394,
    SQL_FALSE = 395,
    SQL_TRANSACTION = 396,
    SQL_BEGIN = 397,
    SQL_COMMIT = 398,
    SQL_ROLLBACK = 399,
    SQL_MULTILINESTRING_SYM = 400,
    SQL_MULTIPOINT_SYM = 401,
    SQL_MULTIPOLYGON_SYM = 402,
    SQL_POINT_SYM = 403,
    SQL_POLYGON_SYM = 404,
    SQL_POINT = 405,
    SQL_ST_SRID = 406,
    SQL_ST_DISTANCE_SPHERE = 407,
    SQL_COUNT = 408,
    SQL_MIN = 409,
    SQL_MAX = 410,
    SQL_SUM = 411,
    SQL_AVG = 412,
    SQL_EQUALS = 413,
    SQL_NOTEQUALS = 414,
    SQL_LESS = 415,
    SQL_GREATER = 416,
    SQL_LESSEQ = 417,
    SQL_GREATEREQ = 418,
    SQL_NOTNULL = 419,
    SQL_UMINUS = 420
  };
#endif

/* Value type.  */
#if ! defined ZSQL_STYPE && ! defined ZSQL_STYPE_IS_DECLARED
union ZSQL_STYPE
{
#line 95 "bison_parser.y"

	double fval;
	__int128_t ival;
	char* sval;
	uintmax_t uval;
	bool bval;

	zsql::SQLStatement* 	  statement;
	zsql::SelectStatement*  select_stmt;
	zsql::ImportStatement*  import_stmt;
	zsql::ExportStatement*  export_stmt;
	zsql::CreateStatement*  create_stmt;
	zsql::InsertStatement*  insert_stmt;
	zsql::DeleteStatement*  delete_stmt;
	zsql::UpdateStatement*  update_stmt;
	zsql::DropStatement*    drop_stmt;
	zsql::PrepareStatement* prep_stmt;
	zsql::ExecuteStatement* exec_stmt;
	zsql::ShowStatement*    show_stmt;
	zsql::TransactionStatement* transaction_stmt;

	zsql::TableName table_name;
	zsql::TableRef* table;
	zsql::Expr* expr;
	zsql::OrderDescription* order;
	zsql::OrderType order_type;
	zsql::WithDescription* with_description_t;
	zsql::DatetimeField datetime_field;
	zsql::LimitDescription* limit;
	zsql::ColumnDefinition* column_t;
	zsql::ColumnType column_type_t;
	zsql::ImportType import_type_t;
	zsql::GroupByDescription* group_t;
	zsql::UpdateClause* update_t;
	zsql::Alias* alias_t;
	zsql::SetOperation* set_operator_t;

	std::vector<zsql::SQLStatement*>* stmt_vec;

	std::vector<char*>* str_vec;
	std::vector<zsql::TableRef*>* table_vec;
	std::vector<zsql::ColumnDefinition*>* column_vec;
	std::vector<zsql::UpdateClause*>* update_vec;
	std::vector<zsql::Expr*>* expr_vec;
	std::vector<zsql::OrderDescription*>* order_vec;
	std::vector<zsql::WithDescription*>* with_description_vec;

#line 406 "bison_parser.cpp"

};
typedef union ZSQL_STYPE ZSQL_STYPE;
# define ZSQL_STYPE_IS_TRIVIAL 1
# define ZSQL_STYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined ZSQL_LTYPE && ! defined ZSQL_LTYPE_IS_DECLARED
typedef struct ZSQL_LTYPE ZSQL_LTYPE;
struct ZSQL_LTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define ZSQL_LTYPE_IS_DECLARED 1
# define ZSQL_LTYPE_IS_TRIVIAL 1
#endif



int zsql_parse (zsql::SQLParserResult* result, yyscan_t scanner);

#endif /* !YY_ZSQL_BISON_PARSER_HPP_INCLUDED  */



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
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
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
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined ZSQL_LTYPE_IS_TRIVIAL && ZSQL_LTYPE_IS_TRIVIAL \
             && defined ZSQL_STYPE_IS_TRIVIAL && ZSQL_STYPE_IS_TRIVIAL)))

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
#define YYFINAL  62
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   952

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  183
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  114
/* YYNRULES -- Number of rules.  */
#define YYNRULES  279
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  523

#define YYUNDEFTOK  2
#define YYMAXUTOK   420


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,   172,     2,     2,
     177,   178,   170,   168,   181,   169,   179,   171,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,   180,
     161,   158,   162,   182,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   175,     2,   176,   173,     2,     2,     2,     2,     2,
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
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
     155,   156,   157,   159,   160,   163,   164,   165,   166,   167,
     174
};

#if ZSQL_DEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   273,   273,   294,   300,   309,   313,   317,   320,   323,
     330,   331,   332,   333,   334,   335,   336,   337,   338,   347,
     348,   353,   354,   358,   362,   374,   377,   380,   386,   387,
     394,   401,   404,   408,   422,   428,   437,   452,   456,   459,
     468,   482,   485,   490,   504,   517,   524,   531,   542,   543,
     547,   548,   552,   558,   559,   560,   561,   562,   563,   564,
     565,   566,   567,   571,   572,   573,   583,   589,   595,   603,
     604,   613,   622,   635,   642,   653,   654,   664,   673,   674,
     678,   690,   694,   698,   712,   713,   716,   717,   728,   729,
     733,   743,   756,   763,   767,   771,   778,   781,   787,   799,
     800,   804,   808,   809,   813,   818,   819,   823,   828,   832,
     833,   837,   838,   842,   843,   847,   851,   852,   853,   859,
     860,   864,   865,   866,   867,   868,   869,   876,   877,   881,
     882,   886,   887,   891,   901,   902,   903,   904,   905,   909,
     910,   911,   912,   913,   914,   915,   916,   917,   918,   919,
     920,   921,   922,   923,   924,   925,   926,   930,   931,   935,
     936,   937,   938,   939,   943,   944,   945,   946,   947,   948,
     949,   950,   951,   952,   953,   957,   958,   959,   963,   964,
     965,   966,   972,   973,   974,   975,   979,   980,   984,   985,
     989,   990,   991,   992,   993,   994,   995,   999,  1000,  1004,
    1008,  1012,  1013,  1014,  1015,  1016,  1017,  1021,  1025,  1029,
    1033,  1037,  1041,  1045,  1049,  1053,  1057,  1061,  1062,  1066,
    1067,  1068,  1069,  1073,  1074,  1075,  1076,  1077,  1081,  1085,
    1086,  1090,  1091,  1095,  1099,  1103,  1115,  1116,  1126,  1127,
    1131,  1132,  1141,  1142,  1147,  1158,  1167,  1168,  1173,  1174,
    1179,  1180,  1185,  1186,  1191,  1192,  1201,  1202,  1206,  1210,
    1214,  1221,  1234,  1242,  1252,  1271,  1272,  1273,  1274,  1275,
    1276,  1277,  1278,  1279,  1280,  1285,  1294,  1295,  1300,  1301
};
#endif

#if ZSQL_DEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "IDENTIFIER", "STRING", "FLOATVAL",
  "INTVAL", "DEALLOCATE", "PARAMETERS", "INTERSECT", "TEMPORARY",
  "TIMESTAMP", "DISTINCT", "NVARCHAR", "RESTRICT", "TRUNCATE", "ANALYZE",
  "BETWEEN", "CASCADE", "COLUMNS", "CONTROL", "DEFAULT", "EXECUTE",
  "EXPLAIN", "INTEGER", "NATURAL", "PREPARE", "PRIMARY", "SCHEMAS",
  "SPATIAL", "VARCHAR", "VIRTUAL", "DESCRIBE", "BEFORE", "COLUMN",
  "CREATE", "DELETE", "DIRECT", "DOUBLE", "ESCAPE", "EXCEPT", "EXISTS",
  "EXTRACT", "CAST", "FORMAT", "GLOBAL", "HAVING", "IMPORT", "INSERT",
  "ISNULL", "OFFSET", "RENAME", "SCHEMA", "SELECT", "SORTED", "TABLES",
  "UNIQUE", "UNLOAD", "UPDATE", "VALUES", "AFTER", "ALTER", "CROSS",
  "DELTA", "FLOAT", "GROUP", "INDEX", "INNER", "LIMIT", "LOCAL", "MERGE",
  "MINUS", "ORDER", "OUTER", "RIGHT", "TABLE", "UNION", "USING", "WHERE",
  "CALL", "CASE", "CHAR", "COPY", "DATE", "DATETIME", "DESC", "DROP",
  "ELSE", "FILE", "FROM", "FULL", "HASH", "HINT", "INTO", "JOIN", "LEFT",
  "LIKE", "LOAD", "LONG", "NULL", "PLAN", "SHOW", "TEXT", "THEN", "TIME",
  "VIEW", "WHEN", "WITH", "ADD", "ALL", "AND", "ASC", "END", "FOR", "INT",
  "KEY", "NOT", "OFF", "SET", "TOP", "AS", "BY", "IF", "IN", "IS", "OF",
  "ON", "OR", "XOR", "TO", "ARRAY", "CONCAT", "ILIKE", "SECOND", "MINUTE",
  "HOUR", "DAY", "MONTH", "YEAR", "TRUE", "FALSE", "TRANSACTION", "BEGIN",
  "COMMIT", "ROLLBACK", "MULTILINESTRING_SYM", "MULTIPOINT_SYM",
  "MULTIPOLYGON_SYM", "POINT_SYM", "POLYGON_SYM", "POINT", "ST_SRID",
  "ST_DISTANCE_SPHERE", "COUNT", "MIN", "MAX", "SUM", "AVG", "'='",
  "EQUALS", "NOTEQUALS", "'<'", "'>'", "LESS", "GREATER", "LESSEQ",
  "GREATEREQ", "NOTNULL", "'+'", "'-'", "'*'", "'/'", "'%'", "'^'",
  "UMINUS", "'['", "']'", "'('", "')'", "'.'", "';'", "','", "'?'",
  "$accept", "input", "statement_list", "statement",
  "preparable_statement", "opt_hints", "hint_list", "hint",
  "transaction_statement", "opt_transaction_keyword", "prepare_statement",
  "prepare_target_query", "execute_statement", "import_statement",
  "file_type", "file_path", "opt_file_type", "export_statement",
  "show_statement", "create_statement", "opt_not_exists",
  "column_def_commalist", "column_def", "column_type",
  "opt_column_nullable", "drop_statement", "opt_exists",
  "delete_statement", "truncate_statement", "insert_statement",
  "opt_column_list", "update_statement", "update_clause_commalist",
  "update_clause", "select_statement", "select_within_set_operation",
  "select_within_set_operation_no_parentheses", "select_with_paren",
  "select_no_paren", "set_operator", "set_type", "opt_all",
  "select_clause", "opt_distinct", "select_list", "opt_from_clause",
  "from_clause", "opt_where", "opt_group", "opt_having", "opt_order",
  "order_list", "order_desc", "opt_order_type", "opt_top", "opt_limit",
  "expr_list", "opt_literal_list", "literal_list", "expr_alias", "expr",
  "operand", "scalar_expr", "unary_expr", "binary_expr", "logic_expr",
  "in_expr", "case_expr", "case_list", "exists_expr", "comp_expr",
  "function_expr", "extract_expr", "cast_expr", "datetime_field",
  "array_expr", "point_expr", "srid_expr", "count_expr", "min_expr",
  "max_expr", "sum_expr", "avg_expr", "distance_expr", "array_index",
  "between_expr", "column_name", "literal", "string_literal",
  "bool_literal", "num_literal", "int_literal", "null_literal",
  "param_expr", "table_ref", "table_ref_atomic",
  "nonjoin_table_ref_atomic", "table_ref_commalist", "table_ref_name",
  "table_ref_name_no_alias", "table_name", "table_alias",
  "opt_table_alias", "alias", "opt_alias", "opt_with_clause",
  "with_clause", "with_description_list", "with_description",
  "join_clause", "opt_join_type", "join_condition", "opt_semicolon",
  "ident_commalist", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,   407,   408,   409,   410,   411,   412,    61,   413,
     414,    60,    62,   415,   416,   417,   418,   419,    43,    45,
      42,    47,    37,    94,   420,    91,    93,    40,    41,    46,
      59,    44,    63
};
# endif

#define YYPACT_NINF (-417)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-277)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     703,   178,    76,   192,   194,    76,   -22,   -39,   123,   121,
      76,    76,    86,    47,   213,    87,    87,    87,   250,    78,
    -417,   153,  -417,   153,  -417,  -417,  -417,  -417,  -417,  -417,
    -417,  -417,  -417,  -417,  -417,   -28,  -417,   265,   100,  -417,
     104,   197,  -417,   186,   186,    76,   289,    76,   191,  -417,
     -51,   214,   214,    76,  -417,   215,   166,  -417,  -417,  -417,
    -417,  -417,  -417,   698,  -417,   246,  -417,  -417,   222,   -28,
     137,  -417,    32,  -417,   345,    53,   351,   241,    76,    76,
     282,  -417,   273,   187,   359,   361,   361,   322,    76,    76,
    -417,   190,   213,  -417,   195,   362,   363,   199,   200,  -417,
    -417,  -417,   -28,   264,   258,   -28,    -4,  -417,  -417,  -417,
    -417,  -417,  -417,  -417,  -417,   202,   201,  -417,  -417,  -417,
    -417,  -417,  -417,  -417,  -417,  -417,   346,   -21,   187,   390,
    -417,   361,   383,    94,   230,   -44,  -417,   284,  -417,   284,
    -417,  -417,  -417,  -417,  -417,   386,  -417,  -417,   390,  -417,
    -417,   325,  -417,  -417,   137,  -417,  -417,   390,   325,   390,
     160,  -417,  -417,    53,  -417,   389,   291,   399,   285,    -7,
     229,   232,   233,   260,   435,   244,   243,   247,   248,   249,
     251,   259,   266,   469,  -417,   328,   128,   531,  -417,  -417,
    -417,  -417,  -417,  -417,  -417,  -417,  -417,  -417,  -417,  -417,
    -417,  -417,  -417,  -417,  -417,  -417,  -417,  -417,  -417,  -417,
    -417,   314,  -417,    96,   268,  -417,   390,   359,  -417,   377,
    -417,  -417,   269,    97,  -417,   334,   254,  -417,    95,    -4,
     -28,   267,  -417,   115,    -4,   128,   397,   -14,  -417,   364,
    -417,   320,   110,  -417,   291,     9,     8,   396,   185,   390,
     390,   -43,   145,   274,   469,   759,   390,   300,    24,    24,
      24,    24,    24,    24,    36,   275,   -67,   390,   390,   390,
     469,  -417,   469,    45,   277,   -45,   469,   469,   469,   469,
     469,   469,   469,   469,   469,   469,   469,   469,   469,   469,
     469,   362,    76,  -417,   452,    53,   128,  -417,   289,    53,
    -417,   386,     7,   282,  -417,   390,  -417,   453,  -417,  -417,
    -417,  -417,   390,  -417,  -417,  -417,  -417,   390,   390,   361,
    -417,   280,  -417,  -417,   283,  -417,  -417,  -417,  -417,  -417,
     -24,  -417,   399,  -417,  -417,   390,  -417,  -417,   281,  -417,
    -417,  -417,  -417,  -417,  -417,   372,   117,   -58,   161,   390,
     390,  -417,   396,   366,   -69,   286,   288,   287,   290,   308,
     309,   310,   312,   313,  -417,  -417,  -417,    52,   354,   659,
     777,   469,   469,   315,   328,  -417,   394,   319,   777,   777,
     777,   777,   504,   504,   504,   504,    36,    36,   -16,   -16,
     -16,   -30,   323,  -417,  -417,   129,  -417,   146,  -417,   291,
    -417,    15,  -417,   321,  -417,    30,  -417,   400,  -417,  -417,
    -417,   128,   128,  -417,   489,   490,  -417,   401,  -417,  -417,
     147,  -417,   390,   320,   390,   390,  -417,   170,   143,   326,
    -417,   390,   362,   350,  -417,  -417,  -417,  -417,  -417,   469,
     691,   777,   328,   329,   165,  -417,  -417,  -417,  -417,   330,
     409,  -417,  -417,  -417,   436,   440,   441,   422,     7,   514,
    -417,  -417,  -417,   398,  -417,   340,   343,  -417,  -417,   -34,
     344,   128,   177,  -417,   390,  -417,   171,   347,   348,   759,
     469,   349,   173,  -417,  -417,    30,     7,  -417,  -417,  -417,
       7,    81,   355,   390,  -417,  -417,  -417,  -417,  -417,   128,
    -417,  -417,  -417,   759,  -417,  -417,  -417,  -417,   157,   383,
     -23,   356,   390,   175,   390,  -417,    24,   128,  -417,  -417,
     128,   353,  -417
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int16 yydefact[] =
{
     257,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    29,    29,    29,     0,   277,
       3,    20,    18,    20,    17,     8,     9,     7,    11,    16,
      13,    14,    12,    15,    10,     0,   256,     0,   246,    72,
      32,     0,    43,    49,    49,     0,     0,     0,     0,   245,
       0,    70,    70,     0,    41,     0,   258,   259,    28,    25,
      27,    26,     1,   257,     2,     0,     6,     5,   120,     0,
      81,    82,   112,    68,     0,   130,     0,     0,     0,     0,
     106,    36,     0,    76,     0,     0,     0,     0,     0,     0,
      42,     0,     0,     4,     0,     0,   100,     0,     0,    94,
      95,    93,     0,    97,     0,     0,   126,   247,   228,   232,
     233,   234,   229,   230,   235,     0,   129,   131,   223,   224,
     225,   231,   226,   227,    31,    30,     0,     0,    76,     0,
      71,     0,     0,     0,     0,   106,    78,    39,    37,    39,
      69,    66,    67,   261,   260,     0,   119,    99,     0,    89,
      88,   112,    85,    84,    86,    96,    92,     0,   112,     0,
       0,    90,    33,     0,    48,     0,   257,     0,     0,   219,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   221,     0,   105,   134,   141,   143,
     142,   136,   138,   144,   137,   164,   145,   146,   147,   148,
     150,   152,   153,   154,   155,   156,   151,   140,   135,   157,
     158,     0,   278,     0,     0,    74,     0,     0,    77,     0,
      35,    40,    23,     0,    21,   103,   101,   127,   255,   126,
       0,   111,   113,   118,   126,   122,   124,   121,   132,     0,
      46,     0,     0,    50,   257,   100,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   160,     0,     0,     0,     0,
       0,     0,     0,     0,   159,     0,     0,     0,     0,     0,
       0,   161,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    75,     0,     0,    80,    79,     0,     0,
      19,     0,     0,   106,   102,     0,   253,     0,   254,   133,
      83,    87,     0,   117,   116,   115,    91,     0,     0,     0,
      54,     0,    57,    56,     0,    62,    61,    55,    60,    53,
      65,    45,     0,    47,   197,     0,   220,   222,     0,   201,
     202,   203,   204,   205,   206,     0,     0,     0,     0,     0,
       0,   184,     0,     0,     0,     0,     0,   219,     0,     0,
       0,     0,     0,     0,   149,   139,   175,   176,   177,     0,
     171,     0,     0,     0,     0,   162,     0,   174,   173,   190,
     191,   192,   193,   194,   195,   196,   166,   165,   168,   167,
     169,   170,     0,    34,   279,     0,    38,     0,    22,   257,
     104,   236,   238,     0,   240,   251,   239,   108,   128,   252,
     114,   125,   123,    44,     0,     0,    63,     0,    52,    51,
       0,   188,     0,     0,     0,     0,   182,     0,     0,     0,
     207,     0,     0,     0,   210,   211,   212,   213,   214,     0,
       0,   172,     0,     0,     0,   163,   216,    73,    24,     0,
       0,   273,   265,   271,   269,   272,   267,     0,     0,     0,
     250,   244,   248,     0,    98,     0,     0,    64,   198,     0,
       0,   186,     0,   185,     0,   189,     0,     0,     0,   217,
       0,     0,     0,   180,   178,   251,     0,   268,   270,   266,
       0,   237,   252,     0,    58,    59,   199,   200,   183,   187,
     208,   209,   215,   218,   181,   179,   241,   262,   274,     0,
     110,     0,     0,     0,     0,   107,     0,   275,   263,   249,
     109,     0,   264
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -417,  -417,  -417,   460,  -417,   501,  -417,   227,  -417,   169,
    -417,  -417,  -417,  -417,   237,   -83,   411,  -417,  -417,  -417,
     492,  -417,   205,   116,  -417,  -417,   486,  -417,  -417,  -417,
     412,  -417,  -417,   335,  -158,   -79,  -417,     4,   -68,   -35,
    -417,  -417,   -70,   311,  -417,  -417,  -417,  -113,  -417,  -417,
      23,  -417,   242,  -417,  -417,    20,  -232,  -417,  -126,   252,
    -129,  -154,  -417,  -417,  -417,  -417,  -417,  -417,   304,  -417,
    -417,  -417,  -417,  -417,  -417,  -417,  -417,   125,  -417,  -417,
    -417,  -417,  -417,  -417,  -417,  -417,  -244,   -66,   -80,  -417,
    -417,   -93,  -417,  -417,  -417,  -416,    75,  -417,  -417,  -417,
       2,  -417,    77,   336,  -417,  -417,  -417,  -417,   471,  -417,
    -417,  -417,  -417,    57
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    18,    19,    20,    21,    66,   223,   224,    22,    59,
      23,   125,    24,    25,    82,   137,   220,    26,    27,    28,
      78,   242,   243,   330,   418,    29,    88,    30,    31,    32,
     133,    33,   135,   136,    34,   151,   152,   153,    71,   102,
     103,   156,    72,   148,   225,   303,   304,   130,   464,   515,
     106,   231,   232,   315,    96,   161,   226,   115,   116,   227,
     228,   187,   188,   189,   190,   191,   192,   193,   252,   194,
     195,   196,   197,   198,   345,   199,   356,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   118,   119,
     120,   121,   122,   123,   400,   401,   402,   403,   404,    48,
     405,   460,   461,   462,   309,    35,    36,    56,    57,   406,
     457,   518,    64,   213
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     186,    98,   146,   139,    39,   138,   138,    42,   240,   117,
      38,   336,    49,    50,   358,   359,   360,   361,   362,   363,
     255,   147,   218,   514,   354,    68,   158,   357,   233,   264,
     235,   237,   154,   306,   129,   154,   318,   105,    85,    70,
     450,    99,   491,   267,   251,   424,   159,    80,   211,    83,
      45,   138,   267,    43,   375,    90,   266,   108,   109,   110,
     268,   269,   371,   250,   160,   215,    53,   267,   165,   268,
     269,   376,   100,    97,   508,   416,   267,   451,    86,    38,
     127,   128,   452,    44,   268,   269,   333,   296,   453,   454,
     141,   142,   417,   268,   269,   143,   267,   238,   306,   166,
     255,   276,    54,   420,   104,   455,   450,   430,   101,  -274,
     456,   365,   305,   268,   269,   276,   369,   265,   370,   230,
     346,   347,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   217,   366,   367,
     368,   372,   444,   451,   496,   291,    99,    68,   452,    69,
     459,   311,   111,   214,   453,   454,   167,   290,   305,   291,
     154,    51,   267,   169,   108,   109,   110,   276,   373,   395,
     245,   455,   246,   397,   229,  -274,   456,   100,   337,   338,
     269,   234,   450,   233,   399,    60,    61,   334,   411,   412,
     407,    52,   112,   113,   184,    40,  -242,    41,   392,   476,
     313,   170,   171,   172,    37,   267,   287,   288,   289,   290,
     482,   291,    46,   101,    47,   307,    55,   440,   441,   451,
     427,   428,   268,   269,   452,   267,   314,   267,    58,   117,
     453,   454,   349,   117,   511,   114,   413,   423,   267,   138,
     173,   449,   268,   269,   268,   269,   474,   455,   425,   310,
      62,   350,   456,   267,   316,   268,   269,   351,    63,   111,
      65,   510,  -243,   169,   108,   109,   110,   350,    73,   236,
     268,   269,   521,   426,   293,   300,   174,   294,   301,    74,
     267,    75,   473,   512,   429,   479,    76,   267,   331,   498,
     175,   332,    81,   469,   393,   471,   472,   268,   269,   112,
     113,   170,   171,   172,   268,   269,   443,   447,    77,    84,
     163,   176,   177,   178,   179,   180,   181,   182,   339,   340,
     341,   342,   343,   344,   448,   468,   503,   163,   305,   183,
     184,   169,   108,   109,   110,    91,    87,   185,    94,   477,
     173,    95,   114,   484,   320,   499,   305,    92,   107,   500,
     321,   505,   305,   519,   305,   124,   294,   126,   322,   111,
     129,   131,   134,   140,   132,   108,   250,    69,   110,   170,
     171,   172,   145,   155,   481,   147,   174,   149,   150,   157,
     162,    68,   163,   517,   323,   520,   212,   164,   216,   222,
     175,   219,   239,   169,   108,   109,   110,   104,    14,   112,
     113,   324,   241,   325,   326,   244,   247,   292,   173,   248,
     249,   176,   177,   178,   179,   180,   181,   182,   327,   256,
     257,   298,   328,   302,   258,   259,   260,   111,   261,   183,
     184,   170,   171,   172,   329,   305,   262,   185,   169,   108,
     109,   110,   114,   263,   174,   295,   299,   317,   312,    68,
     355,   352,   319,   364,   374,   394,   409,   414,   175,   421,
     415,   422,   372,   431,   267,   463,   246,   112,   113,   432,
     173,   433,   169,   108,   109,   110,   253,   171,   172,   176,
     177,   178,   179,   180,   181,   182,   434,   435,   436,   111,
     437,   438,   442,   445,   291,   465,   466,   183,   184,   446,
     467,   176,   458,   486,   475,   185,   174,   483,   485,   487,
     114,   171,   172,   488,   489,   173,   490,   492,   494,   493,
     175,   495,   497,    93,    67,   501,   502,   504,   398,   112,
     113,   522,   509,   516,   111,   396,    79,   419,    89,   470,
     168,   176,   177,   178,   179,   180,   181,   182,   270,   173,
     221,   254,   297,   271,   410,   348,   335,   408,   478,   183,
     184,   507,   506,   144,   308,   175,   513,   185,   111,     0,
       0,     0,   114,     0,   112,   113,     0,     0,     0,     0,
     271,     0,     0,     0,     0,   254,   176,   177,   178,   179,
     180,   181,   182,     0,     0,     0,     0,     0,     0,   175,
       0,     0,     0,     0,   183,   184,     0,     0,   112,   113,
       0,     0,   185,     0,     0,     0,     0,   114,     0,     0,
     176,   177,   178,   179,   180,   181,   182,   272,   275,     0,
       0,     0,     0,     0,     0,   276,     0,     0,   183,   184,
       0,     0,     0,     0,     0,     0,   185,   273,     0,     0,
       0,   114,     0,     0,   274,   275,     0,     0,     0,     0,
       0,     0,   276,   277,     0,  -277,  -277,     0,     0,  -277,
    -277,     0,   285,   286,   287,   288,   289,   290,     0,   291,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   278,
     279,   280,   281,   282,     0,     0,   283,   284,  -276,   285,
     286,   287,   288,   289,   290,     1,   291,     0,   271,     0,
       1,     0,     0,     2,     0,     0,     0,     0,     2,     0,
       3,     0,     0,     0,     4,     3,     0,     0,     0,     4,
       5,     0,     0,     6,     7,     5,     0,     0,     6,     7,
     271,     0,     0,     0,     0,     8,     9,     0,     0,     0,
       8,     9,     0,     0,     0,   272,    10,     0,     0,     0,
       0,    10,     0,     0,     0,     0,     0,     0,     0,   439,
       0,     0,     0,     0,     0,   353,     0,     0,     0,     0,
      11,     0,     0,   275,    12,    11,     0,   272,     0,    12,
     276,   277,     0,     0,     0,     0,     0,     0,     0,    13,
       0,   480,     0,     0,    13,    14,     0,   353,   271,     0,
      14,     0,     0,     0,     0,   275,     0,   278,   279,   280,
     281,   282,   276,   277,   283,   284,   271,   285,   286,   287,
     288,   289,   290,     0,   291,     0,     0,     0,     0,     0,
      15,    16,    17,     0,     0,    15,    16,    17,     0,   278,
     279,   280,   281,   282,     0,   272,   283,   284,     0,   285,
     286,   287,   288,   289,   290,     0,   291,     0,     0,     0,
       0,     0,     0,  -277,     0,   353,     0,     0,     0,     0,
       0,     0,     0,   275,     0,     0,     0,     0,     0,     0,
     276,   277,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   275,     0,     0,     0,     0,     0,     0,   276,  -277,
       0,     0,     0,     0,     0,     0,     0,   278,   279,   280,
     281,   282,     0,     0,   283,   284,     0,   285,   286,   287,
     288,   289,   290,     0,   291,  -277,  -277,  -277,   281,   282,
       0,     0,   283,   284,     0,   285,   286,   287,   288,   289,
     290,     0,   291
};

static const yytype_int16 yycheck[] =
{
     129,    69,    95,    86,     2,    85,    86,     5,   166,    75,
       3,     3,    10,    11,   258,   259,   260,   261,   262,   263,
     174,    12,   135,    46,   256,    53,   105,     3,   157,   183,
     159,   160,   102,     3,    78,   105,    50,    72,    89,    35,
      25,     9,   458,   110,   173,   103,    50,    45,   131,    47,
      89,   131,   110,    75,    99,    53,   185,     4,     5,     6,
     127,   128,    17,   106,    68,   133,    19,   110,    89,   127,
     128,   116,    40,    69,   490,    99,   110,    62,   129,     3,
      78,    79,    67,   105,   127,   128,   244,   216,    73,    74,
      88,    89,   116,   127,   128,    91,   110,   163,     3,   120,
     254,   131,    55,   335,    72,    90,    25,   176,    76,    94,
      95,   178,   181,   127,   128,   131,   270,   185,   272,   154,
     249,   250,   276,   277,   278,   279,   280,   281,   282,   283,
     284,   285,   286,   287,   288,   289,   290,   181,   267,   268,
     269,    96,   374,    62,   178,   175,     9,    53,    67,   177,
     120,   230,    99,    59,    73,    74,   177,   173,   181,   175,
     230,    75,   110,     3,     4,     5,     6,   131,   123,   295,
     177,    90,   179,   299,   151,    94,    95,    40,   170,   247,
     128,   158,    25,   312,   177,    16,    17,   178,   317,   318,
     303,   105,   139,   140,   170,     3,   181,     3,   291,   431,
      85,    41,    42,    43,    26,   110,   170,   171,   172,   173,
     442,   175,    89,    76,    93,   120,     3,   371,   372,    62,
     349,   350,   127,   128,    67,   110,   111,   110,   141,   295,
      73,    74,    87,   299,    77,   182,   319,   120,   110,   319,
      80,   399,   127,   128,   127,   128,   103,    90,    87,   229,
       0,   106,    95,   110,   234,   127,   128,   112,   180,    99,
     107,   493,   181,     3,     4,     5,     6,   106,     3,   109,
     127,   128,   516,   112,   178,   178,   116,   181,   181,   179,
     110,   177,   112,   126,   352,   439,    89,   110,   178,   112,
     130,   181,     3,   422,   292,   424,   425,   127,   128,   139,
     140,    41,    42,    43,   127,   128,   374,   178,   122,   118,
     181,   151,   152,   153,   154,   155,   156,   157,   133,   134,
     135,   136,   137,   138,   178,   178,   480,   181,   181,   169,
     170,     3,     4,     5,     6,   120,   122,   177,    92,   432,
      80,   119,   182,   178,    24,   474,   181,   181,     3,   178,
      30,   178,   181,   178,   181,     4,   181,   116,    38,    99,
      78,    88,     3,    41,   177,     4,   106,   177,     6,    41,
      42,    43,   177,   109,   442,    12,   116,   178,   178,   121,
     178,    53,   181,   512,    64,   514,     3,    41,   158,     3,
     130,   107,     3,     3,     4,     5,     6,    72,   107,   139,
     140,    81,     3,    83,    84,   120,   177,    93,    80,   177,
     177,   151,   152,   153,   154,   155,   156,   157,    98,   175,
     177,    44,   102,    89,   177,   177,   177,    99,   177,   169,
     170,    41,    42,    43,   114,   181,   177,   177,     3,     4,
       5,     6,   182,   177,   116,   177,   177,    50,   181,    53,
     150,   177,    88,   178,   177,     3,     3,   177,   130,   178,
     177,    89,    96,   177,   110,    65,   179,   139,   140,   181,
      80,   181,     3,     4,     5,     6,    41,    42,    43,   151,
     152,   153,   154,   155,   156,   157,   178,   178,   178,    99,
     178,   178,   177,    99,   175,     6,     6,   169,   170,   176,
      99,   151,   181,    94,   178,   177,   116,   178,   178,    73,
     182,    42,    43,    73,    73,    80,    94,     3,   178,   121,
     130,   178,   178,    63,    23,   178,   178,   178,   301,   139,
     140,   178,   177,   177,    99,   298,    44,   332,    52,   423,
     128,   151,   152,   153,   154,   155,   156,   157,    17,    80,
     139,   116,   217,    49,   312,   251,   245,   305,   433,   169,
     170,   486,   485,    92,   228,   130,   509,   177,    99,    -1,
      -1,    -1,   182,    -1,   139,   140,    -1,    -1,    -1,    -1,
      49,    -1,    -1,    -1,    -1,   116,   151,   152,   153,   154,
     155,   156,   157,    -1,    -1,    -1,    -1,    -1,    -1,   130,
      -1,    -1,    -1,    -1,   169,   170,    -1,    -1,   139,   140,
      -1,    -1,   177,    -1,    -1,    -1,    -1,   182,    -1,    -1,
     151,   152,   153,   154,   155,   156,   157,    96,   124,    -1,
      -1,    -1,    -1,    -1,    -1,   131,    -1,    -1,   169,   170,
      -1,    -1,    -1,    -1,    -1,    -1,   177,   116,    -1,    -1,
      -1,   182,    -1,    -1,   123,   124,    -1,    -1,    -1,    -1,
      -1,    -1,   131,   132,    -1,   161,   162,    -1,    -1,   165,
     166,    -1,   168,   169,   170,   171,   172,   173,    -1,   175,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   158,
     159,   160,   161,   162,    -1,    -1,   165,   166,     0,   168,
     169,   170,   171,   172,   173,     7,   175,    -1,    49,    -1,
       7,    -1,    -1,    15,    -1,    -1,    -1,    -1,    15,    -1,
      22,    -1,    -1,    -1,    26,    22,    -1,    -1,    -1,    26,
      32,    -1,    -1,    35,    36,    32,    -1,    -1,    35,    36,
      49,    -1,    -1,    -1,    -1,    47,    48,    -1,    -1,    -1,
      47,    48,    -1,    -1,    -1,    96,    58,    -1,    -1,    -1,
      -1,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   110,
      -1,    -1,    -1,    -1,    -1,   116,    -1,    -1,    -1,    -1,
      82,    -1,    -1,   124,    86,    82,    -1,    96,    -1,    86,
     131,   132,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   101,
      -1,   110,    -1,    -1,   101,   107,    -1,   116,    49,    -1,
     107,    -1,    -1,    -1,    -1,   124,    -1,   158,   159,   160,
     161,   162,   131,   132,   165,   166,    49,   168,   169,   170,
     171,   172,   173,    -1,   175,    -1,    -1,    -1,    -1,    -1,
     142,   143,   144,    -1,    -1,   142,   143,   144,    -1,   158,
     159,   160,   161,   162,    -1,    96,   165,   166,    -1,   168,
     169,   170,   171,   172,   173,    -1,   175,    -1,    -1,    -1,
      -1,    -1,    -1,    96,    -1,   116,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   124,    -1,    -1,    -1,    -1,    -1,    -1,
     131,   132,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   124,    -1,    -1,    -1,    -1,    -1,    -1,   131,   132,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   158,   159,   160,
     161,   162,    -1,    -1,   165,   166,    -1,   168,   169,   170,
     171,   172,   173,    -1,   175,   158,   159,   160,   161,   162,
      -1,    -1,   165,   166,    -1,   168,   169,   170,   171,   172,
     173,    -1,   175
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     7,    15,    22,    26,    32,    35,    36,    47,    48,
      58,    82,    86,   101,   107,   142,   143,   144,   184,   185,
     186,   187,   191,   193,   195,   196,   200,   201,   202,   208,
     210,   211,   212,   214,   217,   288,   289,    26,     3,   283,
       3,     3,   283,    75,   105,    89,    89,    93,   282,   283,
     283,    75,   105,    19,    55,     3,   290,   291,   141,   192,
     192,   192,     0,   180,   295,   107,   188,   188,    53,   177,
     220,   221,   225,     3,   179,   177,    89,   122,   203,   203,
     283,     3,   197,   283,   118,    89,   129,   122,   209,   209,
     283,   120,   181,   186,    92,   119,   237,   220,   221,     9,
      40,    76,   222,   223,    72,   222,   233,     3,     4,     5,
       6,    99,   139,   140,   182,   240,   241,   270,   271,   272,
     273,   274,   275,   276,     4,   194,   116,   283,   283,    78,
     230,    88,   177,   213,     3,   215,   216,   198,   271,   198,
      41,   283,   283,   220,   291,   177,   274,    12,   226,   178,
     178,   218,   219,   220,   225,   109,   224,   121,   218,    50,
      68,   238,   178,   181,    41,    89,   120,   177,   213,     3,
      41,    42,    43,    80,   116,   130,   151,   152,   153,   154,
     155,   156,   157,   169,   170,   177,   243,   244,   245,   246,
     247,   248,   249,   250,   252,   253,   254,   255,   256,   258,
     260,   261,   262,   263,   264,   265,   266,   267,   268,   269,
     270,   198,     3,   296,    59,   221,   158,   181,   230,   107,
     199,   199,     3,   189,   190,   227,   239,   242,   243,   233,
     222,   234,   235,   243,   233,   243,   109,   243,   270,     3,
     217,     3,   204,   205,   120,   177,   179,   177,   177,   177,
     106,   243,   251,    41,   116,   244,   175,   177,   177,   177,
     177,   177,   177,   177,   244,   221,   243,   110,   127,   128,
      17,    49,    96,   116,   123,   124,   131,   132,   158,   159,
     160,   161,   162,   165,   166,   168,   169,   170,   171,   172,
     173,   175,    93,   178,   181,   177,   243,   216,    44,   177,
     178,   181,    89,   228,   229,   181,     3,   120,   286,   287,
     238,   218,   181,    85,   111,   236,   238,    50,    50,    88,
      24,    30,    38,    64,    81,    83,    84,    98,   102,   114,
     206,   178,   181,   217,   178,   226,     3,   170,   221,   133,
     134,   135,   136,   137,   138,   257,   243,   243,   251,    87,
     106,   112,   177,   116,   239,   150,   259,     3,   269,   269,
     269,   269,   269,   269,   178,   178,   243,   243,   243,   244,
     244,    17,    96,   123,   177,    99,   116,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   274,   283,     3,   241,   197,   241,   190,   177,
     277,   278,   279,   280,   281,   283,   292,   230,   242,     3,
     235,   243,   243,   198,   177,   177,    99,   116,   207,   205,
     239,   178,    89,   120,   103,    87,   112,   243,   243,   221,
     176,   177,   181,   181,   178,   178,   178,   178,   178,   110,
     244,   244,   177,   221,   239,    99,   176,   178,   178,   217,
      25,    62,    67,    73,    74,    90,    95,   293,   181,   120,
     284,   285,   286,    65,   231,     6,     6,    99,   178,   243,
     206,   243,   243,   112,   103,   178,   239,   274,   260,   244,
     110,   221,   239,   178,   178,   178,    94,    73,    73,    73,
      94,   278,     3,   121,   178,   178,   178,   178,   112,   243,
     178,   178,   178,   244,   178,   178,   285,   279,   278,   177,
     239,    77,   126,   296,    46,   232,   177,   243,   294,   178,
     243,   269,   178
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int16 yyr1[] =
{
       0,   183,   184,   185,   185,   186,   186,   186,   186,   186,
     187,   187,   187,   187,   187,   187,   187,   187,   187,   188,
     188,   189,   189,   190,   190,   191,   191,   191,   192,   192,
     193,   194,   195,   195,   196,   196,   197,   198,   199,   199,
     200,   201,   201,   201,   202,   202,   202,   202,   203,   203,
     204,   204,   205,   206,   206,   206,   206,   206,   206,   206,
     206,   206,   206,   207,   207,   207,   208,   208,   208,   209,
     209,   210,   211,   212,   212,   213,   213,   214,   215,   215,
     216,   217,   217,   217,   218,   218,   219,   219,   220,   220,
     221,   221,   222,   223,   223,   223,   224,   224,   225,   226,
     226,   227,   228,   228,   229,   230,   230,   231,   231,   232,
     232,   233,   233,   234,   234,   235,   236,   236,   236,   237,
     237,   238,   238,   238,   238,   238,   238,   239,   239,   240,
     240,   241,   241,   242,   243,   243,   243,   243,   243,   244,
     244,   244,   244,   244,   244,   244,   244,   244,   244,   244,
     244,   244,   244,   244,   244,   244,   244,   245,   245,   246,
     246,   246,   246,   246,   247,   247,   247,   247,   247,   247,
     247,   247,   247,   247,   247,   248,   248,   248,   249,   249,
     249,   249,   250,   250,   250,   250,   251,   251,   252,   252,
     253,   253,   253,   253,   253,   253,   253,   254,   254,   255,
     256,   257,   257,   257,   257,   257,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   268,   269,
     269,   269,   269,   270,   270,   270,   270,   270,   271,   272,
     272,   273,   273,   274,   275,   276,   277,   277,   278,   278,
     279,   279,   280,   280,   281,   282,   283,   283,   284,   284,
     285,   285,   286,   286,   287,   287,   288,   288,   289,   290,
     290,   291,   292,   292,   292,   293,   293,   293,   293,   293,
     293,   293,   293,   293,   293,   294,   295,   295,   296,   296
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     3,     2,     2,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     5,
       0,     1,     3,     1,     4,     2,     2,     2,     1,     0,
       4,     1,     2,     5,     7,     5,     1,     1,     3,     0,
       5,     2,     3,     2,     8,     7,     6,     7,     3,     0,
       1,     3,     3,     1,     1,     1,     1,     1,     4,     4,
       1,     1,     1,     1,     2,     0,     4,     4,     3,     2,
       0,     4,     2,     8,     5,     3,     0,     5,     1,     3,
       3,     2,     2,     6,     1,     1,     1,     3,     3,     3,
       3,     5,     2,     1,     1,     1,     1,     0,     7,     1,
       0,     1,     1,     0,     2,     2,     0,     4,     0,     2,
       0,     3,     0,     1,     3,     2,     1,     1,     0,     2,
       0,     2,     2,     4,     2,     4,     0,     1,     3,     1,
       0,     1,     3,     2,     1,     1,     1,     1,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     2,     3,     4,     1,     3,     3,     3,     3,     3,
       3,     3,     4,     3,     3,     3,     3,     3,     5,     6,
       5,     6,     4,     6,     3,     5,     4,     5,     4,     5,
       3,     3,     3,     3,     3,     3,     3,     3,     5,     6,
       6,     1,     1,     1,     1,     1,     1,     4,     4,     6,
       4,     4,     4,     4,     4,     6,     4,     5,     6,     1,
       3,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     3,     1,     1,
       1,     4,     1,     3,     2,     1,     1,     3,     1,     5,
       1,     0,     2,     1,     1,     0,     1,     0,     2,     1,
       3,     3,     4,     6,     8,     1,     2,     1,     2,     1,
       2,     1,     1,     1,     0,     1,     1,     0,     1,     3
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


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
        yyerror (&yylloc, result, scanner, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256


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
#if ZSQL_DEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined ZSQL_LTYPE_IS_TRIVIAL && ZSQL_LTYPE_IS_TRIVIAL

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

#  define YY_LOCATION_PRINT(File, Loc)          \
  yy_location_print_ (File, &(Loc))

# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, Location, result, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, zsql::SQLParserResult* result, yyscan_t scanner)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  YYUSE (yylocationp);
  YYUSE (result);
  YYUSE (scanner);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp, zsql::SQLParserResult* result, yyscan_t scanner)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  YY_LOCATION_PRINT (yyo, *yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yytype, yyvaluep, yylocationp, result, scanner);
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule, zsql::SQLParserResult* result, yyscan_t scanner)
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
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                       , &(yylsp[(yyi + 1) - (yynrhs)])                       , result, scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule, result, scanner); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !ZSQL_DEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !ZSQL_DEBUG */


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


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

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
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
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
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
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
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
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
          yyp += yytnamerr (yyp, yyarg[yyi++]);
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
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp, zsql::SQLParserResult* result, yyscan_t scanner)
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);
  YYUSE (result);
  YYUSE (scanner);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yytype)
    {
    case 3: /* IDENTIFIER  */
#line 149 "bison_parser.y"
            { free( (((*yyvaluep).sval)) ); }
#line 1935 "bison_parser.cpp"
        break;

    case 4: /* STRING  */
#line 149 "bison_parser.y"
            { free( (((*yyvaluep).sval)) ); }
#line 1941 "bison_parser.cpp"
        break;

    case 5: /* FLOATVAL  */
#line 147 "bison_parser.y"
            { }
#line 1947 "bison_parser.cpp"
        break;

    case 6: /* INTVAL  */
#line 147 "bison_parser.y"
            { }
#line 1953 "bison_parser.cpp"
        break;

    case 185: /* statement_list  */
#line 150 "bison_parser.y"
            {
	if ((((*yyvaluep).stmt_vec)) != nullptr) {
		for (auto ptr : *(((*yyvaluep).stmt_vec))) {
			delete ptr;
		}
	}
	delete (((*yyvaluep).stmt_vec));
}
#line 1966 "bison_parser.cpp"
        break;

    case 186: /* statement  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).statement)); }
#line 1972 "bison_parser.cpp"
        break;

    case 187: /* preparable_statement  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).statement)); }
#line 1978 "bison_parser.cpp"
        break;

    case 188: /* opt_hints  */
#line 150 "bison_parser.y"
            {
	if ((((*yyvaluep).expr_vec)) != nullptr) {
		for (auto ptr : *(((*yyvaluep).expr_vec))) {
			delete ptr;
		}
	}
	delete (((*yyvaluep).expr_vec));
}
#line 1991 "bison_parser.cpp"
        break;

    case 189: /* hint_list  */
#line 150 "bison_parser.y"
            {
	if ((((*yyvaluep).expr_vec)) != nullptr) {
		for (auto ptr : *(((*yyvaluep).expr_vec))) {
			delete ptr;
		}
	}
	delete (((*yyvaluep).expr_vec));
}
#line 2004 "bison_parser.cpp"
        break;

    case 190: /* hint  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2010 "bison_parser.cpp"
        break;

    case 191: /* transaction_statement  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).transaction_stmt)); }
#line 2016 "bison_parser.cpp"
        break;

    case 193: /* prepare_statement  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).prep_stmt)); }
#line 2022 "bison_parser.cpp"
        break;

    case 194: /* prepare_target_query  */
#line 149 "bison_parser.y"
            { free( (((*yyvaluep).sval)) ); }
#line 2028 "bison_parser.cpp"
        break;

    case 195: /* execute_statement  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).exec_stmt)); }
#line 2034 "bison_parser.cpp"
        break;

    case 196: /* import_statement  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).import_stmt)); }
#line 2040 "bison_parser.cpp"
        break;

    case 197: /* file_type  */
#line 147 "bison_parser.y"
            { }
#line 2046 "bison_parser.cpp"
        break;

    case 198: /* file_path  */
#line 149 "bison_parser.y"
            { free( (((*yyvaluep).sval)) ); }
#line 2052 "bison_parser.cpp"
        break;

    case 199: /* opt_file_type  */
#line 147 "bison_parser.y"
            { }
#line 2058 "bison_parser.cpp"
        break;

    case 200: /* export_statement  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).export_stmt)); }
#line 2064 "bison_parser.cpp"
        break;

    case 201: /* show_statement  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).show_stmt)); }
#line 2070 "bison_parser.cpp"
        break;

    case 202: /* create_statement  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).create_stmt)); }
#line 2076 "bison_parser.cpp"
        break;

    case 203: /* opt_not_exists  */
#line 147 "bison_parser.y"
            { }
#line 2082 "bison_parser.cpp"
        break;

    case 204: /* column_def_commalist  */
#line 150 "bison_parser.y"
            {
	if ((((*yyvaluep).column_vec)) != nullptr) {
		for (auto ptr : *(((*yyvaluep).column_vec))) {
			delete ptr;
		}
	}
	delete (((*yyvaluep).column_vec));
}
#line 2095 "bison_parser.cpp"
        break;

    case 205: /* column_def  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).column_t)); }
#line 2101 "bison_parser.cpp"
        break;

    case 206: /* column_type  */
#line 147 "bison_parser.y"
            { }
#line 2107 "bison_parser.cpp"
        break;

    case 207: /* opt_column_nullable  */
#line 147 "bison_parser.y"
            { }
#line 2113 "bison_parser.cpp"
        break;

    case 208: /* drop_statement  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).drop_stmt)); }
#line 2119 "bison_parser.cpp"
        break;

    case 209: /* opt_exists  */
#line 147 "bison_parser.y"
            { }
#line 2125 "bison_parser.cpp"
        break;

    case 210: /* delete_statement  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).delete_stmt)); }
#line 2131 "bison_parser.cpp"
        break;

    case 211: /* truncate_statement  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).delete_stmt)); }
#line 2137 "bison_parser.cpp"
        break;

    case 212: /* insert_statement  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).insert_stmt)); }
#line 2143 "bison_parser.cpp"
        break;

    case 213: /* opt_column_list  */
#line 150 "bison_parser.y"
            {
	if ((((*yyvaluep).str_vec)) != nullptr) {
		for (auto ptr : *(((*yyvaluep).str_vec))) {
			delete ptr;
		}
	}
	delete (((*yyvaluep).str_vec));
}
#line 2156 "bison_parser.cpp"
        break;

    case 214: /* update_statement  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).update_stmt)); }
#line 2162 "bison_parser.cpp"
        break;

    case 215: /* update_clause_commalist  */
#line 150 "bison_parser.y"
            {
	if ((((*yyvaluep).update_vec)) != nullptr) {
		for (auto ptr : *(((*yyvaluep).update_vec))) {
			delete ptr;
		}
	}
	delete (((*yyvaluep).update_vec));
}
#line 2175 "bison_parser.cpp"
        break;

    case 216: /* update_clause  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).update_t)); }
#line 2181 "bison_parser.cpp"
        break;

    case 217: /* select_statement  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).select_stmt)); }
#line 2187 "bison_parser.cpp"
        break;

    case 218: /* select_within_set_operation  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).select_stmt)); }
#line 2193 "bison_parser.cpp"
        break;

    case 219: /* select_within_set_operation_no_parentheses  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).select_stmt)); }
#line 2199 "bison_parser.cpp"
        break;

    case 220: /* select_with_paren  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).select_stmt)); }
#line 2205 "bison_parser.cpp"
        break;

    case 221: /* select_no_paren  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).select_stmt)); }
#line 2211 "bison_parser.cpp"
        break;

    case 222: /* set_operator  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).set_operator_t)); }
#line 2217 "bison_parser.cpp"
        break;

    case 223: /* set_type  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).set_operator_t)); }
#line 2223 "bison_parser.cpp"
        break;

    case 224: /* opt_all  */
#line 147 "bison_parser.y"
            { }
#line 2229 "bison_parser.cpp"
        break;

    case 225: /* select_clause  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).select_stmt)); }
#line 2235 "bison_parser.cpp"
        break;

    case 226: /* opt_distinct  */
#line 147 "bison_parser.y"
            { }
#line 2241 "bison_parser.cpp"
        break;

    case 227: /* select_list  */
#line 150 "bison_parser.y"
            {
	if ((((*yyvaluep).expr_vec)) != nullptr) {
		for (auto ptr : *(((*yyvaluep).expr_vec))) {
			delete ptr;
		}
	}
	delete (((*yyvaluep).expr_vec));
}
#line 2254 "bison_parser.cpp"
        break;

    case 228: /* opt_from_clause  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).table)); }
#line 2260 "bison_parser.cpp"
        break;

    case 229: /* from_clause  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).table)); }
#line 2266 "bison_parser.cpp"
        break;

    case 230: /* opt_where  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2272 "bison_parser.cpp"
        break;

    case 231: /* opt_group  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).group_t)); }
#line 2278 "bison_parser.cpp"
        break;

    case 232: /* opt_having  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2284 "bison_parser.cpp"
        break;

    case 233: /* opt_order  */
#line 150 "bison_parser.y"
            {
	if ((((*yyvaluep).order_vec)) != nullptr) {
		for (auto ptr : *(((*yyvaluep).order_vec))) {
			delete ptr;
		}
	}
	delete (((*yyvaluep).order_vec));
}
#line 2297 "bison_parser.cpp"
        break;

    case 234: /* order_list  */
#line 150 "bison_parser.y"
            {
	if ((((*yyvaluep).order_vec)) != nullptr) {
		for (auto ptr : *(((*yyvaluep).order_vec))) {
			delete ptr;
		}
	}
	delete (((*yyvaluep).order_vec));
}
#line 2310 "bison_parser.cpp"
        break;

    case 235: /* order_desc  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).order)); }
#line 2316 "bison_parser.cpp"
        break;

    case 236: /* opt_order_type  */
#line 147 "bison_parser.y"
            { }
#line 2322 "bison_parser.cpp"
        break;

    case 237: /* opt_top  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).limit)); }
#line 2328 "bison_parser.cpp"
        break;

    case 238: /* opt_limit  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).limit)); }
#line 2334 "bison_parser.cpp"
        break;

    case 239: /* expr_list  */
#line 150 "bison_parser.y"
            {
	if ((((*yyvaluep).expr_vec)) != nullptr) {
		for (auto ptr : *(((*yyvaluep).expr_vec))) {
			delete ptr;
		}
	}
	delete (((*yyvaluep).expr_vec));
}
#line 2347 "bison_parser.cpp"
        break;

    case 240: /* opt_literal_list  */
#line 150 "bison_parser.y"
            {
	if ((((*yyvaluep).expr_vec)) != nullptr) {
		for (auto ptr : *(((*yyvaluep).expr_vec))) {
			delete ptr;
		}
	}
	delete (((*yyvaluep).expr_vec));
}
#line 2360 "bison_parser.cpp"
        break;

    case 241: /* literal_list  */
#line 150 "bison_parser.y"
            {
	if ((((*yyvaluep).expr_vec)) != nullptr) {
		for (auto ptr : *(((*yyvaluep).expr_vec))) {
			delete ptr;
		}
	}
	delete (((*yyvaluep).expr_vec));
}
#line 2373 "bison_parser.cpp"
        break;

    case 242: /* expr_alias  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2379 "bison_parser.cpp"
        break;

    case 243: /* expr  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2385 "bison_parser.cpp"
        break;

    case 244: /* operand  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2391 "bison_parser.cpp"
        break;

    case 245: /* scalar_expr  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2397 "bison_parser.cpp"
        break;

    case 246: /* unary_expr  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2403 "bison_parser.cpp"
        break;

    case 247: /* binary_expr  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2409 "bison_parser.cpp"
        break;

    case 248: /* logic_expr  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2415 "bison_parser.cpp"
        break;

    case 249: /* in_expr  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2421 "bison_parser.cpp"
        break;

    case 250: /* case_expr  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2427 "bison_parser.cpp"
        break;

    case 251: /* case_list  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2433 "bison_parser.cpp"
        break;

    case 252: /* exists_expr  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2439 "bison_parser.cpp"
        break;

    case 253: /* comp_expr  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2445 "bison_parser.cpp"
        break;

    case 254: /* function_expr  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2451 "bison_parser.cpp"
        break;

    case 255: /* extract_expr  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2457 "bison_parser.cpp"
        break;

    case 256: /* cast_expr  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2463 "bison_parser.cpp"
        break;

    case 257: /* datetime_field  */
#line 147 "bison_parser.y"
            { }
#line 2469 "bison_parser.cpp"
        break;

    case 258: /* array_expr  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2475 "bison_parser.cpp"
        break;

    case 259: /* point_expr  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2481 "bison_parser.cpp"
        break;

    case 260: /* srid_expr  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2487 "bison_parser.cpp"
        break;

    case 261: /* count_expr  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2493 "bison_parser.cpp"
        break;

    case 262: /* min_expr  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2499 "bison_parser.cpp"
        break;

    case 263: /* max_expr  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2505 "bison_parser.cpp"
        break;

    case 264: /* sum_expr  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2511 "bison_parser.cpp"
        break;

    case 265: /* avg_expr  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2517 "bison_parser.cpp"
        break;

    case 266: /* distance_expr  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2523 "bison_parser.cpp"
        break;

    case 267: /* array_index  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2529 "bison_parser.cpp"
        break;

    case 268: /* between_expr  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2535 "bison_parser.cpp"
        break;

    case 269: /* column_name  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2541 "bison_parser.cpp"
        break;

    case 270: /* literal  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2547 "bison_parser.cpp"
        break;

    case 271: /* string_literal  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2553 "bison_parser.cpp"
        break;

    case 272: /* bool_literal  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2559 "bison_parser.cpp"
        break;

    case 273: /* num_literal  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2565 "bison_parser.cpp"
        break;

    case 274: /* int_literal  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2571 "bison_parser.cpp"
        break;

    case 275: /* null_literal  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2577 "bison_parser.cpp"
        break;

    case 276: /* param_expr  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2583 "bison_parser.cpp"
        break;

    case 277: /* table_ref  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).table)); }
#line 2589 "bison_parser.cpp"
        break;

    case 278: /* table_ref_atomic  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).table)); }
#line 2595 "bison_parser.cpp"
        break;

    case 279: /* nonjoin_table_ref_atomic  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).table)); }
#line 2601 "bison_parser.cpp"
        break;

    case 280: /* table_ref_commalist  */
#line 150 "bison_parser.y"
            {
	if ((((*yyvaluep).table_vec)) != nullptr) {
		for (auto ptr : *(((*yyvaluep).table_vec))) {
			delete ptr;
		}
	}
	delete (((*yyvaluep).table_vec));
}
#line 2614 "bison_parser.cpp"
        break;

    case 281: /* table_ref_name  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).table)); }
#line 2620 "bison_parser.cpp"
        break;

    case 282: /* table_ref_name_no_alias  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).table)); }
#line 2626 "bison_parser.cpp"
        break;

    case 283: /* table_name  */
#line 148 "bison_parser.y"
            { free( (((*yyvaluep).table_name).name) ); free( (((*yyvaluep).table_name).schema) ); }
#line 2632 "bison_parser.cpp"
        break;

    case 284: /* table_alias  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).alias_t)); }
#line 2638 "bison_parser.cpp"
        break;

    case 285: /* opt_table_alias  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).alias_t)); }
#line 2644 "bison_parser.cpp"
        break;

    case 286: /* alias  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).alias_t)); }
#line 2650 "bison_parser.cpp"
        break;

    case 287: /* opt_alias  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).alias_t)); }
#line 2656 "bison_parser.cpp"
        break;

    case 288: /* opt_with_clause  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).with_description_vec)); }
#line 2662 "bison_parser.cpp"
        break;

    case 289: /* with_clause  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).with_description_vec)); }
#line 2668 "bison_parser.cpp"
        break;

    case 290: /* with_description_list  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).with_description_vec)); }
#line 2674 "bison_parser.cpp"
        break;

    case 291: /* with_description  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).with_description_t)); }
#line 2680 "bison_parser.cpp"
        break;

    case 292: /* join_clause  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).table)); }
#line 2686 "bison_parser.cpp"
        break;

    case 293: /* opt_join_type  */
#line 147 "bison_parser.y"
            { }
#line 2692 "bison_parser.cpp"
        break;

    case 294: /* join_condition  */
#line 158 "bison_parser.y"
            { delete (((*yyvaluep).expr)); }
#line 2698 "bison_parser.cpp"
        break;

    case 296: /* ident_commalist  */
#line 150 "bison_parser.y"
            {
	if ((((*yyvaluep).str_vec)) != nullptr) {
		for (auto ptr : *(((*yyvaluep).str_vec))) {
			delete ptr;
		}
	}
	delete (((*yyvaluep).str_vec));
}
#line 2711 "bison_parser.cpp"
        break;

      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (zsql::SQLParserResult* result, yyscan_t scanner)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

/* Location data for the lookahead symbol.  */
static YYLTYPE yyloc_default
# if defined ZSQL_LTYPE_IS_TRIVIAL && ZSQL_LTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
YYLTYPE yylloc = yyloc_default;

    /* Number of syntax errors so far.  */
    int yynerrs;

    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.
       'yyls': related to locations.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    /* The location stack.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls;
    YYLTYPE *yylsp;

    /* The locations where the error started and ended.  */
    YYLTYPE yyerror_range[3];

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yylsp = yyls = yylsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

/* User initialization code.  */
#line 73 "bison_parser.y"
{
	// Initialize
	yylloc.first_column = 0;
	yylloc.last_column = 0;
	yylloc.first_line = 0;
	yylloc.last_line = 0;
	yylloc.total_column = 0;
	yylloc.string_length = 0;
}

#line 2829 "bison_parser.cpp"

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

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
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
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
# undef YYSTACK_RELOCATE
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

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, &yylloc, scanner);
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
  case 2:
#line 273 "bison_parser.y"
                                             {
			for (SQLStatement* stmt : *(yyvsp[-1].stmt_vec)) {
				// Transfers ownership of the statement.
				result->addStatement(stmt);
			}

			unsigned param_id = 0;
			for (void* param : yyloc.param_list) {
				if (param != nullptr) {
					Expr* expr = (Expr*) param;
					expr->ival = param_id;
					result->addParameter(expr);
					++param_id;
				}
			}
			delete (yyvsp[-1].stmt_vec);
		}
#line 3044 "bison_parser.cpp"
    break;

  case 3:
#line 294 "bison_parser.y"
                          {
			(yyvsp[0].statement)->stringLength = yylloc.string_length;
			yylloc.string_length = 0;
			(yyval.stmt_vec) = new std::vector<SQLStatement*>();
			(yyval.stmt_vec)->push_back((yyvsp[0].statement));
		}
#line 3055 "bison_parser.cpp"
    break;

  case 4:
#line 300 "bison_parser.y"
                                             {
			(yyvsp[0].statement)->stringLength = yylloc.string_length;
			yylloc.string_length = 0;
			(yyvsp[-2].stmt_vec)->push_back((yyvsp[0].statement));
			(yyval.stmt_vec) = (yyvsp[-2].stmt_vec);
		}
#line 3066 "bison_parser.cpp"
    break;

  case 5:
#line 309 "bison_parser.y"
                                            {
			(yyval.statement) = (yyvsp[-1].prep_stmt);
			(yyval.statement)->hints = (yyvsp[0].expr_vec);
		}
#line 3075 "bison_parser.cpp"
    break;

  case 6:
#line 313 "bison_parser.y"
                                               {
			(yyval.statement) = (yyvsp[-1].statement);
			(yyval.statement)->hints = (yyvsp[0].expr_vec);
		}
#line 3084 "bison_parser.cpp"
    break;

  case 7:
#line 317 "bison_parser.y"
                               {
			(yyval.statement) = (yyvsp[0].show_stmt);
		}
#line 3092 "bison_parser.cpp"
    break;

  case 8:
#line 320 "bison_parser.y"
                                 {
			(yyval.statement) = (yyvsp[0].import_stmt);
		 }
#line 3100 "bison_parser.cpp"
    break;

  case 9:
#line 323 "bison_parser.y"
                                 {
			(yyval.statement) = (yyvsp[0].export_stmt);
		 }
#line 3108 "bison_parser.cpp"
    break;

  case 10:
#line 330 "bison_parser.y"
                                 { (yyval.statement) = (yyvsp[0].select_stmt); }
#line 3114 "bison_parser.cpp"
    break;

  case 11:
#line 331 "bison_parser.y"
                                 { (yyval.statement) = (yyvsp[0].create_stmt); }
#line 3120 "bison_parser.cpp"
    break;

  case 12:
#line 332 "bison_parser.y"
                                 { (yyval.statement) = (yyvsp[0].insert_stmt); }
#line 3126 "bison_parser.cpp"
    break;

  case 13:
#line 333 "bison_parser.y"
                                 { (yyval.statement) = (yyvsp[0].delete_stmt); }
#line 3132 "bison_parser.cpp"
    break;

  case 14:
#line 334 "bison_parser.y"
                                   { (yyval.statement) = (yyvsp[0].delete_stmt); }
#line 3138 "bison_parser.cpp"
    break;

  case 15:
#line 335 "bison_parser.y"
                                 { (yyval.statement) = (yyvsp[0].update_stmt); }
#line 3144 "bison_parser.cpp"
    break;

  case 16:
#line 336 "bison_parser.y"
                               { (yyval.statement) = (yyvsp[0].drop_stmt); }
#line 3150 "bison_parser.cpp"
    break;

  case 17:
#line 337 "bison_parser.y"
                                  { (yyval.statement) = (yyvsp[0].exec_stmt); }
#line 3156 "bison_parser.cpp"
    break;

  case 18:
#line 338 "bison_parser.y"
                                      { (yyval.statement) = (yyvsp[0].transaction_stmt); }
#line 3162 "bison_parser.cpp"
    break;

  case 19:
#line 347 "bison_parser.y"
                                { (yyval.expr_vec) = (yyvsp[-1].expr_vec); }
#line 3168 "bison_parser.cpp"
    break;

  case 20:
#line 348 "bison_parser.y"
                { (yyval.expr_vec) = nullptr; }
#line 3174 "bison_parser.cpp"
    break;

  case 21:
#line 353 "bison_parser.y"
               { (yyval.expr_vec) = new std::vector<Expr*>(); (yyval.expr_vec)->push_back((yyvsp[0].expr)); }
#line 3180 "bison_parser.cpp"
    break;

  case 22:
#line 354 "bison_parser.y"
                             { (yyvsp[-2].expr_vec)->push_back((yyvsp[0].expr)); (yyval.expr_vec) = (yyvsp[-2].expr_vec); }
#line 3186 "bison_parser.cpp"
    break;

  case 23:
#line 358 "bison_parser.y"
                   {
		(yyval.expr) = Expr::make(kExprHint);
		(yyval.expr)->name = (yyvsp[0].sval);
	}
#line 3195 "bison_parser.cpp"
    break;

  case 24:
#line 362 "bison_parser.y"
                                          {
			(yyval.expr) = Expr::make(kExprHint);
			(yyval.expr)->name = (yyvsp[-3].sval);
			(yyval.expr)->exprList = (yyvsp[-1].expr_vec);
		}
#line 3205 "bison_parser.cpp"
    break;

  case 25:
#line 374 "bison_parser.y"
                                  {
            (yyval.transaction_stmt) = new TransactionStatement(kBeginTransaction);
        }
#line 3213 "bison_parser.cpp"
    break;

  case 26:
#line 377 "bison_parser.y"
                                       {
            (yyval.transaction_stmt) = new TransactionStatement(kRollbackTransaction);
        }
#line 3221 "bison_parser.cpp"
    break;

  case 27:
#line 380 "bison_parser.y"
                                     {
            (yyval.transaction_stmt) = new TransactionStatement(kCommitTransaction);
        }
#line 3229 "bison_parser.cpp"
    break;

  case 30:
#line 394 "bison_parser.y"
                                                     {
		(yyval.prep_stmt) = new PrepareStatement();
		(yyval.prep_stmt)->name = (yyvsp[-2].sval);
		(yyval.prep_stmt)->query = (yyvsp[0].sval);
	}
#line 3239 "bison_parser.cpp"
    break;

  case 32:
#line 404 "bison_parser.y"
                           {
		(yyval.exec_stmt) = new ExecuteStatement();
		(yyval.exec_stmt)->name = (yyvsp[0].sval);
	}
#line 3248 "bison_parser.cpp"
    break;

  case 33:
#line 408 "bison_parser.y"
                                                            {
			(yyval.exec_stmt) = new ExecuteStatement();
			(yyval.exec_stmt)->name = (yyvsp[-3].sval);
			(yyval.exec_stmt)->parameters = (yyvsp[-1].expr_vec);
		}
#line 3258 "bison_parser.cpp"
    break;

  case 34:
#line 422 "bison_parser.y"
                                                             {
		(yyval.import_stmt) = new ImportStatement((yyvsp[-4].import_type_t));
		(yyval.import_stmt)->filePath = (yyvsp[-2].sval);
		(yyval.import_stmt)->schema = (yyvsp[0].table_name).schema;
		(yyval.import_stmt)->tableName = (yyvsp[0].table_name).name;
	}
#line 3269 "bison_parser.cpp"
    break;

  case 35:
#line 428 "bison_parser.y"
                                                             {
			(yyval.import_stmt) = new ImportStatement((yyvsp[0].import_type_t));
			(yyval.import_stmt)->filePath = (yyvsp[-1].sval);
			(yyval.import_stmt)->schema = (yyvsp[-3].table_name).schema;
			(yyval.import_stmt)->tableName = (yyvsp[-3].table_name).name;
		}
#line 3280 "bison_parser.cpp"
    break;

  case 36:
#line 437 "bison_parser.y"
                           {
			if (strcasecmp((yyvsp[0].sval), "json") == 0) {
				(yyval.import_type_t) = kImportJSON;
			} else if (strcasecmp((yyvsp[0].sval), "lz4") == 0) {
				(yyval.import_type_t) = kImportLZ4;
			} else {
				free((yyvsp[0].sval));
				yyerror(&yyloc, result, scanner, "File type is unknown.");
				YYERROR;
			}
			free((yyvsp[0].sval));
		}
#line 3297 "bison_parser.cpp"
    break;

  case 37:
#line 452 "bison_parser.y"
                               { (yyval.sval) = strdup((yyvsp[0].expr)->name); delete (yyvsp[0].expr); }
#line 3303 "bison_parser.cpp"
    break;

  case 38:
#line 456 "bison_parser.y"
                              {
		(yyval.import_type_t) = (yyvsp[0].import_type_t);
	}
#line 3311 "bison_parser.cpp"
    break;

  case 39:
#line 459 "bison_parser.y"
                             { (yyval.import_type_t) = kImportAuto; }
#line 3317 "bison_parser.cpp"
    break;

  case 40:
#line 468 "bison_parser.y"
                                                   {
		(yyval.export_stmt) = new ExportStatement((yyvsp[0].import_type_t));
		(yyval.export_stmt)->filePath = (yyvsp[-1].sval);
		(yyval.export_stmt)->schema = (yyvsp[-3].table_name).schema;
		(yyval.export_stmt)->tableName = (yyvsp[-3].table_name).name;
	}
#line 3328 "bison_parser.cpp"
    break;

  case 41:
#line 482 "bison_parser.y"
                            {
			(yyval.show_stmt) = new ShowStatement(kShowTables);
		}
#line 3336 "bison_parser.cpp"
    break;

  case 42:
#line 485 "bison_parser.y"
                                        {
			(yyval.show_stmt) = new ShowStatement(kShowColumns);
			(yyval.show_stmt)->schema = (yyvsp[0].table_name).schema;
			(yyval.show_stmt)->name = (yyvsp[0].table_name).name;
		}
#line 3346 "bison_parser.cpp"
    break;

  case 43:
#line 490 "bison_parser.y"
                                    {
			(yyval.show_stmt) = new ShowStatement(kShowColumns);
			(yyval.show_stmt)->schema = (yyvsp[0].table_name).schema;
			(yyval.show_stmt)->name = (yyvsp[0].table_name).name;
		}
#line 3356 "bison_parser.cpp"
    break;

  case 44:
#line 504 "bison_parser.y"
                                                                                      {
			(yyval.create_stmt) = new CreateStatement(kCreateTableFromTbl);
			(yyval.create_stmt)->ifNotExists = (yyvsp[-5].bval);
			(yyval.create_stmt)->schema = (yyvsp[-4].table_name).schema;
			(yyval.create_stmt)->tableName = (yyvsp[-4].table_name).name;
			if (strcasecmp((yyvsp[-2].sval), "tbl") != 0) {
				free((yyvsp[-2].sval));
				yyerror(&yyloc, result, scanner, "File type is unknown.");
			 	YYERROR;
			}
			free((yyvsp[-2].sval));
			(yyval.create_stmt)->filePath = (yyvsp[0].sval);
		}
#line 3374 "bison_parser.cpp"
    break;

  case 45:
#line 517 "bison_parser.y"
                                                                                    {
			(yyval.create_stmt) = new CreateStatement(kCreateTable);
			(yyval.create_stmt)->ifNotExists = (yyvsp[-4].bval);
			(yyval.create_stmt)->schema = (yyvsp[-3].table_name).schema;
			(yyval.create_stmt)->tableName = (yyvsp[-3].table_name).name;
			(yyval.create_stmt)->columns = (yyvsp[-1].column_vec);
		}
#line 3386 "bison_parser.cpp"
    break;

  case 46:
#line 524 "bison_parser.y"
                                                                           {
			(yyval.create_stmt) = new CreateStatement(kCreateTable);
			(yyval.create_stmt)->ifNotExists = (yyvsp[-3].bval);
			(yyval.create_stmt)->schema = (yyvsp[-2].table_name).schema;
			(yyval.create_stmt)->tableName = (yyvsp[-2].table_name).name;
			(yyval.create_stmt)->select = (yyvsp[0].select_stmt);
		}
#line 3398 "bison_parser.cpp"
    break;

  case 47:
#line 531 "bison_parser.y"
                                                                                          {
			(yyval.create_stmt) = new CreateStatement(kCreateView);
			(yyval.create_stmt)->ifNotExists = (yyvsp[-4].bval);
			(yyval.create_stmt)->schema = (yyvsp[-3].table_name).schema;
			(yyval.create_stmt)->tableName = (yyvsp[-3].table_name).name;
			(yyval.create_stmt)->viewColumns = (yyvsp[-2].str_vec);
			(yyval.create_stmt)->select = (yyvsp[0].select_stmt);
		}
#line 3411 "bison_parser.cpp"
    break;

  case 48:
#line 542 "bison_parser.y"
                              { (yyval.bval) = true; }
#line 3417 "bison_parser.cpp"
    break;

  case 49:
#line 543 "bison_parser.y"
                            { (yyval.bval) = false; }
#line 3423 "bison_parser.cpp"
    break;

  case 50:
#line 547 "bison_parser.y"
                           { (yyval.column_vec) = new std::vector<ColumnDefinition*>(); (yyval.column_vec)->push_back((yyvsp[0].column_t)); }
#line 3429 "bison_parser.cpp"
    break;

  case 51:
#line 548 "bison_parser.y"
                                                    { (yyvsp[-2].column_vec)->push_back((yyvsp[0].column_t)); (yyval.column_vec) = (yyvsp[-2].column_vec); }
#line 3435 "bison_parser.cpp"
    break;

  case 52:
#line 552 "bison_parser.y"
                                                           {
			(yyval.column_t) = new ColumnDefinition((yyvsp[-2].sval), (yyvsp[-1].column_type_t), (yyvsp[0].bval));
		}
#line 3443 "bison_parser.cpp"
    break;

  case 53:
#line 558 "bison_parser.y"
                    { (yyval.column_type_t) = ColumnType{DataType::INT}; }
#line 3449 "bison_parser.cpp"
    break;

  case 54:
#line 559 "bison_parser.y"
                        { (yyval.column_type_t) = ColumnType{DataType::INT}; }
#line 3455 "bison_parser.cpp"
    break;

  case 55:
#line 560 "bison_parser.y"
                     { (yyval.column_type_t) = ColumnType{DataType::LONG}; }
#line 3461 "bison_parser.cpp"
    break;

  case 56:
#line 561 "bison_parser.y"
                      { (yyval.column_type_t) = ColumnType{DataType::FLOAT}; }
#line 3467 "bison_parser.cpp"
    break;

  case 57:
#line 562 "bison_parser.y"
                       { (yyval.column_type_t) = ColumnType{DataType::DOUBLE}; }
#line 3473 "bison_parser.cpp"
    break;

  case 58:
#line 563 "bison_parser.y"
                                       { (yyval.column_type_t) = ColumnType{DataType::VARCHAR, (yyvsp[-1].ival)}; }
#line 3479 "bison_parser.cpp"
    break;

  case 59:
#line 564 "bison_parser.y"
                                    { (yyval.column_type_t) = ColumnType{DataType::CHAR, (yyvsp[-1].ival)}; }
#line 3485 "bison_parser.cpp"
    break;

  case 60:
#line 565 "bison_parser.y"
                     { (yyval.column_type_t) = ColumnType{DataType::TEXT}; }
#line 3491 "bison_parser.cpp"
    break;

  case 61:
#line 566 "bison_parser.y"
                         { (yyval.column_type_t) = ColumnType{DataType::DATETIME}; }
#line 3497 "bison_parser.cpp"
    break;

  case 62:
#line 567 "bison_parser.y"
                     { (yyval.column_type_t) = ColumnType{DataType::DATE}; }
#line 3503 "bison_parser.cpp"
    break;

  case 63:
#line 571 "bison_parser.y"
                     { (yyval.bval) = true; }
#line 3509 "bison_parser.cpp"
    break;

  case 64:
#line 572 "bison_parser.y"
                         { (yyval.bval) = false; }
#line 3515 "bison_parser.cpp"
    break;

  case 65:
#line 573 "bison_parser.y"
                            { (yyval.bval) = false; }
#line 3521 "bison_parser.cpp"
    break;

  case 66:
#line 583 "bison_parser.y"
                                                 {
			(yyval.drop_stmt) = new DropStatement(kDropTable);
			(yyval.drop_stmt)->ifExists = (yyvsp[-1].bval);
			(yyval.drop_stmt)->schema = (yyvsp[0].table_name).schema;
			(yyval.drop_stmt)->name = (yyvsp[0].table_name).name;
		}
#line 3532 "bison_parser.cpp"
    break;

  case 67:
#line 589 "bison_parser.y"
                                                {
			(yyval.drop_stmt) = new DropStatement(kDropView);
			(yyval.drop_stmt)->ifExists = (yyvsp[-1].bval);
			(yyval.drop_stmt)->schema = (yyvsp[0].table_name).schema;
			(yyval.drop_stmt)->name = (yyvsp[0].table_name).name;
		}
#line 3543 "bison_parser.cpp"
    break;

  case 68:
#line 595 "bison_parser.y"
                                              {
			(yyval.drop_stmt) = new DropStatement(kDropPreparedStatement);
			(yyval.drop_stmt)->ifExists = false;
			(yyval.drop_stmt)->name = (yyvsp[0].sval);
		}
#line 3553 "bison_parser.cpp"
    break;

  case 69:
#line 603 "bison_parser.y"
                            { (yyval.bval) = true; }
#line 3559 "bison_parser.cpp"
    break;

  case 70:
#line 604 "bison_parser.y"
                            { (yyval.bval) = false; }
#line 3565 "bison_parser.cpp"
    break;

  case 71:
#line 613 "bison_parser.y"
                                                 {
			(yyval.delete_stmt) = new DeleteStatement();
			(yyval.delete_stmt)->schema = (yyvsp[-1].table_name).schema;
			(yyval.delete_stmt)->tableName = (yyvsp[-1].table_name).name;
			(yyval.delete_stmt)->expr = (yyvsp[0].expr);
		}
#line 3576 "bison_parser.cpp"
    break;

  case 72:
#line 622 "bison_parser.y"
                                    {
			(yyval.delete_stmt) = new DeleteStatement();
			(yyval.delete_stmt)->schema = (yyvsp[0].table_name).schema;
			(yyval.delete_stmt)->tableName = (yyvsp[0].table_name).name;
		}
#line 3586 "bison_parser.cpp"
    break;

  case 73:
#line 635 "bison_parser.y"
                                                                                   {
			(yyval.insert_stmt) = new InsertStatement(kInsertValues);
			(yyval.insert_stmt)->schema = (yyvsp[-5].table_name).schema;
			(yyval.insert_stmt)->tableName = (yyvsp[-5].table_name).name;
			(yyval.insert_stmt)->columns = (yyvsp[-4].str_vec);
			(yyval.insert_stmt)->values = (yyvsp[-1].expr_vec);
		}
#line 3598 "bison_parser.cpp"
    break;

  case 74:
#line 642 "bison_parser.y"
                                                                       {
			(yyval.insert_stmt) = new InsertStatement(kInsertSelect);
			(yyval.insert_stmt)->schema = (yyvsp[-2].table_name).schema;
			(yyval.insert_stmt)->tableName = (yyvsp[-2].table_name).name;
			(yyval.insert_stmt)->columns = (yyvsp[-1].str_vec);
			(yyval.insert_stmt)->select = (yyvsp[0].select_stmt);
		}
#line 3610 "bison_parser.cpp"
    break;

  case 75:
#line 653 "bison_parser.y"
                                        { (yyval.str_vec) = (yyvsp[-1].str_vec); }
#line 3616 "bison_parser.cpp"
    break;

  case 76:
#line 654 "bison_parser.y"
                            { (yyval.str_vec) = nullptr; }
#line 3622 "bison_parser.cpp"
    break;

  case 77:
#line 664 "bison_parser.y"
                                                                             {
		(yyval.update_stmt) = new UpdateStatement();
		(yyval.update_stmt)->table = (yyvsp[-3].table);
		(yyval.update_stmt)->updates = (yyvsp[-1].update_vec);
		(yyval.update_stmt)->where = (yyvsp[0].expr);
	}
#line 3633 "bison_parser.cpp"
    break;

  case 78:
#line 673 "bison_parser.y"
                              { (yyval.update_vec) = new std::vector<UpdateClause*>(); (yyval.update_vec)->push_back((yyvsp[0].update_t)); }
#line 3639 "bison_parser.cpp"
    break;

  case 79:
#line 674 "bison_parser.y"
                                                          { (yyvsp[-2].update_vec)->push_back((yyvsp[0].update_t)); (yyval.update_vec) = (yyvsp[-2].update_vec); }
#line 3645 "bison_parser.cpp"
    break;

  case 80:
#line 678 "bison_parser.y"
                                    {
			(yyval.update_t) = new UpdateClause();
			(yyval.update_t)->column = (yyvsp[-2].sval);
			(yyval.update_t)->value = (yyvsp[0].expr);
		}
#line 3655 "bison_parser.cpp"
    break;

  case 81:
#line 690 "bison_parser.y"
                                                  {
			(yyval.select_stmt) = (yyvsp[0].select_stmt);
			(yyval.select_stmt)->withDescriptions = (yyvsp[-1].with_description_vec);
		}
#line 3664 "bison_parser.cpp"
    break;

  case 82:
#line 694 "bison_parser.y"
                                                {
			(yyval.select_stmt) = (yyvsp[0].select_stmt);
			(yyval.select_stmt)->withDescriptions = (yyvsp[-1].with_description_vec);
		}
#line 3673 "bison_parser.cpp"
    break;

  case 83:
#line 698 "bison_parser.y"
                                                                                                               {
			(yyval.select_stmt) = (yyvsp[-4].select_stmt);
			if ((yyval.select_stmt)->setOperations == nullptr) {
				(yyval.select_stmt)->setOperations = new std::vector<SetOperation*>();
			}
			(yyval.select_stmt)->setOperations->push_back((yyvsp[-3].set_operator_t));
			(yyval.select_stmt)->setOperations->back()->nestedSelectStatement = (yyvsp[-2].select_stmt);
			(yyval.select_stmt)->setOperations->back()->resultOrder = (yyvsp[-1].order_vec);
			(yyval.select_stmt)->setOperations->back()->resultLimit = (yyvsp[0].limit);
			(yyval.select_stmt)->withDescriptions = (yyvsp[-5].with_description_vec);
		}
#line 3689 "bison_parser.cpp"
    break;

  case 86:
#line 716 "bison_parser.y"
                              { (yyval.select_stmt) = (yyvsp[0].select_stmt); }
#line 3695 "bison_parser.cpp"
    break;

  case 87:
#line 717 "bison_parser.y"
                                                                       {
		(yyval.select_stmt) = (yyvsp[-2].select_stmt);
		if ((yyval.select_stmt)->setOperations == nullptr) {
			(yyval.select_stmt)->setOperations = new std::vector<SetOperation*>();
		}
		(yyval.select_stmt)->setOperations->push_back((yyvsp[-1].set_operator_t));
		(yyval.select_stmt)->setOperations->back()->nestedSelectStatement = (yyvsp[0].select_stmt);
	}
#line 3708 "bison_parser.cpp"
    break;

  case 88:
#line 728 "bison_parser.y"
                                        { (yyval.select_stmt) = (yyvsp[-1].select_stmt); }
#line 3714 "bison_parser.cpp"
    break;

  case 89:
#line 729 "bison_parser.y"
                                          { (yyval.select_stmt) = (yyvsp[-1].select_stmt); }
#line 3720 "bison_parser.cpp"
    break;

  case 90:
#line 733 "bison_parser.y"
                                                  {
			(yyval.select_stmt) = (yyvsp[-2].select_stmt);
			(yyval.select_stmt)->order = (yyvsp[-1].order_vec);

			// Limit could have been set by TOP.
			if ((yyvsp[0].limit) != nullptr) {
				delete (yyval.select_stmt)->limit;
				(yyval.select_stmt)->limit = (yyvsp[0].limit);
			}
		}
#line 3735 "bison_parser.cpp"
    break;

  case 91:
#line 743 "bison_parser.y"
                                                                                           {
			(yyval.select_stmt) = (yyvsp[-4].select_stmt);
			if ((yyval.select_stmt)->setOperations == nullptr) {
				(yyval.select_stmt)->setOperations = new std::vector<SetOperation*>();
			}
			(yyval.select_stmt)->setOperations->push_back((yyvsp[-3].set_operator_t));
			(yyval.select_stmt)->setOperations->back()->nestedSelectStatement = (yyvsp[-2].select_stmt);
			(yyval.select_stmt)->setOperations->back()->resultOrder = (yyvsp[-1].order_vec);
			(yyval.select_stmt)->setOperations->back()->resultLimit = (yyvsp[0].limit);
		}
#line 3750 "bison_parser.cpp"
    break;

  case 92:
#line 756 "bison_parser.y"
                                 {
		(yyval.set_operator_t) = (yyvsp[-1].set_operator_t);
		(yyval.set_operator_t)->isAll = (yyvsp[0].bval);
		}
#line 3759 "bison_parser.cpp"
    break;

  case 93:
#line 763 "bison_parser.y"
                      {
		(yyval.set_operator_t) = new SetOperation();
		(yyval.set_operator_t)->setType = SetType::kSetUnion;
		}
#line 3768 "bison_parser.cpp"
    break;

  case 94:
#line 767 "bison_parser.y"
                          {
		(yyval.set_operator_t) = new SetOperation();
		(yyval.set_operator_t)->setType = SetType::kSetIntersect;
	}
#line 3777 "bison_parser.cpp"
    break;

  case 95:
#line 771 "bison_parser.y"
                       {
		(yyval.set_operator_t) = new SetOperation();
		(yyval.set_operator_t)->setType = SetType::kSetExcept;
	}
#line 3786 "bison_parser.cpp"
    break;

  case 96:
#line 778 "bison_parser.y"
                    {
			(yyval.bval) = true;
		}
#line 3794 "bison_parser.cpp"
    break;

  case 97:
#line 781 "bison_parser.y"
                            {
		(yyval.bval) = false;
	}
#line 3802 "bison_parser.cpp"
    break;

  case 98:
#line 787 "bison_parser.y"
                                                                                            {
			(yyval.select_stmt) = new SelectStatement();
			(yyval.select_stmt)->limit = (yyvsp[-5].limit);
			(yyval.select_stmt)->selectDistinct = (yyvsp[-4].bval);
			(yyval.select_stmt)->selectList = (yyvsp[-3].expr_vec);
			(yyval.select_stmt)->fromTable = (yyvsp[-2].table);
			(yyval.select_stmt)->whereClause = (yyvsp[-1].expr);
			(yyval.select_stmt)->groupBy = (yyvsp[0].group_t);
		}
#line 3816 "bison_parser.cpp"
    break;

  case 99:
#line 799 "bison_parser.y"
                         { (yyval.bval) = true; }
#line 3822 "bison_parser.cpp"
    break;

  case 100:
#line 800 "bison_parser.y"
                            { (yyval.bval) = false; }
#line 3828 "bison_parser.cpp"
    break;

  case 102:
#line 808 "bison_parser.y"
                     { (yyval.table) = (yyvsp[0].table); }
#line 3834 "bison_parser.cpp"
    break;

  case 103:
#line 809 "bison_parser.y"
                     { (yyval.table) = nullptr; }
#line 3840 "bison_parser.cpp"
    break;

  case 104:
#line 813 "bison_parser.y"
                               { (yyval.table) = (yyvsp[0].table); }
#line 3846 "bison_parser.cpp"
    break;

  case 105:
#line 818 "bison_parser.y"
                           { (yyval.expr) = (yyvsp[0].expr); }
#line 3852 "bison_parser.cpp"
    break;

  case 106:
#line 819 "bison_parser.y"
                            { (yyval.expr) = nullptr; }
#line 3858 "bison_parser.cpp"
    break;

  case 107:
#line 823 "bison_parser.y"
                                              {
			(yyval.group_t) = new GroupByDescription();
			(yyval.group_t)->columns = (yyvsp[-1].expr_vec);
			(yyval.group_t)->having = (yyvsp[0].expr);
		}
#line 3868 "bison_parser.cpp"
    break;

  case 108:
#line 828 "bison_parser.y"
                            { (yyval.group_t) = nullptr; }
#line 3874 "bison_parser.cpp"
    break;

  case 109:
#line 832 "bison_parser.y"
                            { (yyval.expr) = (yyvsp[0].expr); }
#line 3880 "bison_parser.cpp"
    break;

  case 110:
#line 833 "bison_parser.y"
                            { (yyval.expr) = nullptr; }
#line 3886 "bison_parser.cpp"
    break;

  case 111:
#line 837 "bison_parser.y"
                                    { (yyval.order_vec) = (yyvsp[0].order_vec); }
#line 3892 "bison_parser.cpp"
    break;

  case 112:
#line 838 "bison_parser.y"
                            { (yyval.order_vec) = nullptr; }
#line 3898 "bison_parser.cpp"
    break;

  case 113:
#line 842 "bison_parser.y"
                           { (yyval.order_vec) = new std::vector<OrderDescription*>(); (yyval.order_vec)->push_back((yyvsp[0].order)); }
#line 3904 "bison_parser.cpp"
    break;

  case 114:
#line 843 "bison_parser.y"
                                          { (yyvsp[-2].order_vec)->push_back((yyvsp[0].order)); (yyval.order_vec) = (yyvsp[-2].order_vec); }
#line 3910 "bison_parser.cpp"
    break;

  case 115:
#line 847 "bison_parser.y"
                                    { (yyval.order) = new OrderDescription((yyvsp[0].order_type), (yyvsp[-1].expr)); }
#line 3916 "bison_parser.cpp"
    break;

  case 116:
#line 851 "bison_parser.y"
                    { (yyval.order_type) = kOrderAsc; }
#line 3922 "bison_parser.cpp"
    break;

  case 117:
#line 852 "bison_parser.y"
                     { (yyval.order_type) = kOrderDesc; }
#line 3928 "bison_parser.cpp"
    break;

  case 118:
#line 853 "bison_parser.y"
                            { (yyval.order_type) = kOrderAsc; }
#line 3934 "bison_parser.cpp"
    break;

  case 119:
#line 859 "bison_parser.y"
                                { (yyval.limit) = new LimitDescription((yyvsp[0].expr), nullptr); }
#line 3940 "bison_parser.cpp"
    break;

  case 120:
#line 860 "bison_parser.y"
                            { (yyval.limit) = nullptr; }
#line 3946 "bison_parser.cpp"
    break;

  case 121:
#line 864 "bison_parser.y"
                           { (yyval.limit) = new LimitDescription((yyvsp[0].expr), nullptr); }
#line 3952 "bison_parser.cpp"
    break;

  case 122:
#line 865 "bison_parser.y"
                            { (yyval.limit) = new LimitDescription(nullptr, (yyvsp[0].expr)); }
#line 3958 "bison_parser.cpp"
    break;

  case 123:
#line 866 "bison_parser.y"
                                       { (yyval.limit) = new LimitDescription((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 3964 "bison_parser.cpp"
    break;

  case 124:
#line 867 "bison_parser.y"
                          { (yyval.limit) = new LimitDescription(nullptr, nullptr); }
#line 3970 "bison_parser.cpp"
    break;

  case 125:
#line 868 "bison_parser.y"
                                      { (yyval.limit) = new LimitDescription(nullptr, (yyvsp[0].expr)); }
#line 3976 "bison_parser.cpp"
    break;

  case 126:
#line 869 "bison_parser.y"
                            { (yyval.limit) = nullptr; }
#line 3982 "bison_parser.cpp"
    break;

  case 127:
#line 876 "bison_parser.y"
                           { (yyval.expr_vec) = new std::vector<Expr*>(); (yyval.expr_vec)->push_back((yyvsp[0].expr)); }
#line 3988 "bison_parser.cpp"
    break;

  case 128:
#line 877 "bison_parser.y"
                                         { (yyvsp[-2].expr_vec)->push_back((yyvsp[0].expr)); (yyval.expr_vec) = (yyvsp[-2].expr_vec); }
#line 3994 "bison_parser.cpp"
    break;

  case 129:
#line 881 "bison_parser.y"
                             { (yyval.expr_vec) = (yyvsp[0].expr_vec); }
#line 4000 "bison_parser.cpp"
    break;

  case 130:
#line 882 "bison_parser.y"
                            { (yyval.expr_vec) = nullptr; }
#line 4006 "bison_parser.cpp"
    break;

  case 131:
#line 886 "bison_parser.y"
                        { (yyval.expr_vec) = new std::vector<Expr*>(); (yyval.expr_vec)->push_back((yyvsp[0].expr)); }
#line 4012 "bison_parser.cpp"
    break;

  case 132:
#line 887 "bison_parser.y"
                                         { (yyvsp[-2].expr_vec)->push_back((yyvsp[0].expr)); (yyval.expr_vec) = (yyvsp[-2].expr_vec); }
#line 4018 "bison_parser.cpp"
    break;

  case 133:
#line 891 "bison_parser.y"
                               {
			(yyval.expr) = (yyvsp[-1].expr);
			if ((yyvsp[0].alias_t)) {
				(yyval.expr)->alias = strdup((yyvsp[0].alias_t)->name);
				delete (yyvsp[0].alias_t);
			}
		}
#line 4030 "bison_parser.cpp"
    break;

  case 139:
#line 909 "bison_parser.y"
                             { (yyval.expr) = (yyvsp[-1].expr); }
#line 4036 "bison_parser.cpp"
    break;

  case 149:
#line 919 "bison_parser.y"
                                        { (yyval.expr) = Expr::makeSelect((yyvsp[-1].select_stmt)); }
#line 4042 "bison_parser.cpp"
    break;

  case 159:
#line 935 "bison_parser.y"
                            { (yyval.expr) = Expr::makeOpUnary(kOpUnaryMinus, (yyvsp[0].expr)); }
#line 4048 "bison_parser.cpp"
    break;

  case 160:
#line 936 "bison_parser.y"
                            { (yyval.expr) = Expr::makeOpUnary(kOpNot, (yyvsp[0].expr)); }
#line 4054 "bison_parser.cpp"
    break;

  case 161:
#line 937 "bison_parser.y"
                               { (yyval.expr) = Expr::makeOpUnary(kOpIsNull, (yyvsp[-1].expr)); }
#line 4060 "bison_parser.cpp"
    break;

  case 162:
#line 938 "bison_parser.y"
                                { (yyval.expr) = Expr::makeOpUnary(kOpIsNull, (yyvsp[-2].expr)); }
#line 4066 "bison_parser.cpp"
    break;

  case 163:
#line 939 "bison_parser.y"
                                    { (yyval.expr) = Expr::makeOpUnary(kOpNot, Expr::makeOpUnary(kOpIsNull, (yyvsp[-3].expr))); }
#line 4072 "bison_parser.cpp"
    break;

  case 165:
#line 944 "bison_parser.y"
                                                { (yyval.expr) = Expr::makeOpBinary((yyvsp[-2].expr), kOpMinus, (yyvsp[0].expr)); }
#line 4078 "bison_parser.cpp"
    break;

  case 166:
#line 945 "bison_parser.y"
                                                { (yyval.expr) = Expr::makeOpBinary((yyvsp[-2].expr), kOpPlus, (yyvsp[0].expr)); }
#line 4084 "bison_parser.cpp"
    break;

  case 167:
#line 946 "bison_parser.y"
                                                { (yyval.expr) = Expr::makeOpBinary((yyvsp[-2].expr), kOpSlash, (yyvsp[0].expr)); }
#line 4090 "bison_parser.cpp"
    break;

  case 168:
#line 947 "bison_parser.y"
                                                { (yyval.expr) = Expr::makeOpBinary((yyvsp[-2].expr), kOpAsterisk, (yyvsp[0].expr)); }
#line 4096 "bison_parser.cpp"
    break;

  case 169:
#line 948 "bison_parser.y"
                                                { (yyval.expr) = Expr::makeOpBinary((yyvsp[-2].expr), kOpPercentage, (yyvsp[0].expr)); }
#line 4102 "bison_parser.cpp"
    break;

  case 170:
#line 949 "bison_parser.y"
                                                { (yyval.expr) = Expr::makeOpBinary((yyvsp[-2].expr), kOpCaret, (yyvsp[0].expr)); }
#line 4108 "bison_parser.cpp"
    break;

  case 171:
#line 950 "bison_parser.y"
                                                { (yyval.expr) = Expr::makeOpBinary((yyvsp[-2].expr), kOpLike, (yyvsp[0].expr)); }
#line 4114 "bison_parser.cpp"
    break;

  case 172:
#line 951 "bison_parser.y"
                                                { (yyval.expr) = Expr::makeOpBinary((yyvsp[-3].expr), kOpNotLike, (yyvsp[0].expr)); }
#line 4120 "bison_parser.cpp"
    break;

  case 173:
#line 952 "bison_parser.y"
                                                { (yyval.expr) = Expr::makeOpBinary((yyvsp[-2].expr), kOpILike, (yyvsp[0].expr)); }
#line 4126 "bison_parser.cpp"
    break;

  case 174:
#line 953 "bison_parser.y"
                                                { (yyval.expr) = Expr::makeOpBinary((yyvsp[-2].expr), kOpConcat, (yyvsp[0].expr)); }
#line 4132 "bison_parser.cpp"
    break;

  case 175:
#line 957 "bison_parser.y"
                                { (yyval.expr) = Expr::makeOpBinary((yyvsp[-2].expr), kOpAnd, (yyvsp[0].expr)); }
#line 4138 "bison_parser.cpp"
    break;

  case 176:
#line 958 "bison_parser.y"
                                { (yyval.expr) = Expr::makeOpBinary((yyvsp[-2].expr), kOpOr, (yyvsp[0].expr)); }
#line 4144 "bison_parser.cpp"
    break;

  case 177:
#line 959 "bison_parser.y"
                                { (yyval.expr) = Expr::makeOpBinary((yyvsp[-2].expr), kOpXor, (yyvsp[0].expr)); }
#line 4150 "bison_parser.cpp"
    break;

  case 178:
#line 963 "bison_parser.y"
                                                                { (yyval.expr) = Expr::makeInOperator((yyvsp[-4].expr), (yyvsp[-1].expr_vec)); }
#line 4156 "bison_parser.cpp"
    break;

  case 179:
#line 964 "bison_parser.y"
                                                                { (yyval.expr) = Expr::makeOpUnary(kOpNot, Expr::makeInOperator((yyvsp[-5].expr), (yyvsp[-1].expr_vec))); }
#line 4162 "bison_parser.cpp"
    break;

  case 180:
#line 965 "bison_parser.y"
                                                                { (yyval.expr) = Expr::makeInOperator((yyvsp[-4].expr), (yyvsp[-1].select_stmt)); }
#line 4168 "bison_parser.cpp"
    break;

  case 181:
#line 966 "bison_parser.y"
                                                        { (yyval.expr) = Expr::makeOpUnary(kOpNot, Expr::makeInOperator((yyvsp[-5].expr), (yyvsp[-1].select_stmt))); }
#line 4174 "bison_parser.cpp"
    break;

  case 182:
#line 972 "bison_parser.y"
                                                        { (yyval.expr) = Expr::makeCase((yyvsp[-2].expr), (yyvsp[-1].expr), nullptr); }
#line 4180 "bison_parser.cpp"
    break;

  case 183:
#line 973 "bison_parser.y"
                                                        { (yyval.expr) = Expr::makeCase((yyvsp[-4].expr), (yyvsp[-3].expr), (yyvsp[-1].expr)); }
#line 4186 "bison_parser.cpp"
    break;

  case 184:
#line 974 "bison_parser.y"
                                                        { (yyval.expr) = Expr::makeCase(nullptr, (yyvsp[-1].expr), nullptr); }
#line 4192 "bison_parser.cpp"
    break;

  case 185:
#line 975 "bison_parser.y"
                                                        { (yyval.expr) = Expr::makeCase(nullptr, (yyvsp[-3].expr), (yyvsp[-1].expr)); }
#line 4198 "bison_parser.cpp"
    break;

  case 186:
#line 979 "bison_parser.y"
                                                 { (yyval.expr) = Expr::makeCaseList(Expr::makeCaseListElement((yyvsp[-2].expr), (yyvsp[0].expr))); }
#line 4204 "bison_parser.cpp"
    break;

  case 187:
#line 980 "bison_parser.y"
                                                 { (yyval.expr) = Expr::caseListAppend((yyvsp[-4].expr), Expr::makeCaseListElement((yyvsp[-2].expr), (yyvsp[0].expr))); }
#line 4210 "bison_parser.cpp"
    break;

  case 188:
#line 984 "bison_parser.y"
                                               { (yyval.expr) = Expr::makeExists((yyvsp[-1].select_stmt)); }
#line 4216 "bison_parser.cpp"
    break;

  case 189:
#line 985 "bison_parser.y"
                                                   { (yyval.expr) = Expr::makeOpUnary(kOpNot, Expr::makeExists((yyvsp[-1].select_stmt))); }
#line 4222 "bison_parser.cpp"
    break;

  case 190:
#line 989 "bison_parser.y"
                                                { (yyval.expr) = Expr::makeOpBinary((yyvsp[-2].expr), kOpEquals, (yyvsp[0].expr)); }
#line 4228 "bison_parser.cpp"
    break;

  case 191:
#line 990 "bison_parser.y"
                                                { (yyval.expr) = Expr::makeOpBinary((yyvsp[-2].expr), kOpEquals, (yyvsp[0].expr)); }
#line 4234 "bison_parser.cpp"
    break;

  case 192:
#line 991 "bison_parser.y"
                                                { (yyval.expr) = Expr::makeOpBinary((yyvsp[-2].expr), kOpNotEquals, (yyvsp[0].expr)); }
#line 4240 "bison_parser.cpp"
    break;

  case 193:
#line 992 "bison_parser.y"
                                                { (yyval.expr) = Expr::makeOpBinary((yyvsp[-2].expr), kOpLess, (yyvsp[0].expr)); }
#line 4246 "bison_parser.cpp"
    break;

  case 194:
#line 993 "bison_parser.y"
                                                { (yyval.expr) = Expr::makeOpBinary((yyvsp[-2].expr), kOpGreater, (yyvsp[0].expr)); }
#line 4252 "bison_parser.cpp"
    break;

  case 195:
#line 994 "bison_parser.y"
                                                { (yyval.expr) = Expr::makeOpBinary((yyvsp[-2].expr), kOpLessEq, (yyvsp[0].expr)); }
#line 4258 "bison_parser.cpp"
    break;

  case 196:
#line 995 "bison_parser.y"
                                                { (yyval.expr) = Expr::makeOpBinary((yyvsp[-2].expr), kOpGreaterEq, (yyvsp[0].expr)); }
#line 4264 "bison_parser.cpp"
    break;

  case 197:
#line 999 "bison_parser.y"
                                  { (yyval.expr) = Expr::makeFunctionRef((yyvsp[-2].sval), new std::vector<Expr*>(), false); }
#line 4270 "bison_parser.cpp"
    break;

  case 198:
#line 1000 "bison_parser.y"
                                                         { (yyval.expr) = Expr::makeFunctionRef((yyvsp[-4].sval), (yyvsp[-1].expr_vec), (yyvsp[-2].bval)); }
#line 4276 "bison_parser.cpp"
    break;

  case 199:
#line 1004 "bison_parser.y"
                                                     { (yyval.expr) = Expr::makeExtract((yyvsp[-3].datetime_field), (yyvsp[-1].expr)); }
#line 4282 "bison_parser.cpp"
    break;

  case 200:
#line 1008 "bison_parser.y"
                                            { (yyval.expr) = Expr::makeCast((yyvsp[-3].expr), (yyvsp[-1].column_type_t)); }
#line 4288 "bison_parser.cpp"
    break;

  case 201:
#line 1012 "bison_parser.y"
               { (yyval.datetime_field) = kDatetimeSecond; }
#line 4294 "bison_parser.cpp"
    break;

  case 202:
#line 1013 "bison_parser.y"
               { (yyval.datetime_field) = kDatetimeMinute; }
#line 4300 "bison_parser.cpp"
    break;

  case 203:
#line 1014 "bison_parser.y"
             { (yyval.datetime_field) = kDatetimeHour; }
#line 4306 "bison_parser.cpp"
    break;

  case 204:
#line 1015 "bison_parser.y"
            { (yyval.datetime_field) = kDatetimeDay; }
#line 4312 "bison_parser.cpp"
    break;

  case 205:
#line 1016 "bison_parser.y"
              { (yyval.datetime_field) = kDatetimeMonth; }
#line 4318 "bison_parser.cpp"
    break;

  case 206:
#line 1017 "bison_parser.y"
             { (yyval.datetime_field) = kDatetimeYear; }
#line 4324 "bison_parser.cpp"
    break;

  case 207:
#line 1021 "bison_parser.y"
                                        { (yyval.expr) = Expr::makeArray((yyvsp[-1].expr_vec)); }
#line 4330 "bison_parser.cpp"
    break;

  case 208:
#line 1025 "bison_parser.y"
                                        { (yyval.expr) = Expr::makePoint((yyvsp[-1].expr_vec)); }
#line 4336 "bison_parser.cpp"
    break;

  case 209:
#line 1029 "bison_parser.y"
                                                           { (yyval.expr) = Expr::makeSrid((yyvsp[-3].expr),(yyvsp[-1].expr)->ival); }
#line 4342 "bison_parser.cpp"
    break;

  case 210:
#line 1033 "bison_parser.y"
                                          { (yyval.expr) = Expr::makeCount((yyvsp[-1].expr)); }
#line 4348 "bison_parser.cpp"
    break;

  case 211:
#line 1037 "bison_parser.y"
                                        { (yyval.expr) = Expr::makeMin((yyvsp[-1].expr)); }
#line 4354 "bison_parser.cpp"
    break;

  case 212:
#line 1041 "bison_parser.y"
                                        { (yyval.expr) = Expr::makeMax((yyvsp[-1].expr)); }
#line 4360 "bison_parser.cpp"
    break;

  case 213:
#line 1045 "bison_parser.y"
                                        { (yyval.expr) = Expr::makeSum((yyvsp[-1].expr)); }
#line 4366 "bison_parser.cpp"
    break;

  case 214:
#line 1049 "bison_parser.y"
                                        { (yyval.expr) = Expr::makeAvg((yyvsp[-1].expr)); }
#line 4372 "bison_parser.cpp"
    break;

  case 215:
#line 1053 "bison_parser.y"
                                                                       { (yyval.expr) = Expr::makeDistanceSphere((yyvsp[-3].expr),(yyvsp[-1].expr)); }
#line 4378 "bison_parser.cpp"
    break;

  case 216:
#line 1057 "bison_parser.y"
                                            { (yyval.expr) = Expr::makeArrayIndex((yyvsp[-3].expr), (yyvsp[-1].expr)->ival); }
#line 4384 "bison_parser.cpp"
    break;

  case 217:
#line 1061 "bison_parser.y"
                                                    { (yyval.expr) = Expr::makeBetween((yyvsp[-4].expr), (yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 4390 "bison_parser.cpp"
    break;

  case 218:
#line 1062 "bison_parser.y"
                                                        { (yyval.expr) = Expr::makeOpUnary(kOpNot, Expr::makeBetween((yyvsp[-5].expr), (yyvsp[-2].expr), (yyvsp[0].expr))); }
#line 4396 "bison_parser.cpp"
    break;

  case 219:
#line 1066 "bison_parser.y"
                           { (yyval.expr) = Expr::makeColumnRef((yyvsp[0].sval)); }
#line 4402 "bison_parser.cpp"
    break;

  case 220:
#line 1067 "bison_parser.y"
                                          { (yyval.expr) = Expr::makeColumnRef((yyvsp[-2].sval), (yyvsp[0].sval)); }
#line 4408 "bison_parser.cpp"
    break;

  case 221:
#line 1068 "bison_parser.y"
                    { (yyval.expr) = Expr::makeStar(); }
#line 4414 "bison_parser.cpp"
    break;

  case 222:
#line 1069 "bison_parser.y"
                                   { (yyval.expr) = Expr::makeStar((yyvsp[-2].sval)); }
#line 4420 "bison_parser.cpp"
    break;

  case 228:
#line 1081 "bison_parser.y"
                       { (yyval.expr) = Expr::makeLiteral((yyvsp[0].sval)); }
#line 4426 "bison_parser.cpp"
    break;

  case 229:
#line 1085 "bison_parser.y"
                     { (yyval.expr) = Expr::makeLiteral(true); }
#line 4432 "bison_parser.cpp"
    break;

  case 230:
#line 1086 "bison_parser.y"
                      { (yyval.expr) = Expr::makeLiteral(false); }
#line 4438 "bison_parser.cpp"
    break;

  case 232:
#line 1091 "bison_parser.y"
                         { (yyval.expr) = Expr::makeLiteral((yyvsp[0].fval)); }
#line 4444 "bison_parser.cpp"
    break;

  case 233:
#line 1095 "bison_parser.y"
                       { (yyval.expr) = Expr::makeLiteral((yyvsp[0].ival)); }
#line 4450 "bison_parser.cpp"
    break;

  case 234:
#line 1099 "bison_parser.y"
                     { (yyval.expr) = Expr::makeNullLiteral(); }
#line 4456 "bison_parser.cpp"
    break;

  case 235:
#line 1103 "bison_parser.y"
                    {
			(yyval.expr) = Expr::makeParameter(yylloc.total_column);
			(yyval.expr)->ival2 = yyloc.param_list.size();
			yyloc.param_list.push_back((yyval.expr));
		}
#line 4466 "bison_parser.cpp"
    break;

  case 237:
#line 1116 "bison_parser.y"
                                                         {
			(yyvsp[-2].table_vec)->push_back((yyvsp[0].table));
			auto tbl = new TableRef(kTableCrossProduct);
			tbl->list = (yyvsp[-2].table_vec);
			(yyval.table) = tbl;
		}
#line 4477 "bison_parser.cpp"
    break;

  case 241:
#line 1132 "bison_parser.y"
                                                         {
			auto tbl = new TableRef(kTableSelect);
			tbl->select = (yyvsp[-2].select_stmt);
			tbl->alias = (yyvsp[0].alias_t);
			(yyval.table) = tbl;
		}
#line 4488 "bison_parser.cpp"
    break;

  case 242:
#line 1141 "bison_parser.y"
                                 { (yyval.table_vec) = new std::vector<TableRef*>(); (yyval.table_vec)->push_back((yyvsp[0].table)); }
#line 4494 "bison_parser.cpp"
    break;

  case 243:
#line 1142 "bison_parser.y"
                                                         { (yyvsp[-2].table_vec)->push_back((yyvsp[0].table)); (yyval.table_vec) = (yyvsp[-2].table_vec); }
#line 4500 "bison_parser.cpp"
    break;

  case 244:
#line 1147 "bison_parser.y"
                                           {
			auto tbl = new TableRef(kTableName);
			tbl->schema = (yyvsp[-1].table_name).schema;
			tbl->name = (yyvsp[-1].table_name).name;
			tbl->alias = (yyvsp[0].alias_t);
			(yyval.table) = tbl;
		}
#line 4512 "bison_parser.cpp"
    break;

  case 245:
#line 1158 "bison_parser.y"
                           {
			(yyval.table) = new TableRef(kTableName);
			(yyval.table)->schema = (yyvsp[0].table_name).schema;
			(yyval.table)->name = (yyvsp[0].table_name).name;
		}
#line 4522 "bison_parser.cpp"
    break;

  case 246:
#line 1167 "bison_parser.y"
                                          { (yyval.table_name).schema = nullptr; (yyval.table_name).name = (yyvsp[0].sval);}
#line 4528 "bison_parser.cpp"
    break;

  case 247:
#line 1168 "bison_parser.y"
                                          { (yyval.table_name).schema = (yyvsp[-2].sval); (yyval.table_name).name = (yyvsp[0].sval); }
#line 4534 "bison_parser.cpp"
    break;

  case 249:
#line 1174 "bison_parser.y"
                                                      { (yyval.alias_t) = new Alias((yyvsp[-3].sval), (yyvsp[-1].str_vec)); }
#line 4540 "bison_parser.cpp"
    break;

  case 251:
#line 1180 "bison_parser.y"
                            { (yyval.alias_t) = nullptr; }
#line 4546 "bison_parser.cpp"
    break;

  case 252:
#line 1185 "bison_parser.y"
                              { (yyval.alias_t) = new Alias((yyvsp[0].sval)); }
#line 4552 "bison_parser.cpp"
    break;

  case 253:
#line 1186 "bison_parser.y"
                           { (yyval.alias_t) = new Alias((yyvsp[0].sval)); }
#line 4558 "bison_parser.cpp"
    break;

  case 255:
#line 1192 "bison_parser.y"
                            { (yyval.alias_t) = nullptr; }
#line 4564 "bison_parser.cpp"
    break;

  case 257:
#line 1202 "bison_parser.y"
                            { (yyval.with_description_vec) = nullptr; }
#line 4570 "bison_parser.cpp"
    break;

  case 258:
#line 1206 "bison_parser.y"
                                           { (yyval.with_description_vec) = (yyvsp[0].with_description_vec); }
#line 4576 "bison_parser.cpp"
    break;

  case 259:
#line 1210 "bison_parser.y"
                                 {
			(yyval.with_description_vec) = new std::vector<WithDescription*>();
			(yyval.with_description_vec)->push_back((yyvsp[0].with_description_t));
		}
#line 4585 "bison_parser.cpp"
    break;

  case 260:
#line 1214 "bison_parser.y"
                                                           {
			(yyvsp[-2].with_description_vec)->push_back((yyvsp[0].with_description_t));
                        (yyval.with_description_vec) = (yyvsp[-2].with_description_vec);
		}
#line 4594 "bison_parser.cpp"
    break;

  case 261:
#line 1221 "bison_parser.y"
                                                {
			(yyval.with_description_t) = new WithDescription();
			(yyval.with_description_t)->alias = (yyvsp[-2].sval);
			(yyval.with_description_t)->select = (yyvsp[0].select_stmt);
		}
#line 4604 "bison_parser.cpp"
    break;

  case 262:
#line 1235 "bison_parser.y"
                {
			(yyval.table) = new TableRef(kTableJoin);
			(yyval.table)->join = new JoinDefinition();
			(yyval.table)->join->type = kJoinNatural;
			(yyval.table)->join->left = (yyvsp[-3].table);
			(yyval.table)->join->right = (yyvsp[0].table);
		}
#line 4616 "bison_parser.cpp"
    break;

  case 263:
#line 1243 "bison_parser.y"
                {
			(yyval.table) = new TableRef(kTableJoin);
			(yyval.table)->join = new JoinDefinition();
			(yyval.table)->join->type = (JoinType) (yyvsp[-4].uval);
			(yyval.table)->join->left = (yyvsp[-5].table);
			(yyval.table)->join->right = (yyvsp[-2].table);
			(yyval.table)->join->condition = (yyvsp[0].expr);
		}
#line 4629 "bison_parser.cpp"
    break;

  case 264:
#line 1253 "bison_parser.y"
                {
			(yyval.table) = new TableRef(kTableJoin);
			(yyval.table)->join = new JoinDefinition();
			(yyval.table)->join->type = (JoinType) (yyvsp[-6].uval);
			(yyval.table)->join->left = (yyvsp[-7].table);
			(yyval.table)->join->right = (yyvsp[-4].table);
			auto left_col = Expr::makeColumnRef(strdup((yyvsp[-1].expr)->name));
			if ((yyvsp[-1].expr)->alias != nullptr) left_col->alias = strdup((yyvsp[-1].expr)->alias);
			if ((yyvsp[-7].table)->getName() != nullptr) left_col->table = strdup((yyvsp[-7].table)->getName());
			auto right_col = Expr::makeColumnRef(strdup((yyvsp[-1].expr)->name));
			if ((yyvsp[-1].expr)->alias != nullptr) right_col->alias = strdup((yyvsp[-1].expr)->alias);
			if ((yyvsp[-4].table)->getName() != nullptr) right_col->table = strdup((yyvsp[-4].table)->getName());
			(yyval.table)->join->condition = Expr::makeOpBinary(left_col, kOpEquals, right_col);
			delete (yyvsp[-1].expr);
		}
#line 4649 "bison_parser.cpp"
    break;

  case 265:
#line 1271 "bison_parser.y"
                                { (yyval.uval) = kJoinInner; }
#line 4655 "bison_parser.cpp"
    break;

  case 266:
#line 1272 "bison_parser.y"
                                { (yyval.uval) = kJoinLeft; }
#line 4661 "bison_parser.cpp"
    break;

  case 267:
#line 1273 "bison_parser.y"
                                { (yyval.uval) = kJoinLeft; }
#line 4667 "bison_parser.cpp"
    break;

  case 268:
#line 1274 "bison_parser.y"
                                { (yyval.uval) = kJoinRight; }
#line 4673 "bison_parser.cpp"
    break;

  case 269:
#line 1275 "bison_parser.y"
                                { (yyval.uval) = kJoinRight; }
#line 4679 "bison_parser.cpp"
    break;

  case 270:
#line 1276 "bison_parser.y"
                                { (yyval.uval) = kJoinFull; }
#line 4685 "bison_parser.cpp"
    break;

  case 271:
#line 1277 "bison_parser.y"
                                { (yyval.uval) = kJoinFull; }
#line 4691 "bison_parser.cpp"
    break;

  case 272:
#line 1278 "bison_parser.y"
                                { (yyval.uval) = kJoinFull; }
#line 4697 "bison_parser.cpp"
    break;

  case 273:
#line 1279 "bison_parser.y"
                                { (yyval.uval) = kJoinCross; }
#line 4703 "bison_parser.cpp"
    break;

  case 274:
#line 1280 "bison_parser.y"
                                        { (yyval.uval) = kJoinInner; }
#line 4709 "bison_parser.cpp"
    break;

  case 278:
#line 1300 "bison_parser.y"
                           { (yyval.str_vec) = new std::vector<char*>(); (yyval.str_vec)->push_back((yyvsp[0].sval)); }
#line 4715 "bison_parser.cpp"
    break;

  case 279:
#line 1301 "bison_parser.y"
                                               { (yyvsp[-2].str_vec)->push_back((yyvsp[0].sval)); (yyval.str_vec) = (yyvsp[-2].str_vec); }
#line 4721 "bison_parser.cpp"
    break;


#line 4725 "bison_parser.cpp"

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
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

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
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (&yylloc, result, scanner, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (&yylloc, result, scanner, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
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
                      yytoken, &yylval, &yylloc, result, scanner);
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

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
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

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, yylsp, result, scanner);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the lookahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, yyerror_range, 2);
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


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (&yylloc, result, scanner, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc, result, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[+*yyssp], yyvsp, yylsp, result, scanner);
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
  return yyresult;
}
#line 1304 "bison_parser.y"

/*********************************
 ** Section 4: Additional C code
 *********************************/

/* empty */
