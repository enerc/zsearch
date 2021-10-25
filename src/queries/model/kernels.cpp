#include "kernels.hpp"
#include "../../repository/storageModel.hpp"

using namespace std;
using namespace indexes;
using namespace devices;

namespace queries::model {

inline void mergeDeleted(uint64_t *workingSet,const void *deletedInfo) {
    fastAnd(workingSet,workingSet,deletedInfo,CHUNK_SIZE/8);
}
bool Kernels::execKernelNone(const std::shared_ptr<indexes::IndexManager> &indexManager, uint64_t *workingSet,const std::vector<uint32_t>& jobs, uint32_t start, uint32_t JobLen) {
    for (uint32_t i = start; i < start + JobLen && i < jobs.size(); i++) {
        shared_ptr<IndexChunk> c = indexManager->getChunkForRead(jobs.at(i));
        const uint32_t chunkId = c->getChunk();
        fastMemcpy4(workingSet+chunkId * CHUNK_SIZE / 64,c->getBase()->getDeletedInfo(),CHUNK_SIZE/8);
    }
    return true;
}

// Assume all indexes have the same size ... for now
template<MergeOper merge> bool Kernels::execKernelNumbers(const std::vector<std::shared_ptr<indexes::IndexManager>> &indexManagers,uint64_t *workingSet,const std::vector<uint32_t>& jobs,uint32_t start, uint32_t JobLen,const Codegen &code) {
    if (indexManagers.at(0)->getMaxArraySize() == 1) {
        return execKernelNumbersNoArray<merge>(indexManagers,workingSet,jobs,start,JobLen,code);
    }
    vector<void*> bases;
    bases.resize(indexManagers.size());
    vector<uint32_t> blocSizes;
    blocSizes.resize(indexManagers.size());
    auto banks = (uint8_t*)aligned_alloc(256,MAX_BANKS*8*32);
    if (banks == nullptr) {
        Log::panic("Fails to alloc space for kernel data.");
    }

    int z = 0;
    for (const auto &e : indexManagers) {
        blocSizes.at(z) = StorageModelFactory::getModelSize(e->getLength());
        z++;
    }

    for (uint32_t i = start; i < start + JobLen && i < jobs.size() && jobs.at(i) < indexManagers.at(0)->getNumberOfChunks(); i++) {
        z = 0;
        for (const auto &e : indexManagers) {
            bases.at(z) = e->getData(jobs.at(i));
            z++;
        }
        shared_ptr<IndexChunk> c0 = indexManagers.at(0)->getChunkForRead(jobs.at(i));
        uint32_t w = c0->getBase()->getNbRows();
        const uint32_t chunkId = c0->getChunk();
        uint32_t nbFullBlocs = ((w&15) == 0) ? w/16 :  1 +(w/16);
        uint32_t nbDocuments = c0->getBase()->getNbDocuments();
        uint32_t documentId = 0;
        uint64_t res = 0;

        for (int j=0; j< nbFullBlocs; j++) {
            auto e = (uint16_t*)bases.at(0);
            uint16_t d = code.exec(&bases.at(0),banks);
            if (d == 0) {
                uint32_t o0 = documentId & 63;
                documentId += __builtin_popcount(e[1]);
                uint32_t o1 = documentId & 63;
                if (o1 < o0) {
                    if (merge == MergeOr) {
                        workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6) - 1] |= res;
                    } else if (merge == MergeAnd) {
                        workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6) - 1] &= res;
                    } else {
                        workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6) - 1] = res;
                    }
                    res = 0;
                }
            } else {
                for (int k = 0; k < 16; k++ ) {
                    if (_bextr_u32(d,k,1) == 1 && documentId < nbDocuments) {
                        res |= 1L << (documentId & 63);
                    }
                    if ((documentId & 63) == 63) {
                        if (merge == MergeOr) {
                            workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6)] |= res;
                        } else if (merge == MergeAnd) {
                            workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6)] &= res;
                        } else {
                            workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6)] = res;
                        }
                        res = 0;
                    }
                    documentId += _bextr_u32(e[1],k,1);
                }
            }
            for (int l=0; l < indexManagers.size(); l++) {
                bases.at(l) = (void*) ((uint8_t*)bases.at(l) + blocSizes.at(l));
            }
        }
        if ((documentId & 63) > 0) {
            if ( merge == MergeOr) {
                workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6)] |= res;
            } else if (merge == MergeAnd) {
                workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6)] &= res;
            } else {
                workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6)] = res;
            }
        }
        for (const auto &e : indexManagers) {
            mergeDeleted(workingSet+chunkId * CHUNK_SIZE / 64,e->getDeletedPtr(jobs.at(i)));
        }
    }
    free(banks);
    return true;
}

