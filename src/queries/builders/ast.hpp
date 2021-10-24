#pragma once
#include <map>
#include <vector>
#include <memory>

#include "../model/base.hpp"
#include "../model/importFile.hpp"
#include "../../sql/SQLStatement.hpp"
#include "../../sql/statements.hpp"

namespace queries::model {
    class WhereModel;
    class SelectModel;
    class OrderModel;
    class RootModel;
}

namespace queries::builders {

class AST {
public:
    AST(const zsql::SQLStatement *, std::shared_ptr<QueryStatus> &q, std::string query);

    void iterate(bool isAdmin);
    [[nodiscard]] std::shared_ptr<mapping::IndexDefinition> getColumnMapping() const;
    [[nodiscard]] std::shared_ptr<QueryStatus> &getQueryStatus() ;
    [[nodiscard]] std::string getSaveqQuery() const {return savedQuery;}
    [[nodiscard]] std::string getResultAsFiles(int &nbFiles,uint64_t &resultSize);
    [[nodiscard]] std::shared_ptr<queries::model::WhereModel> getWhere() const { return where; }
    [[nodiscard]] const std::vector<std::string> &getFromFunctionTables() const {return fromFunctionTables; }
private:
    std::shared_ptr<queries::model::BaseModel> base;
    std::shared_ptr<queries::model::WhereModel> where;
    std::shared_ptr<queries::model::SelectModel> select;
    std::shared_ptr<queries::model::OrderModel> order;
    std::shared_ptr<mapping::IndexDefinition> columnMapping;
    std::shared_ptr<QueryStatus> queryStatus;
    std::map<std::string,std::string> aliases;
    std::vector<queries::model::FieldAndFunction> fieldsToReturn;
    std::vector<std::string>   fieldsToGroup;
    std::vector<std::string>   fieldsToOrder;
    std::vector<std::string>   fromTables;
    std::vector<std::string>   fromFunctionTables;

private:
    std::vector<bool> orderAsc;
    uint64_t limit;
    uint64_t offset;
    std::string savedQuery;

    bool convertStatement(const zsql::SQLStatement* stmt, std::shared_ptr<queries::model::BaseModel> b);
    bool convertSelectStatement(const zsql::SelectStatement* stmt, std::shared_ptr<queries::model::BaseModel> b);
    bool convertInsertStatement(const zsql::InsertStatement* stmt, std::shared_ptr<queries::model::BaseModel> b);
    bool convertCreateStatement(const zsql::CreateStatement* stmt, std::shared_ptr<queries::model::BaseModel> b);
    bool convertImportStatement(const zsql::ImportStatement* stmt, std::shared_ptr<queries::model::ImportFileModel> b);
    bool convertExportStatement(const zsql::ExportStatement* stmt, std::shared_ptr<queries::model::BaseModel> b);
    bool convertTransactionStatement(const zsql::TransactionStatement* stmt, std::shared_ptr<queries::model::BaseModel> b);
    bool convertDropStatement(const zsql::DropStatement* stmt, std::shared_ptr<queries::model::BaseModel> b);
    bool convertDeleteStatement(const zsql::DeleteStatement* stmt, std::shared_ptr<queries::model::BaseModel> b);
    bool processTableRef(const zsql::TableRef* table);
    bool convertExpression(const zsql::Expr *expr, std::shared_ptr<queries::model::BaseModel> b);
    bool convertOrderBy(const std::vector<zsql::OrderDescription*>* expr, std::shared_ptr<queries::model::BaseModel> b);
    bool convertOperatorExpression(const zsql::Expr *expr, std::shared_ptr<queries::model::BaseModel> b);
    std::shared_ptr<queries::model::RootModel> makeFunctionObject(zsql::OperatorType type,std::shared_ptr<queries::model::BaseModel> b) ;

    void iterate(const std::shared_ptr<queries::model::BaseModel>&);
};
}