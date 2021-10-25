#include "not.hpp"
#include "../../queries/builders/ast.hpp"

using namespace std;
using namespace indexes;

namespace queries::model {

void NotModel::execute(queries::builders::AST *ast) {
    if (exprList.size() != 1) {
        Log::error(ast->getQueryStatus(),"SQL query NOT should have 1 children. "+ast->getSaveqQuery());
        return;
    }
    auto a = exprList.at(0)->getWorkingSet();
    if (a == nullptr) {
        Log::error(ast->getQueryStatus(),"SQL subexpression NOT failed "+ast->getSaveqQuery());
        return;
    }
    allocWorkingSet(exprList.at(0)->getWorkingSetSize());

    for (int chunk=0; chunk < workingSetSize*8/CHUNK_SIZE; chunk ++) {
        shared_ptr<IndexChunk> c = exprList.at(0)->getIndexManager(0)->getChunkForRead(chunk);
        uint32_t l = 128*(c->getBase()->getNbDocuments()/1024);
        uint64_t p = chunk*CHUNK_SIZE/64;
        fastNot(workingSet+p,exprList.at(0)->getWorkingSet()+p, l);
        for (uint32_t i=l*8; i < c->getBase()->getNbDocuments(); i++) {
            uint32_t k = i >> 6;
            uint32_t x = i&63;
            uint64_t b = (exprList.at(0)->getWorkingSet()[p+k] >> x) &1;
            b = b == 0 ? 1 : 0;
            workingSet[p+k] |=  b << x;
        }
        fastAnd(workingSet+p,workingSet+p,c->getBase()->getDeletedInfo(),CHUNK_SIZE/8);
    }
    exprList.at(0)->freeWorkingSet();

    nbResults = fastCount(workingSet,workingSetSize);
}

}
