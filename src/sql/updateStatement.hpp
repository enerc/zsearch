#pragma once
#include "SQLStatement.hpp"

namespace zsql {

// Represents "column = value" expressions.
struct UpdateClause {
    char* column;
    Expr* value;
};

// Represents SQL Update statements.
struct UpdateStatement : SQLStatement {
    UpdateStatement();
    ~UpdateStatement() override;

    // TODO: switch to char* instead of TableRef
    TableRef* table;
    std::vector<UpdateClause*>* updates;
    Expr* where;
};

}