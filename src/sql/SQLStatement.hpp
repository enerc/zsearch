#pragma once

#include <vector>
#include "expr.hpp"

namespace zsql {

enum StatementType {
    kStmtError, // unused
    kStmtSelect,
    kStmtImport,
    kStmtInsert,
    kStmtUpdate,
    kStmtDelete,
    kStmtCreate,
    kStmtDrop,
    kStmtPrepare,
    kStmtExecute,
    kStmtExport,
    kStmtRename,
    kStmtAlter,
    kStmtShow,
    kStmtTransaction
};

// Base struct for every SQL statement
struct SQLStatement {

    explicit SQLStatement(StatementType type);

    virtual ~SQLStatement();

    [[nodiscard]] StatementType type() const;

    [[nodiscard]] bool isType(StatementType type) const;

    // Shorthand for isType(type).
    [[nodiscard]] bool is(StatementType type) const;

    // Length of the string in the SQL query string
    size_t stringLength{};

    std::vector<Expr*>* hints;

private:
    StatementType type_;

};

}