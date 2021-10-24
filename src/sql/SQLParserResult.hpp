#pragma once

#include "SQLStatement.hpp"

namespace zsql {
// Represents the result of the SQLParser.
// If parsing was successful it contains a list of SQLStatement.
class SQLParserResult {
public:
    // Initialize with empty statement list.
    SQLParserResult();

    // Initialize with a single statement.
    // Takes ownership of the statement.
    explicit SQLParserResult(SQLStatement* stmt);

    // Move constructor.
    SQLParserResult(SQLParserResult&& moved) noexcept ;

    // Deletes all statements in the result.
    virtual ~SQLParserResult();

    // Set whether parsing was successful.
    void setIsValid(bool isValid);

    // Returns true if parsing was successful.
    [[nodiscard]] bool isValid() const;

    // Returns the number of statements in the result.
    [[nodiscard]] size_t size() const;

    // Set the details of the error, if available.
    // Takes ownership of errorMsg.
    void setErrorDetails(char* errorMsg, int errorLine, int errorColumn);

    // Returns the error message, if an error occurred.
    [[nodiscard]] const char* errorMsg() const;

    // Returns the line number of the occurrance of the error in the query.
    [[nodiscard]] int errorLine() const;

    // Returns the column number of the occurrance of the error in the query.
    [[nodiscard]] int errorColumn() const;

    // Adds a statement to the result list of statements.
    // SQLParserResult takes ownership of the statement.
    void addStatement(SQLStatement* stmt);

    // Gets the SQL statement with the given index.
    [[nodiscard]] const SQLStatement* getStatement(size_t index) const;

    // Gets the non const SQL statement with the given index.
    SQLStatement* getMutableStatement(size_t index);

    // Get the list of all statements.
    [[nodiscard]] const std::vector<SQLStatement*>& getStatements() const;

    // Returns a copy of the list of all statements in this result.
    // Removes them from this result.
    std::vector<SQLStatement*> releaseStatements();

    // Deletes all statements and other data within the result.
    void reset();

    // Does NOT take ownership.
    void addParameter(Expr* parameter);

    const std::vector<Expr*>& parameters();

    std::string asString() const;
private:
    // List of statements within the result.
    std::vector<SQLStatement*> statements_;

    // Flag indicating the parsing was successful.
    bool isValid_;

    // Error message, if an error occurred.
    char* errorMsg_;

    // Line number of the occurrance of the error in the query.
    int errorLine_{};

    // Column number of the occurrance of the error in the query.
    int errorColumn_{};

    // Does NOT have ownership.
    std::vector<Expr*> parameters_;
};

} // namespace hsql