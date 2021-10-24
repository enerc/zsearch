#pragma once

#include "SQLStatement.hpp"

namespace zsql {

// Represents SQL Transaction statements.
// Example: BEGIN TRANSACTION;
enum TransactionCommand {
    kBeginTransaction,
    kCommitTransaction,
    kRollbackTransaction
};

struct TransactionStatement : SQLStatement {
    explicit TransactionStatement(TransactionCommand command);
    ~TransactionStatement() override;

    TransactionCommand command;
};

}