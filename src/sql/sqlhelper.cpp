#include "sqlhelper.hpp"
#include <iostream>
#include <map>
#include <string>

using namespace std;

namespace zsql {

void printOperatorExpression(Expr* expr, uintmax_t numIndent);
void printAlias(std::ostream &o,Alias* alias, uintmax_t numIndent);

ostream& operator<<(ostream& os, const OperatorType& op);
ostream& operator<<(ostream& os, const DatetimeField& op);

string indent(uintmax_t numIndent) {
    return string(numIndent, '\t');
}
void inprint(std::ostream &o,int64_t val, uintmax_t numIndent) {
    o << indent(numIndent).c_str() << val << "  " << endl;
}
void inprint(std::ostream &o,double val, uintmax_t numIndent) {
    o << indent(numIndent).c_str() << val << endl;
}
void inprint(std::ostream &o,const char* val, uintmax_t numIndent) {
    o << indent(numIndent).c_str() << val << endl;
}
void inprint(std::ostream &o,const char* val, const char* val2, uintmax_t numIndent) {
    o << indent(numIndent).c_str() << val << "->" << val2 << endl;
}
void inprintC(std::ostream &o,char val, uintmax_t numIndent) {
    o << indent(numIndent).c_str() << val << endl;
}
void inprint(std::ostream &o,const OperatorType& op, uintmax_t numIndent) {
    o << indent(numIndent) << op << endl;
}
void inprint(std::ostream &o,const ColumnType& colType, uintmax_t numIndent) {
    o << indent(numIndent) << colType << endl;
}
void inprint(std::ostream &o,const DatetimeField& colType, uintmax_t numIndent) {
    o << indent(numIndent) << colType << endl;
}

void printTableRefInfo(std::ostream &o,TableRef* table, uintmax_t numIndent) {
    switch (table->type) {
        case kTableName:
            inprint(o,table->name, numIndent);
            if(table->schema)  {
                inprint(o,"Schema", numIndent + 1);
                inprint(o,table->schema, numIndent + 2);
            }
            break;
        case kTableSelect:
            printSelectStatementInfo(o,table->select, numIndent);
            break;
        case kTableJoin:
            inprint(o,"Join Table", numIndent);
            inprint(o,"Left", numIndent + 1);
            printTableRefInfo(o,table->join->left, numIndent + 2);
            inprint(o,"Right", numIndent + 1);
            printTableRefInfo(o,table->join->right, numIndent + 2);
            inprint(o,"Join Condition", numIndent + 1);
            printExpression(o,table->join->condition, numIndent + 2);
            break;
        case kTableCrossProduct:
            for (TableRef* tbl : *table->list) printTableRefInfo(o,tbl, numIndent);
            break;
    }

    if (table->alias) {
        printAlias(o,table->alias, numIndent);
    }
}

void printAlias(std::ostream &o,Alias* alias, uintmax_t numIndent) {
    inprint(o,"Alias", numIndent + 1);
    inprint(o,alias->name, numIndent + 2);

    if (alias->columns) {
        for (char* column : *(alias->columns)) {
            inprint(o,column, numIndent + 3);
        }
    }
}

void printOperatorExpression(std::ostream &o,Expr* expr, uintmax_t numIndent) {
    if (expr == nullptr) {
        inprint(o,"null", numIndent);
        return;
    }

    inprint(o,expr->opType, numIndent);

    printExpression(o,expr->expr, numIndent + 1);
    if (expr->expr2 != nullptr) {
        printExpression(o,expr->expr2, numIndent + 1);
    } else if (expr->exprList != nullptr) {
        for (Expr* e : *expr->exprList) printExpression(o,e, numIndent + 1);
    }
}

void printExpression(std::ostream &o,Expr* expr, uintmax_t numIndent) {
    if (!expr) return;
    switch (expr->type) {
        case kExprStar:
            inprint(o,"*", numIndent);
            break;
        case kExprColumnRef:
            inprint(o,expr->name, numIndent);
            if(expr->table) {
                inprint(o,"Table:", numIndent+1);
                inprint(o,expr->table, numIndent+2);
            }
            break;
            // case kExprTableColumnRef: inprint(o,expr->table, expr->name, numIndent); break;
        case kExprLiteralFloat:
            inprint(o,expr->fval, numIndent);
            break;
        case kExprLiteralInt:
            inprint(o,(int64_t)expr->ival, numIndent);
            break;
        case kExprLiteralString:
            inprint(o,expr->name, numIndent);
            break;
        case kExprFunctionRef:
        case kExprDistanceSphere:
        case kExprSrid:
            inprint(o,expr->name, numIndent);
            for (Expr* e : *expr->exprList) printExpression(o,e, numIndent + 1);
            break;
        case kExprExtract:
            inprint(o,expr->name, numIndent);
            inprint(o,expr->datetimeField, numIndent + 1);
            printExpression(o,expr->expr, numIndent + 1);
            break;
        case kExprCast:
            inprint(o,expr->name, numIndent);
            inprint(o,expr->columnType, numIndent + 1);
            printExpression(o,expr->expr, numIndent + 1);
            break;
        case kExprOperator:
            printOperatorExpression(o,expr, numIndent);
            break;
        case kExprSelect:
            printSelectStatementInfo(o,expr->select, numIndent);
            break;
        case kExprParameter:
            inprint(o,(int64_t)expr->ival, numIndent);
            break;
        case kExprArray:
            for (Expr* e : *expr->exprList) printExpression(o,e, numIndent + 1);
            break;
        case kExprArrayIndex:
            printExpression(o,expr->expr, numIndent + 1);
            inprint(o,(int64_t)expr->ival, numIndent);
            break;
        default:
            cerr << "Unrecognized expression type " << expr->type << endl;
            return;
    }
    if (expr->alias != nullptr) {
        inprint(o,"Alias", numIndent + 1);
        inprint(o,expr->alias, numIndent + 2);
    }
}

void printOrderBy(std::ostream &o,const vector<OrderDescription*>* expr, uintmax_t numIndent) {
    if (!expr) return;
    for (const auto& order_description : *expr) {
        printExpression(o,order_description->expr, numIndent);
        if (order_description->type == kOrderAsc) {
            inprint(o,"ascending", numIndent);
        }
        else {
            inprint(o,"descending", numIndent);
        }
    }
}

void printSelectStatementInfo(std::ostream &o,const SelectStatement* stmt, uintmax_t numIndent) {
    inprint(o,"SelectStatement", numIndent);
    inprint(o,"Fields:", numIndent + 1);
    for (Expr* expr : *stmt->selectList) printExpression(o,expr, numIndent + 2);

    if (stmt->fromTable != nullptr) {
        inprint(o,"Sources:", numIndent + 1);
        printTableRefInfo(o,stmt->fromTable, numIndent + 2);
    }

    if (stmt->whereClause != nullptr) {
        inprint(o,"Search Conditions:", numIndent + 1);
        printExpression(o,stmt->whereClause, numIndent + 2);
    }

    if (stmt->groupBy != nullptr) {
        inprint(o,"GroupBy:", numIndent + 1);
        for (Expr* expr : *stmt->groupBy->columns) printExpression(o,expr, numIndent + 2);
        if (stmt->groupBy->having != nullptr) {
            inprint(o,"Having:", numIndent + 1);
            printExpression(o,stmt->groupBy->having, numIndent + 2);
        }
    }

    if (stmt->setOperations != nullptr) {
        for (SetOperation* setOperation : *stmt->setOperations) {
            switch (setOperation->setType) {
                case SetType::kSetIntersect:
                    inprint(o,"Intersect:", numIndent + 1);
                    break;
                case SetType::kSetUnion:
                    inprint(o,"Union:", numIndent + 1);
                    break;
                case SetType::kSetExcept:
                    inprint(o,"Except:", numIndent + 1);
                    break;
            }

            printSelectStatementInfo(o,setOperation->nestedSelectStatement, numIndent + 2);

            if (setOperation->resultOrder != nullptr) {
                inprint(o,"SetResultOrderBy:", numIndent + 1);
                printOrderBy(o,setOperation->resultOrder, numIndent + 2);
            }

            if (setOperation->resultLimit != nullptr) {
                if (setOperation->resultLimit->limit != nullptr) {
                    inprint(o,"SetResultLimit:", numIndent + 1);
                    printExpression(o,setOperation->resultLimit->limit, numIndent + 2);
                }

                if (setOperation->resultLimit->offset != nullptr) {
                    inprint(o,"SetResultOffset:", numIndent + 1);
                    printExpression(o,setOperation->resultLimit->offset, numIndent + 2);
                }
            }
        }
    }

    if (stmt->order != nullptr) {
        inprint(o,"OrderBy:", numIndent + 1);
        printOrderBy(o,stmt->order, numIndent + 2);
    }

    if (stmt->limit != nullptr && stmt->limit->limit != nullptr) {
        inprint(o,"Limit:", numIndent + 1);
        printExpression(o,stmt->limit->limit, numIndent + 2);
    }

    if (stmt->limit != nullptr && stmt->limit->offset != nullptr) {
        inprint(o,"Offset:", numIndent + 1);
        printExpression(o,stmt->limit->offset, numIndent + 2);
    }
}



void printImportStatementInfo(std::ostream &o,const ImportStatement* stmt, uintmax_t numIndent) {
    inprint(o,"ImportStatement", numIndent);
    inprint(o,stmt->filePath, numIndent + 1);
    switch (stmt->type) {
        case ImportType::kImportJSON:
            inprint(o,"JSON", numIndent + 1);
            break;
        case ImportType::kImportLZ4:
            inprint(o,"LZ4", numIndent + 1);
            break;
        case ImportType::kImportAuto:
            inprint(o,"AUTO", numIndent + 1);
            break;
    }
    inprint(o,stmt->tableName, numIndent + 1);
}

void printExportStatementInfo(std::ostream &o,const ExportStatement* stmt, uintmax_t numIndent) {
    inprint(o,"ExportStatement", numIndent);
    inprint(o,stmt->filePath, numIndent + 1);
    switch (stmt->type) {
        case ImportType::kImportJSON:
            inprint(o,"JSON", numIndent + 1);
            break;
        case ImportType::kImportLZ4:
            inprint(o,"LZ4", numIndent + 1);
            break;
        case ImportType::kImportAuto:
            inprint(o,"AUTO", numIndent + 1);
            break;
    }
    inprint(o,stmt->tableName, numIndent + 1);
}

void printCreateStatementInfo(std::ostream &o,const CreateStatement* stmt, uintmax_t numIndent) {
    inprint(o,"CreateStatement", numIndent);
    inprint(o,stmt->tableName, numIndent + 1);
    if (stmt->filePath) inprint(o,stmt->filePath, numIndent + 1);
}

void printInsertStatementInfo(std::ostream &o,const InsertStatement* stmt, uintmax_t numIndent) {
    inprint(o,"InsertStatement", numIndent);
    inprint(o,stmt->tableName, numIndent + 1);
    if (stmt->columns != nullptr) {
        inprint(o,"Columns", numIndent + 1);
        for (char* col_name : *stmt->columns) {
            inprint(o,col_name, numIndent + 2);
        }
    }
    switch (stmt->type) {
        case kInsertValues:
            inprint(o,"Values", numIndent + 1);
            for (Expr* expr : *stmt->values) {
                printExpression(o,expr, numIndent + 2);
            }
            break;
        case kInsertSelect:
            printSelectStatementInfo(o,stmt->select, numIndent + 1);
            break;
    }
}

void printTransactionStatementInfo(std::ostream &o,const TransactionStatement* stmt, uintmax_t numIndent) {
    inprint(o,"TransactionStatement", numIndent);
    switch (stmt->command){
        case kBeginTransaction:
            inprint(o,"BEGIN", numIndent + 1);
            break;
        case kCommitTransaction:
            inprint(o,"COMMIT", numIndent + 1);
            break;
        case kRollbackTransaction:
            inprint(o,"ROLLBACK", numIndent + 1);
            break;
    }
}

void printStatementInfo(std::ostream &o,const SQLStatement* stmt) {
    switch (stmt->type()) {
        case kStmtSelect:
            printSelectStatementInfo(o,(const SelectStatement*) stmt, 0);
            break;
        case kStmtInsert:
            printInsertStatementInfo(o,(const InsertStatement*) stmt, 0);
            break;
        case kStmtCreate:
            printCreateStatementInfo(o,(const CreateStatement*) stmt, 0);
            break;
        case kStmtImport:
            printImportStatementInfo(o,(const ImportStatement*) stmt, 0);
            break;
        case kStmtExport:
            printExportStatementInfo(o,(const ExportStatement*) stmt, 0);
            break;
        case kStmtTransaction:
            printTransactionStatementInfo(o,(const TransactionStatement*) stmt, 0);
            break;
        default:
            break;
    }
}

ostream& operator<<(ostream& os, const OperatorType& op) {
    static const map<const OperatorType, const string> operatorToToken = {
            {kOpNone, "None"},
            {kOpBetween, "BETWEEN"},
            {kOpCase, "CASE"},
            {kOpCaseListElement, "CASE LIST ELEMENT"},
            {kOpPlus, "+"},
            {kOpMinus, "-"},
            {kOpAsterisk, "*"},
            {kOpSlash, "/"},
            {kOpPercentage, "%"},
            {kOpCaret, "^"},
            {kOpEquals, "="},
            {kOpNotEquals, "!="},
            {kOpLess, "<"},
            {kOpLessEq, "<="},
            {kOpGreater, ">"},
            {kOpGreaterEq, ">="},
            {kOpLike, "LIKE"},
            {kOpNotLike, "NOT LIKE"},
            {kOpILike, "ILIKE"},
            {kOpAnd, "AND"},
            {kOpOr, "OR"},
            {kOpXor, "XOR"},
            {kOpIn, "IN"},
            {kOpConcat, "CONCAT"},
            {kOpNot, "NOT"},
            {kOpUnaryMinus, "-"},
            {kOpIsNull, "IS NULL"},
            {kOpExists, "EXISTS"},
            {kOpPoint,"POINT"}
    };

    const auto found = operatorToToken.find(op);
    if (found == operatorToToken.cend()) {
        return os << static_cast<uint64_t>(op);
    } else {
        return os << (*found).second;
    }
}

ostream& operator<<(ostream& os, const DatetimeField& datetime) {
    static const map<const DatetimeField, const string> operatorToToken = {
            {kDatetimeNone, "None"},
            {kDatetimeSecond, "SECOND"},
            {kDatetimeMinute, "MINUTE"},
            {kDatetimeHour, "HOUR"},
            {kDatetimeDay, "DAY"},
            {kDatetimeMonth, "MONTH"},
            {kDatetimeYear, "YEAR"}
    };

    const auto found = operatorToToken.find(datetime);
    if (found == operatorToToken.cend()) {
        return os << static_cast<uint64_t>(datetime);
    } else {
        return os << (*found).second;
    }
}

}