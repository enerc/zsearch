#pragma once

#include "SQLStatement.hpp"

namespace zsql {

enum ImportType {
    kImportJSON,
    kImportLZ4,
    kImportAuto
};

// Represents SQL Import statements.
struct ImportStatement : SQLStatement {
    explicit ImportStatement(ImportType type);
    ~ImportStatement() override;

    ImportType type;
    char* filePath;
    char* schema;
    char* tableName;
};

}
