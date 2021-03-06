#pragma once

#include "SQLStatement.hpp"
#include "selectStatement.hpp"

namespace zsql {
enum InsertType {
    kInsertValues,
    kInsertSelect
};

// Represents SQL Insert statements.
// Example: "INSERT INTO students VALUES ('Max', 1112233, 'Musterhausen', 2.3)"
struct InsertStatement : SQLStatement {
    explicit InsertStatement(InsertType type);
    ~InsertStatement() override;

    InsertType type;
    char* schema;
    char* tableName;
    std::vector<char*>* columns;
    std::vector<Expr*>* values;
    SelectStatement* select;
};

}
