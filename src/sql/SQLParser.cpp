#include <string>

#include "SQLParser.hpp"
#include "SQLParserResult.hpp"
#include "bison_parser.hpp"
#include "flex_lexer.hpp"
#include "../common/log.hpp"

namespace zsql {

 bool zsql::SQLParser::parse(const std::string &sql, SQLParserResult *result) {
     yyscan_t scanner;
     YY_BUFFER_STATE state;

     if (zsql_lex_init(&scanner)) {
         // Couldn't initialize the lexer.
         Log::error("SQLParser: Error when initializing lexer!\n");
         return false;
     }
     const char* text = sql.c_str();
     state = zsql__scan_string(text, scanner);

     // Parse the tokens.
     // If parsing fails, the result will contain an error object.
     int ret = zsql_parse(result, scanner);
     bool success = (ret == 0);
     result->setIsValid(success);

     zsql__delete_buffer(state, scanner);
     zsql_lex_destroy(scanner);

     return true;
 }

 bool zsql::SQLParser::tokenize(const std::string &sql, std::vector <int16_t> *tokens) {
     // Initialize the scanner.
     yyscan_t scanner;
     if (zsql_lex_init(&scanner)) {
         Log::error("SQLParser: Error when initializing lexer!\n");
         return false;
     }

     YY_BUFFER_STATE state;
     state = zsql__scan_string(sql.c_str(), scanner);

     YYSTYPE yylval;
     YYLTYPE yylloc;

     // Step through the string until EOF is read.
     // Note: hsql_lex returns int, but we know that its range is within 16 bit.
     int16_t token = zsql_lex(&yylval, &yylloc, scanner);
     while (token != 0) {
         tokens->push_back(token);
         token = zsql_lex(&yylval, &yylloc, scanner);

         if (token == SQL_IDENTIFIER || token == SQL_STRING) {
             free(yylval.sval);
         }
     }

     zsql__delete_buffer(state, scanner);
     zsql_lex_destroy(scanner);
     return true;
 }

}