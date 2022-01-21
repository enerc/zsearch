#include "in.hpp"
#include "../../queries/builders/ast.hpp"
#include "../../repository/indexRepository.hpp"
#include "../../common/tools.hpp"

using namespace indexes;
using namespace mapping;
using namespace std;
#include "kernels.hpp"

namespace queries::model {

uint64_t InModel::execShaderThreadOnCpu(uint32_t thread,uint32_t nbThreads) {
    uint32_t len = (jobs.size()+nbThreads-1)/nbThreads;
    uint32_t start =  len * thread;
    Kernels k;
    return k.execKernelNumbers<MergeNone>(indexManagers, workingSet, jobs, start, len,codegen);
}

void InModel::execute(queries::builders::AST *ast) {
    executeQuery(ast);
}

}
