#include "lessEq.hpp"

using namespace indexes;
using namespace mapping;
using namespace std;
#include "kernels.hpp"

namespace queries::model {

void LessEqModel::selectChunks() {
    if (hasFunctionInside) {
        return RootModel::selectChunks();
    } else {
        jobs.clear();
        const uint32_t m = indexManagers.at(0)->getNumberOfChunks();

        for (int i =0 ; i< m; i++) {
            const auto &st = indexManagers.at(0)->getState(i);
            if (st != ChunkStateEmpty && indexManagers.at(0)->compareLessEq(i,toFilter,isaFloat)) {
                jobs.push_back(i);
            }
        }

    }
}

bool LessEqModel::execShaderThreadOnCpu(uint32_t thread,uint32_t nbThreads) {
    uint32_t len = (jobs.size()+nbThreads-1)/nbThreads;
    uint32_t start =  len * thread;
    Kernels k;
    return k.execKernelNumbers<MergeNone>(indexManagers, workingSet, jobs, start, len,codegen);
}

void LessEqModel::execute(queries::builders::AST *ast) {
    executeQuery(ast);
}

}