#include "none.hpp"

using namespace indexes;
using namespace mapping;
using namespace std;
#include "kernels.hpp"
#include "../builders/ast.hpp"
#include "columnRef.hpp"
#include "../../repository/indexRepository.hpp"

namespace queries::model {

void NoneModel::mergeChildrenCandidates() {
}

void NoneModel::parseSubExpression(queries::builders::AST *ast) {
    if (indexManagers.empty() && ast->getColumnMapping() != nullptr && !ast->getColumnMapping()->getIndexFields().empty()) {
        shared_ptr<IndexManager> im = IndexRepository::getInstance()->getIndex(ast->getColumnMapping()->getIndexName(), ast->getColumnMapping()->getIndexFields().at(0));
        indexManagers.push_back(im);
    }
}

uint64_t NoneModel::execShaderThreadOnCpu(uint32_t thread, uint32_t nbThreads) {
    uint32_t len = (jobs.size() + nbThreads - 1) / nbThreads;
    uint32_t start = len * thread;
    return Kernels::execKernelNone(indexManagers.at(0), workingSet, jobs, start, len);
}

void NoneModel::execute(queries::builders::AST *ast) {
    if (exprList.empty()) {
        shared_ptr<ColumnRefModel> r = make_shared<ColumnRefModel>(parent);
        r->values.at(0).push_back(ast->getColumnMapping()->getIndexFields().at(0));
        exprList.push_back(r);
    }
    executeQuery(ast);
}

}
