#include "root.hpp"
#include "../../repository/indexRepository.hpp"
#include "../../queries/builders/ast.hpp"

#include <thread>

using namespace std;
using namespace mapping;
using namespace indexes;

namespace queries::model {

void RootModel::selectChunks(){
    jobs.clear();
    const uint32_t m = indexManagers.at(0)->getNumberOfChunks();

    for (int i =0 ; i< m; i++) {
        const auto &c = indexManagers.at(0)->getState(i);
        if (c != ChunkStateEmpty) {
            jobs.push_back(i);
        }
    }
}

void RootModel::mergeChildrenCandidates() {
}

bool RootModel::execShaderOnCpu() {
    auto processor_count = min(thread::hardware_concurrency(),(uint)jobs.size());
    if (processor_count == 0) {
        processor_count = 1;
    }
    vector<pthread_t> threads;
    vector<WorkerInitParam> w;
    for (int i = 0; i < processor_count; i++) {
        WorkerInitParam e;
        e.t = this;
        e.me = i;
        e.nbThreads = processor_count;
        w.push_back(e);
    }
    for (int i = 0; i < processor_count; i++) {
        pthread_t t;
        pthread_create(&t, nullptr, RootModel::InternalThreadEntryFunc, &w.at(i));
        threads.push_back(t);
    }
    for (auto &i : threads) {
        void* status;
        pthread_join(i, &status);
    }
    threads.clear();
    w.clear();
    return true;
}

__uint128_t RootModel::getResult(bool &isFloat) const {
    isFloat = getClassInstanceType() == ClassInstanceFloat;
    if (!vals.empty()) return vals.at(0);
    else return 0;
}

void RootModel::parseSubExpression(queries::builders::AST *ast) {
    //cout << "ROOT " << getClassInstanceType() << " " << myName() << endl;
}

void RootModel::executeQuery(queries::builders::AST *ast) {
    if (ast->getColumnMapping() == nullptr) {
        return;
    }
    uint64_t n = now();
    parseSubExpression(ast);

    if (indexManagers.empty()) {
        // ex. select count(bar) from foo;
        uint32_t f = IndexRepository::getInstance()->getNumberOfChunks(ast->getColumnMapping()->getIndexName());
        if (!allocWorkingSet(f*CHUNK_SIZE/8)) {
            return;
        }
        fastOne4(workingSet,f*CHUNK_SIZE/8);
        const string &s = IndexRepository::getInstance()->getOneMember(ast->getColumnMapping()->getIndexName());
        const auto &e = IndexRepository::getInstance()->getIndex(ast->getColumnMapping()->getIndexName(), s);
        for (int chunkId=0; chunkId< f; chunkId++) {
            shared_ptr<IndexChunk> p = e->getChunkForRead(chunkId);
            fastAnd(workingSet+chunkId * CHUNK_SIZE / 64,workingSet+chunkId * CHUNK_SIZE / 64,p->getBase()->getDeletedInfo(),CHUNK_SIZE/8);
        }
        nbResults = fastCount(workingSet,workingSetSize);
    } else {
        if (!allocWorkingSet(indexManagers.at(0)->getNumberOfChunks()*CHUNK_SIZE/8)) {
            return;
        }
        selectChunks();
        execShaderOnCpu();
        postFilter();
        nbResults = fastCount(workingSet,workingSetSize);
        Log::debug("Query/subquery "+myName() + " took " + timeSpent(n) + ". There were "+to_string(jobs.size()) + " chunks to scan (about "+ to_string(jobs.size()*CHUNK_SIZE)+ " documents). Output has "+to_string(nbResults)+" results");
    }
}

__uint128_t RootModel::getFuncResult(const vector<__uint128_t> &a, __uint128_t count) const {
    __uint128_t ret = getGlobalFuncStartVal();
    for (const auto &i: a) {
        ret = processGlobalFunc(i,ret);
    }
    return ret;
}


}