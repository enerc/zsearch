#include "ast.hpp"

#include <utility>
#include "../model/and.hpp"
#include "../model/caseListElement.hpp"
#include "../model/equals.hpp"
#include "../model/greaterEq.hpp"
#include "../model/in.hpp"
#include "../model/like.hpp"
#include "../model/not.hpp"
#include "../model/select.hpp"
#include "../model/array.hpp"
#include "../model/between.hpp"
#include "../model/exists.hpp"
#include "../model/greater.hpp"
#include "../model/isNull.hpp"
#include "../model/minus.hpp"
#include "../model/notLike.hpp"
#include "../model/percentage.hpp"
#include "../model/slash.hpp"
#include "../model/caret.hpp"
#include "../model/columnRef.hpp"
#include "../model/lessEq.hpp"
#include "../model/none.hpp"
#include "../model/plus.hpp"
#include "../model/star.hpp"
#include "../model/asterisk.hpp"
#include "../model/case.hpp"
#include "../model/concat.hpp"
#include "../model/iLike.hpp"
#include "../model/less.hpp"
#include "../model/notEquals.hpp"
#include "../model/or.hpp"
#include "../model/xor.hpp"
#include "../model/point.hpp"
#include "../model/unaryMinus.hpp"
#include "../model/where.hpp"
#include "../model/having.hpp"
#include "../model/intersect.hpp"
#include "../model/union.hpp"
#include "../model/except.hpp"
#include "../model/order.hpp"
#include "../model/limit.hpp"
#include "../model/offset.hpp"
#include "../model/delete.hpp"
#include "../model/insert.hpp"
#include "../model/values.hpp"
#include "../model/srid.hpp"
#include "../model/distanceSphere.hpp"
#include "../model/count.hpp"
#include "../model/min.hpp"
#include "../model/max.hpp"
#include "../model/sum.hpp"
#include "../model/avg.hpp"
#include "../model/drop.hpp"


using namespace std;
using namespace zsql;
using namespace queries::model;
using namespace mapping;

