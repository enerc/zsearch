#pragma once

#include "SQLStatement.hpp"

// Note: Implementations of constructors and destructors can be found in statements.cpp.
namespace zsql {

enum DropType {
    kDropTable,
    kDropSchema,
    kDropIndex,
    kDropView,
    kDropPreparedStatement
};

// Represents SQL Delete statements.
// Example "DROP TABLE students;"
struct DropStatement : SQLStatement {

    explicit DropStatement(DropType type);
    ~DropStatement() override;

    DropType type;
    bool ifExists = true;
    char* schema;
    char* name;
};

}
