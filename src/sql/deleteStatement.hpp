#pragma once

#include "SQLStatement.hpp"

// Note: Implementations of constructors and destructors can be found in statements.cpp.
namespace zsql {

// Represents SQL Delete statements.
// Example: "DELETE FROM students WHERE grade > 3.0"
// Note: if (expr == nullptr) => delete all rows (truncate)
struct DeleteStatement : SQLStatement {
    DeleteStatement();
    ~DeleteStatement() override;

    char* schema;
    char* tableName;
    Expr* expr;
};

}