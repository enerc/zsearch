#include "notEquals.hpp"

using namespace indexes;
using namespace mapping;
using namespace std;
#include "kernels.hpp"

namespace queries::model {

void NotEqualsModel::mergeChildrenCandidates() {
}

uint64_t NotEqualsModel::execShaderThreadOnCpu(uint32_t thread,uint32_t nbThreads) {
    uint32_t len = (jobs.size()+nbThreads-1)/nbThreads;
    uint32_t start =  len * thread;
    Kernels k;
    for (const auto &j : exprList) {
        if (j->getClassInstanceType() == ClassInstanceString) {
            return k.execKernelVar<OperNotEqualType, MergeNone>(indexManagers.at(0), workingSet, jobs, start, len,j->values.at(0));
        }
    }
    return k.execKernelNumbers<MergeNone>(indexManagers, workingSet, jobs, start, len,codegen);
}

void NotEqualsModel::execute(queries::builders::AST *ast) {
    executeQuery(ast);
}

}