template<MergeOper merge> bool Kernels::execKernelNumbersNoArray(const std::vector<std::shared_ptr<indexes::IndexManager>> &indexManagers,uint64_t *workingSet,const std::vector<uint32_t>& jobs,uint32_t start, uint32_t JobLen,const Codegen &code) {
    vector<void*> bases;
    bases.resize(indexManagers.size());
    vector<uint32_t> blocSizes;
    blocSizes.resize(indexManagers.size());
    auto banks = (uint8_t*)aligned_alloc(256,MAX_BANKS*8*32);
    if (banks == nullptr) {
        Log::panic("Fails to alloc space for kernel data.");
    }

    int z = 0;
    for (const auto &e : indexManagers) {
        blocSizes.at(z) = StorageModelFactory::getModelSize(e->getLength());
        z++;
    }

    for (uint32_t i = start; i < start + JobLen && i < jobs.size() && jobs.at(i) < indexManagers.at(0)->getNumberOfChunks(); i++) {
        z = 0;
        for (const auto &e : indexManagers) {
            bases.at(z) = e->getData(jobs.at(i));
            z++;
        }
        shared_ptr<IndexChunk> c0 = indexManagers.at(0)->getChunkForRead(jobs.at(i));
        uint32_t w = c0->getBase()->getNbRows();
        const uint32_t chunkId = c0->getChunk();
        uint32_t nbFullBlocs = ((w&15) == 0) ? w/16 :  1 +(w/16);
        uint32_t documentId = 0;
        uint64_t res = 0;

        for (int j=0; j< nbFullBlocs; j++) {
            uint16_t d = code.exec(&bases.at(0),banks);
            res |= (uint64_t)d << (documentId & 63);
            documentId += 16;
            if (j%4 == 3) {
                if (merge == MergeOr) {
                    workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6) - 1] |= res;
                } else if (merge == MergeAnd) {
                    workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6) - 1] &= res;
                } else {
                    workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6) - 1] = res;
                }
                res = 0;
            }

            for (int l=0; l < indexManagers.size(); l++) {
                bases.at(l) = (void*) ((uint8_t*)bases.at(l) + blocSizes.at(l));
            }
        }
        if ((documentId & 63) > 0) {
            if ( merge == MergeOr) {
                workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6)] |= res;
            } else if (merge == MergeAnd) {
                workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6)] &= res;
            } else {
                workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6)] = res;
            }
        }
        for (const auto &e : indexManagers) {
            mergeDeleted(workingSet+chunkId * CHUNK_SIZE / 64,e->getDeletedPtr(jobs.at(i)));
        }
    }
    free(banks);
    return true;
}

