#pragma once

#include "SQLStatement.hpp"

namespace zsql {

// Represents SQL Execute statements.
// Example: "EXECUTE ins_prep(100, "test", 2.3);"
struct ExecuteStatement : SQLStatement {
    ExecuteStatement();
    ~ExecuteStatement() override;

    char* name;
    std::vector<Expr*>* parameters;
};

}