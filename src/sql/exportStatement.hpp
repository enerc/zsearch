#pragma once

#include "importStatement.hpp"
#include "SQLStatement.hpp"

namespace zsql {
// Represents SQL Export statements.
struct ExportStatement : SQLStatement {
    explicit ExportStatement(ImportType type);
    ~ExportStatement() override;

    // ImportType is used for compatibility reasons
    ImportType type;
    char* filePath;
    char* schema;
    char* tableName;
};

}