#pragma once

#include "SQLStatement.hpp"

// Note: Implementations of constructors and destructors can be found in statements.cpp.
namespace zsql {

enum ShowType {
    kShowColumns,
    kShowTables
};

// Represents SQL SHOW statements.
// Example "SHOW TABLES;"
struct ShowStatement : SQLStatement {

    explicit ShowStatement(ShowType type);
    ~ShowStatement() override;

    ShowType type;
    char* schema;
    char* name;
};

}