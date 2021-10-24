#include "delete.hpp"

#include "../../mapping/indexDefinitionManager.hpp"
#include "../builders/ast.hpp"
#include "../../repository/indexRepository.hpp"
#include "where.hpp"

using namespace std;
using namespace mapping;
using namespace indexes;

namespace queries::model {

void DeleteModel::execute(queries::builders::AST *ast) {
    shared_ptr<IndexDefinition> id = IndexDefinitionManager::getInstance()->getIndex(tableName);
    if (id == nullptr) {
        Log::error(ast->getQueryStatus(),"Drop index "+tableName+ " failed. Table not found");
        return;
    }
    if (exprList.empty() ) {
        IndexDefinitionManager::getInstance()->truncateIndex(tableName);
    } else {
        IndexRepository::getInstance()->deleteDocuments(tableName,ast->getWhere()->getWorkingSet(),ast->getWhere()->getWorkingSetSize());
    }
}

}