#pragma once
#include <vector>

#include "SQLParserResult.hpp"
#include "statements.hpp"

namespace zsql {

// Static methods used to parse SQL strings.
class SQLParser {
public:
    // Parses a given constant character SQL string into the result object.
    // Returns true if the lexer and parser could run without internal errors.
    // This does NOT mean that the SQL string was valid SQL. To check that
    // you need to check result->isValid();
    static bool parse(const std::string& sql, SQLParserResult* result);

    // Run tokenization on the given string and store the tokens in the output vector.
    static bool tokenize(const std::string& sql, std::vector<int16_t>* tokens);

private:
};


} // namespace hsql