template<OperatorType oper,MergeOper merge> bool Kernels::execKernelVar(const std::shared_ptr<indexes::IndexManager> &indexManager, uint64_t *workingSet,const std::vector<uint32_t>& jobs, uint32_t start, uint32_t JobLen, const vector<string>  &toSearch) {
    auto banks = (uint8_t*)aligned_alloc(256,MAX_BANKS*8*32);
    if (banks == nullptr) {
        Log::panic("Fails to alloc space for kernel data.");
    }

    for (uint32_t i = start; i < start+JobLen && i < jobs.size(); i++) {
        shared_ptr<IndexChunk> c = indexManager->getChunkForRead(jobs.at(i));
        const uint32_t chunkId = c->getChunk();
        const uint32_t len = c->getBase()->getLength();
        const uint32_t w = c->getBase()->getNbRows();
        if (w == 0) continue;
        const uint16_t blocSize = StorageModelFactory::getModelSize(len);
        const uint32_t nbDocuments = c->getBase()->getNbDocuments();
        uint32_t nbFullBlocs = ((w&15) == 0) ? w/16 :  1 +(w/16);
        auto ptr = (uint8_t*)c->getBase()->getYmmData();
        uint32_t documentId = 0;
        uint64_t res = 0;
        if (len == 0) continue; // empty chunk
        Codegen code;
        code.fetchVariable((int)len,false,false,32,false,0,0,1);

        bool hasOneKey = false;
        for (const auto &e:toSearch) {
            auto k = c->getDictionary()->getKey(e);
            if (k != -1) {
                __uint128_t o = k;
                code.loadConstant(o,32,false,0,true);
                hasOneKey = true;
                //cout << "Key found " << e << " chunk #"<<chunkId << "  " << k << " len=" << len <<  endl;
            } else {
                //cout << "Key Not found " << e << " chunk #"<<chunkId << endl;
            }
        }

        if (!hasOneKey && oper == OperEqualType) continue;
        if (oper == OperEqualType) code.in32(0);
        else if (oper == OperNotEqualType) code.notin32(0);
        code.flush();

        for (int j = 0; j < nbFullBlocs; j++) {
            auto e = (uint16_t *) ptr;
            uint16_t d = code.exec(&ptr, banks);
            if (d == 0) {
                uint32_t o0 = documentId & 63;
                documentId += __builtin_popcount(e[1]);
                uint32_t o1 = documentId & 63;
                if (o1 < o0) {
                    if (merge == MergeOr) {
                        workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6) - 1] |= res;
                    } else if (merge == MergeAnd) {
                        workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6) - 1] &= res;
                    } else {
                        workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6) - 1] = res;
                    }
                    res = 0;
                }
            } else {
                for (int k = 0; k < 16; k++) {
                    if (_bextr_u32(d, k, 1) == 1 && documentId < nbDocuments) {
                        res |= 1L << (documentId & 63);
                    }
                    if ((documentId & 63) == 63) {
                        if (merge == MergeOr) {
                            workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6)] |= res;
                        } else if (merge == MergeAnd) {
                            workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6)] &= res;
                        } else {
                            workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6)] = res;
                        }
                        res = 0;
                    }
                    documentId += _bextr_u32(e[1], k, 1);
                }
            }
            ptr += blocSize;
        }
        if ((documentId & 63) > 0) {
            if (merge == MergeOr) {
                workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6)] |= res;
            } else if (merge == MergeAnd) {
                workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6)] &= res;
            } else {
                workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6)] = res;
            }
        }
        mergeDeleted(workingSet + chunkId * CHUNK_SIZE / 64, c->getBase()->getDeletedInfo());
    }
    free(banks);
    return true;
}


template bool Kernels::execKernelNumbers<MergeOr>(const std::vector<std::shared_ptr<indexes::IndexManager>> &indexManagers,uint64_t *workingSet,const std::vector<uint32_t>& jobs,uint32_t start, uint32_t JobLen,const Codegen &code);
template bool Kernels::execKernelNumbers<MergeNone>(const std::vector<std::shared_ptr<indexes::IndexManager>> &indexManagers,uint64_t *workingSet,const std::vector<uint32_t>& jobs,uint32_t start, uint32_t JobLen,const Codegen &code);
template bool Kernels::execKernelNumbers<MergeAnd>(const std::vector<std::shared_ptr<indexes::IndexManager>> &indexManagers,uint64_t *workingSet,const std::vector<uint32_t>& jobs,uint32_t start, uint32_t JobLen,const Codegen &code);
template bool Kernels::execKernelVar<OperEqualType,MergeOr> (const std::shared_ptr<indexes::IndexManager> &indexManager, uint64_t *workingSet,const std::vector<uint32_t>& jobs, uint32_t start, uint32_t JobLen, const vector<string>  &toSearch) ;
template bool Kernels::execKernelVar<OperEqualType,MergeNone> (const std::shared_ptr<indexes::IndexManager> &indexManager, uint64_t *workingSet,const std::vector<uint32_t>& jobs, uint32_t start, uint32_t JobLen, const vector<string>  &toSearch) ;
template bool Kernels::execKernelVar<OperEqualType,MergeAnd> (const std::shared_ptr<indexes::IndexManager> &indexManager, uint64_t *workingSet,const std::vector<uint32_t>& jobs, uint32_t start, uint32_t JobLen, const vector<string>  &toSearch) ;
template bool Kernels::execKernelVar<OperNotEqualType,MergeOr> (const std::shared_ptr<indexes::IndexManager> &indexManager, uint64_t *workingSet,const std::vector<uint32_t>& jobs, uint32_t start, uint32_t JobLen, const vector<string>  &toSearch) ;
template bool Kernels::execKernelVar<OperNotEqualType,MergeNone> (const std::shared_ptr<indexes::IndexManager> &indexManager, uint64_t *workingSet,const std::vector<uint32_t>& jobs, uint32_t start, uint32_t JobLen, const vector<string>  &toSearch) ;
template bool Kernels::execKernelVar<OperNotEqualType,MergeAnd> (const std::shared_ptr<indexes::IndexManager> &indexManager, uint64_t *workingSet,const std::vector<uint32_t>& jobs, uint32_t start, uint32_t JobLen, const vector<string>  &toSearch) ;

}
