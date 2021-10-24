/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

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

#line 80 "bison_parser.hpp"

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

#line 305 "bison_parser.hpp"

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