namespace queries::builders {

AST::AST(const SQLStatement *statement, shared_ptr<QueryStatus> &q,std::string query) : queryStatus(q) , savedQuery(std::move(query)){
    std::shared_ptr<BaseModel> a;
    base = make_shared<RootModel>(a);
    limit = Config::getInstance()->getLimit();
    offset = 0;
    where = nullptr;
    convertStatement(statement,base);
}

bool AST::convertStatement(const SQLStatement *stmt, shared_ptr<BaseModel> b) {
    switch (stmt->type()) {
        case kStmtSelect: {
            select = make_shared<SelectModel>(b);
            b->exprList.push_back(select);
            convertSelectStatement((const SelectStatement *) stmt,select);
        }
        break;
        case kStmtInsert: {
            b->exprList.push_back(make_shared<InsertModel>(b));
            convertInsertStatement((const InsertStatement *) stmt, b->exprList.back());
        }
        break;
        case kStmtCreate:
            convertCreateStatement((const CreateStatement *) stmt, b);
            break;
        case kStmtImport: {
            shared_ptr<ImportFileModel> e = make_shared<ImportFileModel>(b);
            b->exprList.push_back(e);
            convertImportStatement((const ImportStatement *) stmt, e);
        }
        break;
        case kStmtExport:
            convertExportStatement((const ExportStatement *) stmt, b);
            break;
        case kStmtTransaction:
            convertTransactionStatement((const TransactionStatement *) stmt, b);
            break;
        case kStmtDrop:
            convertDropStatement((const DropStatement *) stmt, b);
            break;
        case kStmtDelete:
            convertDeleteStatement((const DeleteStatement *) stmt, b);
            break;
        default:
            break;
    }
    return true;
}

bool AST::convertSelectStatement(const SelectStatement *stmt, shared_ptr<BaseModel> b) {
    if (stmt->fromTable != nullptr) {
        if (!processTableRef(stmt->fromTable)) {
            return false;
        }
    } else {
        fromTables = fromFunctionTables;
    }

    for (auto const expr : *stmt->selectList) {
        FieldAndFunction fa;
        fa.field = expr->name == nullptr ? "a" : expr->name;
        fa.alias = fa.field;
        fa.function = nullptr;
        if (expr->alias != nullptr) {
            fa.alias = expr->alias;
        }
        if (expr->type == kExprColumnRef) {
            fieldsToReturn.push_back(fa);
        } else if (expr->type == kExprDistanceSphere || expr->type == kExprFunctionRef) {
            fa.function = makeFunctionObject(expr->opType,(b));
            for (const auto &i : *expr->exprList) {
                convertExpression(i, fa.function);
            }
            if (!fa.function->getExprList().empty() && !fa.function->getExprList().at(0)->values.empty()) {
                fa.field = fa.function->getExprList().at(0)->values.at(0).at(0);
            }
            fieldsToReturn.push_back(fa);
        } else if (expr->type == zsql::kExprStar) {
            if (fromTables.size() == 0) {
                Log::error(queryStatus,"at least one table must be set for select *");
                return false;
            }
            auto id = IndexDefinitionManager::getInstance()->getIndex(fromTables.at(0));
            if (id == nullptr) {
                Log::error(queryStatus,"table "+fromTables.at(0)+" not found");
                return false;
            }
            auto v = id->getIndexFields();
            for (const auto &i : v) {
                FieldAndFunction fa;
                fa.field = i;
                fa.alias = fa.field;
                fa.function = nullptr;
                fieldsToReturn.push_back(fa);
            }
            columnMapping = id;
        } else {
            Log::error(queryStatus, "Invalid type \"" + string(ExprTypeName[expr->type]) + "\" for a selected columns. Must be a column or a function.");
            return false;
        }
    }

    where = make_shared<WhereModel>(b);
    if (stmt->whereClause != nullptr) {
        where->setNoWhere(false);
        b->exprList.push_back(where);
        convertExpression(stmt->whereClause, where);
    } else {
        where->setNoWhere(true);
        shared_ptr<NoneModel> n = make_shared<NoneModel>(b);
        where->exprList.push_back(n);
        b->exprList.push_back(where);
    }

    if (stmt->groupBy != nullptr) {
        for (const auto expr : *stmt->groupBy->columns) {
            if (expr->type == kExprColumnRef) {
                fieldsToGroup.emplace_back(expr->table);
                fromTables.emplace_back(expr->table);
            } else {
                Log::error(queryStatus,"Invalid type for a group by column");
            }
        }
        if (stmt->groupBy->having != nullptr) {
            b->exprList.push_back(make_shared<HavingModel>(b));
            convertExpression(stmt->groupBy->having, b->exprList.back());
        }
    }

    if (stmt->setOperations != nullptr) {
        for (SetOperation *setOperation : *stmt->setOperations) {
            switch (setOperation->setType) {
                case SetType::kSetIntersect: {
                    b->exprList.push_back(make_shared<IntersectModel>(b));
                }
                break;
                case SetType::kSetUnion: {
                    b->exprList.push_back(make_shared<UnionModel>(b));
                }
                break;
                case SetType::kSetExcept: {
                    b->exprList.push_back(make_shared<ExceptModel>(b));
                }
                break;
            }

            convertSelectStatement(setOperation->nestedSelectStatement, b->exprList.back());

            if (setOperation->resultOrder != nullptr) {
                b->exprList.push_back(make_shared<OrderModel>(b));
                convertOrderBy(setOperation->resultOrder,b->exprList.back());
            }

            if (setOperation->resultLimit != nullptr) {
                if (setOperation->resultLimit->limit != nullptr) {
                    b->exprList.push_back(make_shared<LimitModel>(b));
                    convertExpression(setOperation->resultLimit->limit, b->exprList.back());
                }

                if (setOperation->resultLimit->offset != nullptr) {
                    b->exprList.push_back(make_shared<OffsetModel>(b));
                    convertExpression(setOperation->resultLimit->offset, b->exprList.back());
                }
            }
        }
    }

    if (stmt->order != nullptr) {
        b->exprList.push_back(make_shared<RootModel>(b));
        convertOrderBy(stmt->order, b->exprList.back());
    }

    if (stmt->limit != nullptr && stmt->limit->limit != nullptr) {
        if (stmt->limit->limit->type == kExprLiteralInt) {
            limit = (int64_t)stmt->limit->limit->ival;
        } else {
            Log::error(queryStatus,"SQL limit must be a number.");
        }
    }

    if (stmt->limit != nullptr && stmt->limit->offset != nullptr) {
        if (stmt->limit->offset->type == kExprLiteralInt) {
            offset = (int64_t)stmt->limit->offset->ival;
        } else {
            Log::error(queryStatus,"SQL offset must be a number.");
        }
    }
    return true;
}


bool AST::convertImportStatement(const ImportStatement *stmt, shared_ptr<ImportFileModel> b) {
    b->path = stmt->filePath;
    switch (stmt->type) {
        case ImportType::kImportJSON:
            b->type = repository::FileTypeText;
            break;
        case ImportType::kImportLZ4:
            b->type = repository::FileTypeLZ4;
            break;
        case ImportType::kImportAuto:
            b->type = repository::FileTypeText;
            break;
    }
    b->indexName = stmt->tableName;
    return true;
}

bool AST::convertExportStatement(const ExportStatement *stmt, shared_ptr<BaseModel> b) {
    Log::error(queryStatus,"SQL export not implemented.");
    return false;
}

bool AST::convertCreateStatement(const CreateStatement *stmt, shared_ptr<BaseModel> b) {
    Log::error(queryStatus,"SQL create statement not implemented.");
    return false;
}

bool AST::convertDropStatement(const DropStatement *stmt, shared_ptr<BaseModel> b) {
    auto a = make_shared<DropModel>(b);
    a->tableName = stmt->name;
    a->ifExists = stmt->ifExists;
    b->exprList.push_back(a);
    return true;
}

bool AST::convertDeleteStatement(const zsql::DeleteStatement* stmt, shared_ptr<BaseModel> b) {
    auto a = make_shared<DeleteModel>(b);
    a->schema = stmt->schema == nullptr ? "" : string(stmt->schema);
    a->tableName = stmt->tableName;
    if (stmt->expr) {
        fromTables.emplace_back(a->tableName );
        where = make_shared<WhereModel>(b);
        where->setNoWhere(false);
        a->exprList.push_back(where);
        convertExpression(stmt->expr, where);
    }
    b->exprList.push_back(a);
    return true;
}

bool AST::convertInsertStatement(const InsertStatement *stmt, shared_ptr<BaseModel> b) {
    std::shared_ptr<InsertModel> a = make_shared<InsertModel>(b);
    std::shared_ptr<BaseModel> ab = static_pointer_cast<BaseModel>(a);

    a->tableName = stmt->tableName;
    fromTables.push_back(a->tableName);
    if (stmt->columns != nullptr) {
        for (const auto col_name : *stmt->columns) {
            a->columns.emplace_back(col_name);
            columnMapping = IndexDefinitionManager::getIndexManager(col_name, fromTables,queryStatus);
            if (columnMapping == nullptr) {
                Log::error(queryStatus, string("Column ") + col_name + " not found in \"" + savedQuery+"\"");
                return false; // error set
            }
        }
    }
    switch (stmt->type) {
        case kInsertValues: {
            std::shared_ptr<ValuesModel> z = make_shared<ValuesModel>(ab);
            std::shared_ptr<BaseModel> d = static_pointer_cast<BaseModel>(z);
            a->exprList.push_back(z);
            for (const auto expr : *stmt->values) {
                std::shared_ptr<ValuesModel> k = make_shared<ValuesModel>(d);
                convertExpression(expr, k);
                z->items.push_back(k);
            }
        }
        break;
        case kInsertSelect: {
            b->exprList.push_back(make_shared<InsertModel>(ab));
            convertSelectStatement(stmt->select, b->exprList.back());
        }
        break;
    }
    b->exprList.push_back(a);
    return true;
}

bool AST::convertTransactionStatement(const TransactionStatement *stmt, shared_ptr<BaseModel> b) {
    Log::error(queryStatus,"SQL transaction statements not implemented. This is a query engine, not a database.");
    return false;
}

bool AST::processTableRef(const TableRef *table) {
    string ftn;
    switch (table->type) {
        case kTableName: {
            if (table->schema) {
                ftn += string(table->schema) + ".";
            }
            ftn += table->name;
            std::shared_ptr<mapping::IndexDefinition> o = IndexDefinitionManager::getInstance()->getIndex(ftn);
            if (o == nullptr) {
                Log::error(queryStatus, string("Table ") + ftn + " not found");
                return false;
            }
            fromTables.emplace_back(ftn);
        }
        break;
        case kTableSelect:
            Log::error(queryStatus,"Table as select not supported yet. Inline your data in the index with a primary key to an other index.");
            return false;
        case kTableJoin:
            Log::error(queryStatus,"Table joins not supported. This does not scale well with large datasets. Inline your data in the index with a primary key to an other index.");
            return false;
        case kTableCrossProduct:
            Log::error(queryStatus,"Table cross joins not supported. This does not scale at all with large datasets. Inline your data in the index with a primary key to an other index.");
            return false;
    }

    if (table->alias && !ftn.empty()) {
        aliases[table->alias->name] = ftn;
    }
    return true;
}

std::shared_ptr<queries::model::RootModel> AST::makeFunctionObject(zsql::OperatorType type,shared_ptr<BaseModel> b) {
    switch (type) {
        case kOpPoint:
            return make_shared<PointModel>(b);
        case kOpSrid:
            return make_shared<SridModel>(b);
        case kOpDistanceSphere:
            return make_shared<DistanceSphereModel>(b);
        case kOpCount:
            return make_shared<CountModel>(b);
        case kOpMin:
            return make_shared<MinModel>(b);
        case kOpMax:
            return make_shared<MaxModel>(b);
        case kOpSum:
            return make_shared<SumModel>(b);
        case kOpAvg:
            return make_shared<AvgModel>(b);
        default:
            Log::error(queryStatus,"unrecognized operator type in \"" + savedQuery +"\"");
            return make_shared<RootModel>(b);;
    }
}


bool AST::convertExpression(const Expr *expr, shared_ptr<BaseModel> b) {
    if (!expr) return false;

    switch (expr->type) {
        case kExprStar: {
            b->exprList.push_back(make_shared<StarModel>(b));
        }
        break;
        case kExprColumnRef: {
            b->exprList.push_back(make_shared<ColumnRefModel>(b));
            string s;
            if (expr->table) {
                s += string(expr->table) + ".";
            }
            s += expr->name;
            b->exprList.back()->values.at(0).push_back(s);
            columnMapping = IndexDefinitionManager::getIndexManager(s,fromTables,queryStatus);
            if (columnMapping == nullptr) {
                Log::error(queryStatus,string("Column ")+s+" not found in \""+savedQuery+"\"");
                return false; // error set
            }
            fromFunctionTables.emplace_back(s);
        }
        break;
        // case kExprTableColumnRef: inprint(expr->table, expr->name, numIndent); break;
        case kExprLiteralFloat: {
            b->exprList.push_back(make_shared<RootModel>(b));
            b->exprList.back()->vals.at(0) = convertTo128d(expr->fval);
            b->exprList.back()->setClassInstanceType(ClassInstanceFloat);
            b->exprList.back()->setMyName("float");
        }
        break;
        case kExprLiteralInt: {
            b->exprList.push_back(make_shared<RootModel>(b));
            b->exprList.back()->vals.at(0) = expr->ival;
            b->exprList.back()->setClassInstanceType(ClassInstanceInteger);
            b->exprList.back()->setMyName("int");
        }
        break;
        case kExprLiteralString: {
            b->exprList.push_back(make_shared<RootModel>(b));
            b->exprList.back()->values.at(0).push_back(expr->name);
            b->exprList.back()->setClassInstanceType(ClassInstanceString);
        }
        break;
        case kExprFunctionRef:{
            b->values.at(0).emplace_back(expr->name);
            for (const auto e : *expr->exprList) {
                std::shared_ptr<RootModel> z = makeFunctionObject(e->opType,b);
                convertExpression(e, z);
                b->exprList.push_back(z);
            }
        }
        break;
        case kExprSrid: {
            b->values.at(0).emplace_back(expr->name);
            std::shared_ptr<RootModel> z = makeFunctionObject(kOpSrid,b);
            b->exprList.push_back(z);
            for (const auto e : *expr->exprList) {
                convertExpression(e, z);
            }
        }
        break;
        case kExprDistanceSphere: {
            b->values.at(0).emplace_back(expr->name);
            std::shared_ptr<RootModel> z = makeFunctionObject(kOpDistanceSphere,b);
            b->exprList.push_back(z);
            for (const auto e : *expr->exprList) {
                convertExpression(e, z);
            }
        }
        break;
        case kExprExtract: {
            //std::shared_ptr<ExtractModel> z = make_shared<ExtractModel>(b);
            //std::shared_ptr<BaseModel> d = static_pointer_cast<BaseModel>(z);
            //z->values.push_back(expr->name);
            //expr->datetimeField;
            //expression(expr->expr;
            Log::error(queryStatus,"SQL date field extract not implemented since whole JSON is returned.");
            return false;
        }
        case kExprCast:
            //inprint(expr->name;
            //inprint(expr->columnType;
            //printExpression(expr->expr
            Log::error(queryStatus,"SQL cast not implemented since whole JSON is returned.");
            break;
        case kExprOperator:
            convertOperatorExpression(expr, b);
            break;
        case kExprSelect:
            convertSelectStatement(expr->select,b);
            break;
        case kExprParameter: {
            b->exprList.push_back(make_shared<RootModel>(b));
            b->exprList.back()->vals.at(0) = expr->ival;
        }
        break;
        case kExprArray: {
            b->exprList.push_back(make_shared<ArrayModel>(b));
            for (const auto e : *expr->exprList) {
                std::shared_ptr<RootModel> z = make_shared<RootModel>(b);
                convertExpression(e, z);
                b->exprList.push_back(z);
            }
        }
        break;
        case kExprArrayIndex:
            Log::error(queryStatus,"SQL array indexes not supported yet.");
            //expr->expr;
            //inprint((int64_t) expr->ival;
            return false;
        default:
            Log::error(queryStatus, "Unrecognized expression type " + to_string(expr->type));
            return false;
    }
    // ???
    // if (expr->alias != nullptr) {
    //    inprint("Alias";
    //    inprint(expr->alias;
    //}
    return true;
}

bool AST::convertOrderBy(const std::vector<OrderDescription*>* expr, shared_ptr<BaseModel> b) {
    if (!expr) return false;
    shared_ptr<OrderModel> c = make_shared<OrderModel>(b);
    for (const auto& order_description : *expr) {
        if (order_description->expr->type == kExprColumnRef) {
            if (b == nullptr) {
                fieldsToOrder.emplace_back(order_description->expr->name);
            } else {
                b->fieldsToOrder.emplace_back(order_description->expr->name);
            }
        } else if (order_description->expr->type == kExprOperator) {
            convertExpression(order_description->expr,c);
        } else if (order_description->expr->type == kExprFunctionRef) {
            cout << "order by FUNCTION REF" << endl;
        } else if (order_description->expr->type == kExprDistanceSphere) {
            convertExpression(order_description->expr,c);
            shared_ptr<DistanceSphereModel> distanceSphere = static_pointer_cast<DistanceSphereModel>(c->exprList.at(0));
            if (!distanceSphere->getExprList().empty() && !distanceSphere->getExprList().at(0)->values.empty()) {
                if (b == nullptr) {
                    fieldsToOrder.emplace_back(distanceSphere->getExprList().at(0)->values.at(0).at(0));
                } else {
                    b->fieldsToOrder.emplace_back(distanceSphere->getExprList().at(0)->values.at(0).at(0));
                }
            }
        } else {
            Log::error(queryStatus, "Unsupported order expression " + savedQuery);
            return false;
        }
        if (order_description->type == kOrderAsc) {
            if (b == nullptr) {
                orderAsc.push_back(true);
            } else {
                b->orderAsc.push_back(true);
            }
        }
        else {
            if (b == nullptr) {
                orderAsc.push_back(false);
            } else {
                b->orderAsc.push_back(false);
            }
        }
    }
    if (b == nullptr) {
        c->setFieldsToOrder(fieldsToOrder);
        c->setOrder(orderAsc);
    } else {
        c->setFieldsToOrder(b->fieldsToOrder);
        c->setOrder(b->orderAsc);
    }
    c->setColumnMapping(columnMapping);
    c->setFromTables(fromTables);
    c->setWhere(where);
    order = c;
    b->exprList.push_back(c);
    return true;
}

bool AST::convertOperatorExpression(const Expr *expr, shared_ptr<BaseModel> b) {
    std::shared_ptr<BaseModel> d;
    switch(expr->opType) {
        case kOpNone: {
            b->exprList.push_back(make_shared<NoneModel>(b));
        }
        break;
        case kOpBetween: {
            b->exprList.push_back(make_shared<BetweenModel>(b));
        }
        break;
        case kOpCase: {
            b->exprList.push_back(make_shared<CaseModel>(b));
        }
        break;
        case kOpCaseListElement: {
            b->exprList.push_back(make_shared<CaseListElementModel>(b));
        }
        break;
        case kOpPlus: {
            b->exprList.push_back(make_shared<PlusModel>(b));
        }
        break;
        case kOpMinus: {
            b->exprList.push_back(make_shared<MinusModel>(b));
        }
        break;
        case kOpAsterisk: {
            b->exprList.push_back(make_shared<AsteriskModel>(b));
        }
        break;
        case kOpSlash: {
            b->exprList.push_back(make_shared<SlashModel>(b));
        }
        break;
        case kOpPercentage: {
            b->exprList.push_back(make_shared<PercentageModel>(b));
        }
        break;
        case kOpCaret: {
            b->exprList.push_back(make_shared<CaretModel>(b));
        }
        break;
        case kOpEquals: {
            b->exprList.push_back(make_shared<EqualsModel>(b));
        }
        break;
        case kOpNotEquals: {
            b->exprList.push_back(make_shared<NotEqualsModel>(b));
        }
        break;
        case kOpLess: {
            b->exprList.push_back(make_shared<LessModel>(b));
        }
        break;
        case kOpLessEq: {
            b->exprList.push_back(make_shared<LessEqModel>(b));
        }
        break;
        case kOpGreater: {
            b->exprList.push_back(make_shared<GreaterModel>(b));
        }
        break;
        case kOpGreaterEq: {
            b->exprList.push_back(make_shared<GreaterEqModel>(b));
        }
        break;
        case kOpLike: {
            b->exprList.push_back(make_shared<LikeModel>(b));
        }
        break;
        case kOpNotLike: {
            b->exprList.push_back(make_shared<NotLikeModel>(b));
        }
        break;
        case kOpILike: {
            b->exprList.push_back(make_shared<ILikeModel>(b));
        }
        break;
        case kOpAnd: {
            b->exprList.push_back(make_shared<AndModel>(b));
        }
        break;
        case kOpOr: {
            b->exprList.push_back(make_shared<OrModel>(b));
        }
        break;
        case kOpXor: {
            b->exprList.push_back(make_shared<XorModel>(b));
        }
        break;
        case kOpIn: {
            b->exprList.push_back(make_shared<InModel>(b));
        }
        break;
        case kOpConcat: {
            b->exprList.push_back(make_shared<ConcatModel>(b));
        }
        break;
        case kOpNot: {
            b->exprList.push_back(make_shared<NotModel>(b));
        }
        break;
        case kOpUnaryMinus: {
            b->exprList.push_back(make_shared<UnaryMinusModel>(b));
        }
        break;
        case kOpIsNull: {
            b->exprList.push_back(make_shared<IsNullModel>(b));
        }
        break;
        case kOpExists: {
            b->exprList.push_back(make_shared<ExistsModel>(b));
        }
        break;
        case kOpPoint: {
            b->exprList.push_back(make_shared<PointModel>(b));
        }
        break;
        case kOpSrid:
        case kOpDistanceSphere:
        case kOpCount:
        case kOpMin:
        case kOpMax:
        case kOpSum:
        case kOpAvg:
            break;
    };

    convertExpression(expr->expr, b->exprList.back());
    if (expr->expr2 != nullptr) {
        convertExpression(expr->expr2, b->exprList.back());
    } else if (expr->exprList != nullptr) {
        for (const auto e : *expr->exprList) {
            convertExpression(e, b->exprList.back());
        }
    }
    return true;
}

void AST::iterate(bool isAdmin) {
    auto n = now();
    if (select != nullptr) {
        iterate(base);
    } else if (isAdmin) {
        if (!base->exprList.empty()) {
            if (base->exprList.at(0)->getClassInstanceType() == model::ClassInstanceDrop) {
                base->exprList.at(0)->execute(this);
            } else if (base->exprList.at(0)->getClassInstanceType() == model::ClassInstanceImportFile) {
                base->exprList.at(0)->execute(this);
            } else if (base->exprList.at(0)->getClassInstanceType() == model::ClassInstanceDelete) {
                if (!base->exprList.empty() ) {
                    iterate(where);
                }
                base->exprList.at(0)->execute(this);
            }
        }
    } else {
        Log::error(queryStatus,"Admin required");
    }
    if (where != nullptr) {
        Log::info("SQL query returns  "+to_string(where->getNbResults())+ " results in " + to_string((now()-n)) + "µS.");
    }
}

void AST::iterate(const std::shared_ptr<queries::model::BaseModel>&b) {
    if (queryStatus->status != HTTP_200 || b == nullptr) {
        return;
    }
    for (const auto &e : b->exprList) {
        iterate(e);
    }
    b->execute(this);
}

shared_ptr<mapping::IndexDefinition> AST::getColumnMapping() const {
    return columnMapping;
}

shared_ptr<QueryStatus> &AST::getQueryStatus()  {
    return queryStatus;
}

std::string AST::getResultAsFiles(int &nbFiles,uint64_t &resultSize) {
    uint64_t n = now();
    nbFiles = 0;
    resultSize = 0;
    string ret{};
    if (where == nullptr) {
        return ret;
    }
    if (select == nullptr) {
        return ret;
    }
    if (queryStatus->status == HTTP_200) {
        select->setFieldsToReturn(fieldsToReturn);
        select->setIndexDefinition(columnMapping);
        select->setOrderModel(order);
        ret = select->getResultAsFileThread(where, queryStatus,nbFiles,resultSize,offset,limit);
    }
    Log::debug("Time spent in creating JSON result: "+ timeSpent(n) + ". Output size is "+to_string(resultSize) + " bytes");
    return ret;
}

}
