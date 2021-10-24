#include "drop.hpp"
#include "../../mapping/indexDefinitionManager.hpp"
#include "../builders/ast.hpp"

using namespace std;
using namespace mapping;

namespace queries::model {

void DropModel::execute(queries::builders::AST *ast) {
    if (!IndexDefinitionManager::getInstance()->removeIndex(tableName)) {
        Log::error(ast->getQueryStatus(),"Drop index "+tableName+ " failed. Table not found");
    }
}

}