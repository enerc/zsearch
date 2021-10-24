#pragma once

#include "SQLStatement.hpp"

namespace zsql {

// Represents SQL Prepare statements.
// Example: PREPARE test FROM 'SELECT * FROM test WHERE a = ?;'
struct PrepareStatement : SQLStatement {
    PrepareStatement() :
            SQLStatement(kStmtPrepare),
            name(nullptr),
            query(nullptr) {}

    ~PrepareStatement() override {
        free(name);
        free(query);
    }
    char* name;

    // The query that is supposed to be prepared.
    char* query;
};

}