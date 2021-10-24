#include "isNull.hpp"

using namespace indexes;
using namespace mapping;
using namespace std;
#include "kernels.hpp"

namespace queries::model {

void IsNullModel::mergeChildrenCandidates() {

}

bool IsNullModel::execShaderThreadOnCpu(uint32_t thread,uint32_t nbThreads) {
    uint32_t len = (jobs.size()+nbThreads-1)/nbThreads;
    uint32_t start =  len * thread;
    Kernels k;
    return k.execKernelNumbers<MergeNone>(indexManagers, workingSet, jobs, start, len,codegen);
}

void IsNullModel::execute(queries::builders::AST *ast) {
    executeQuery(ast);
}

}