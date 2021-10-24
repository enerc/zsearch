#pragma once

#include "SQLStatement.hpp"
#include "columnType.hpp"
#include <ostream>

// Note: Implementations of constructors and destructors can be found in statements.cpp.
namespace zsql {
struct SelectStatement;

// Represents definition of a table column
struct ColumnDefinition {
    ColumnDefinition(char* name, ColumnType type, bool nullable);
    virtual~ColumnDefinition();

    char* name;
    ColumnType type;
    bool nullable;
};

enum CreateType {
    kCreateTable,
    kCreateTableFromTbl, // Hyrise file format
    kCreateView
};

// Represents SQL Create statements.
// Example: "CREATE TABLE students (name TEXT, student_number INTEGER, city TEXT, grade DOUBLE)"
struct CreateStatement : SQLStatement {
    explicit CreateStatement(CreateType type);
    ~CreateStatement() override;

    CreateType type;
    bool ifNotExists; // default: false
    char* filePath;   // default: nullptr
    char* schema;     // default: nullptr
    char* tableName;  // default: nullptr
    std::vector<ColumnDefinition*>* columns; // default: nullptr
    std::vector<char*>* viewColumns;
    SelectStatement* select;
};

}
