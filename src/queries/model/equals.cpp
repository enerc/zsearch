#include "equals.hpp"
#include "../../queries/builders/ast.hpp"

using namespace indexes;
using namespace mapping;
using namespace std;
#include "kernels.hpp"

namespace queries::model {

void EqualsModel::selectChunks() {
    if (hasFunctionInside || !exprList.at(1)->values.at(0).empty()) {
        return RootModel::selectChunks();
    } else {
        jobs.clear();
        const uint32_t m = indexManagers.at(0)->getNumberOfChunks();

        for (int i =0 ; i< m; i++) {
            const auto &st = indexManagers.at(0)->getState(i);
            if (st != ChunkStateEmpty && (indexManagers.at(0)->compareLess(i,toFilter,isaFloat) || indexManagers.at(0)->compareGreater(i,toFilter,isaFloat))) {
                jobs.push_back(i);
            }
        }
    }
}

bool EqualsModel::execShaderThreadOnCpu(uint32_t thread,uint32_t nbThreads) {
    uint32_t len = (jobs.size()+nbThreads-1)/nbThreads;
    uint32_t start =  len * thread;
    Kernels k;
    for (const auto &j : exprList) {
        if (j->getClassInstanceType() == ClassInstanceString) {
            return k.execKernelVar<OperEqualType, MergeNone>(indexManagers.at(0), workingSet, jobs, start, len,j->values.at(0));
        }
    }
    return k.execKernelNumbers<MergeNone>(indexManagers, workingSet, jobs, start, len,codegen);
}

void EqualsModel::execute(queries::builders::AST *ast) {
    executeQuery(ast);
}

}