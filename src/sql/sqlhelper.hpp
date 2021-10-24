#pragma once

#include "statements.hpp"

namespace zsql {

    // Prints a summary of the given SQLStatement.
    void printStatementInfo(std::ostream &, const SQLStatement* stmt);

    // Prints a summary of the given SelectStatement with the given indentation.
    void printSelectStatementInfo(std::ostream &,const SelectStatement* stmt, uintmax_t num_indent);

    // Prints a summary of the given ImportStatement with the given indentation.
    void printImportStatementInfo(std::ostream &,const ImportStatement* stmt, uintmax_t num_indent);

    // Prints a summary of the given CopyStatement with the given indentation.
    void printExportStatementInfo(std::ostream &,const ExportStatement* stmt, uintmax_t num_indent);

    // Prints a summary of the given InsertStatement with the given indentation.
    void printInsertStatementInfo(std::ostream &,const InsertStatement* stmt, uintmax_t num_indent);

    // Prints a summary of the given CreateStatement with the given indentation.
    void printCreateStatementInfo(std::ostream &,const CreateStatement* stmt, uintmax_t num_indent);

    // Prints a summary of the given TransactionStatement with the given indentation.
    void printTransactionStatementInfo(std::ostream &,const TransactionStatement* stmt, uintmax_t num_indent);

    // Prints a summary of the given Expression with the given indentation.
    void printExpression(std::ostream &,Expr* expr, uintmax_t num_indent);

    // Prints an ORDER BY clause
    void printOrderBy(std::ostream &,const std::vector<OrderDescription*>* expr, uintmax_t num_indent);

}