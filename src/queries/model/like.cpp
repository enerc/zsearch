#include "like.hpp"
#include <cstdint>

using namespace std;
using namespace indexes;
using namespace dictionary;
#include "kernels.hpp"

namespace queries::model {

#define STRSTRX(p) for (int j = 0; j < dic->getNbEntries(); j++) {\
    uint32_t size = offsets[1] - offsets[0];\
    if (size >= (p)) {\
        dicMatch[j >> 6] |= ((uint64_t) avx2_strstr<p>(avx2_search_needle_first_byte, avx2_search_needle_last_byte,(const char *) (stringPtr), size, w.c_str())) << (j & 63);\
    }\
    stringPtr += size; \
    offsets++; \
}

bool LikeModel::execShaderThreadOnCpu(uint32_t thread,uint32_t nbThreads) {
    uint32_t JobLen = (jobs.size() + nbThreads - 1) / nbThreads;
    uint32_t start = JobLen * thread;
    Kernels k;

    if (searchMode == SearchExactMatch) {
        return k.execKernelVar<OperEqualType,MergeNone>(indexManagers.at(0),workingSet,jobs,start,JobLen,exprList.at(1)->values.at(0));
    }

    const auto &w = exprList.at(1)->values.at(0).at(0);

    uint32_t allocatedDicSize = CHUNK_SIZE;
    vector<uint64_t> dicMatch;
    dicMatch.resize(allocatedDicSize/64);
    auto searching = (uint64_t)(unsigned char)w.at(0);
    uint64_t mask = 0xff;
    int q = 1;
    while (q < w.size() && w.at(q) != '%' && q < 8) {
        searching |= (uint64_t) w.at(q) << (8 * q);
        mask |= 0xffL << (8 * q);
        q++;
    }
    const __m256i avx2_search_needle_first_byte = _mm256_set1_epi8(w.front());
    const __m256i avx2_search_needle_last_byte = _mm256_set1_epi8(w.back());

    for (uint32_t i = start; i < start + JobLen && i < jobs.size(); i++) {
        shared_ptr<IndexChunk> c = indexManagers.at(0)->getChunkForRead(i);
        const uint32_t chunkId = c->getChunk();
        shared_ptr<Dictionary> dic = c->getDictionary();
        if (dic->getNbEntries() > allocatedDicSize) {
            allocatedDicSize = dic->getNbEntries()+63;
            dicMatch.resize(allocatedDicSize/64);
        }
        for (auto &e : dicMatch) {
            e = 0;
        }
        auto stringPtr = dic->getValuePtr();
        auto offsets = dic->getSizes();
        switch (searchMode) {
            case SearchMiddle: {
                switch (w.size()) {
                    case 1: {
                        for (int j = 0; j < dic->getNbEntries(); j++) {
                            uint32_t size = offsets[1] - offsets[0];
                            if (size > 0) {
                                dicMatch[j >> 6] |= ((uint64_t) (memchr((const void *) stringPtr, w.at(0), size) != nullptr)) << (j & 63);
                            }
                            stringPtr += size;
                            offsets++;
                        }
                    }
                        break;
                    case 2:
                        STRSTRX(2);
                        break;
                    case 3:
                        STRSTRX(3);
                        break;
                    case 4:
                        STRSTRX(4);
                        break;
                    case 5:
                        STRSTRX(5);
                        break;
                    case 6:
                        STRSTRX(6);
                        break;
                    case 7:
                        STRSTRX(7);
                        break;
                    case 8:
                        STRSTRX(8);
                        break;
                    case 9:
                        STRSTRX(9);
                        break;
                    case 10:
                        STRSTRX(10);
                        break;
                    case 11:
                        STRSTRX(11);
                        break;
                    case 12:
                        STRSTRX(12);
                        break;
                    case 13:
                        STRSTRX(13);
                        break;
                    case 14:
                        STRSTRX(14);
                        break;
                    case 15:
                        STRSTRX(15);
                        break;
                    case 16:
                        STRSTRX(16);
                        break;
                    case 17:
                        STRSTRX(17);
                        break;
                    case 18:
                        STRSTRX(18);
                        break;
                    case 19:
                        STRSTRX(19);
                        break;
                    case 20:
                        STRSTRX(20);
                        break;
                    case 21:
                        STRSTRX(21);
                        break;
                    case 22:
                        STRSTRX(22);
                        break;
                    case 23:
                        STRSTRX(23);
                        break;
                    case 24:
                        STRSTRX(24);
                        break;
                    case 25:
                        STRSTRX(25);
                        break;
                    case 26:
                        STRSTRX(26);
                        break;
                    case 27:
                        STRSTRX(27);
                        break;
                    case 28:
                        STRSTRX(28);
                        break;
                    case 29:
                        STRSTRX(29);
                        break;
                    case 30:
                        STRSTRX(30);
                        break;
                    case 31:
                        STRSTRX(31);
                        break;
                    case 32:
                        STRSTRX(32);
                        break;
                    default:
                        // 2.5 x slower because of gcc/clang optimizer
                        for (int j = 0; j < dic->getNbEntries(); j++) {
                            uint32_t size = offsets[1] - offsets[0];
                            if (size >= w.size()) {
                                dicMatch[j >> 6] |=
                                        ((uint64_t) avx2_strstr_any(avx2_search_needle_first_byte, avx2_search_needle_last_byte, (const char *) (stringPtr), size, w.c_str(), w.size())) << (j & 63);\

                            }

                            stringPtr += size;
                            offsets++;
                        }
                        break;
                }
            }
                break;
            case SearchFirst: {
                for (int j = 0; j < dic->getNbEntries(); j++) {
                    uint32_t size = offsets[1] - offsets[0];
                    auto p = (uint64_t *) stringPtr;
                    if (size >= w.size() && (p[0] & mask) == searching) {
                        dicMatch[j >> 6] |= ((uint64_t) (memcmp((const char *) (stringPtr), w.c_str(), w.size()) == 0)) << (j & 63);
                    }
                    stringPtr += size;
                    offsets++;
                }
            }
            break;
            case SearchExactMatch: {
                // should use optimized kernel
                for (int j = 0; j < dic->getNbEntries(); j++) {
                    uint32_t size = offsets[1] - offsets[0];
                    auto p = (uint64_t *) stringPtr;
                    if (size == w.size() && (p[0] & mask) == searching) {
                        dicMatch[j >> 6] |= ((uint64_t) (memcmp((const char *) (stringPtr), w.c_str(), w.size()) == 0)) << (j & 63);
                    }
                    stringPtr += size;
                    offsets++;
                }
            }
            break;
            case SearchEnd: {
                for (int j = 0; j < dic->getNbEntries(); j++) {
                    uint32_t size = offsets[1] - offsets[0];
                    auto p = (uint64_t*)(stringPtr+size-w.size());
                    if (size >= w.size() && (p[0] & mask) == searching) {
                        dicMatch[j >> 6] |= ((uint64_t) (memcmp((const char *) p, w.c_str(), w.size()) == 0)) << (j & 63);
                    }
                    stringPtr += size;
                    offsets++;
                }
            }
                break;
            case SearchFirstEnd: {
                size_t split = w.find('%');
                searching = (uint64_t)(unsigned char)w.at(0);
                mask = 0xff;
                q = 1;
                while (q < split && q < 8) {
                    searching |= (uint64_t) w.at(q) << (8 * q);
                    mask |= 0xffL << (8 * q);
                    q++;
                }

                auto toScanR = w.size()-split-1;
                for (int j = 0; j < dic->getNbEntries(); j++) {
                    uint32_t size = offsets[1] - offsets[0];
                    auto p = (uint64_t *) stringPtr;
                    if (size >= w.size()-1 && (p[0] & mask) == searching) {
                        bool b = memcmp((const char *) (stringPtr), w.c_str(), split) == 0;
                        if (b) {
                            b = memcmp((const char *) (stringPtr+size-toScanR), w.c_str()+split+1, toScanR) == 0;
                            dicMatch[j >> 6] |= (uint64_t)b << (j & 63);
                        }
                    }
                    stringPtr += size;
                    offsets++;
                }
                break;
            }
        }
        const uint32_t rows = c->getBase()->getNbRows();
        uint32_t documentId = 0;
        uint64_t res = 0;
        uint32_t p = 0;
        __uint128_t v;
        bool isNull;
        bool endOfDocument;
        bool isDeleted;
        while (p < rows) {
            c->getBase()->getValue(p,v,isNull,endOfDocument,isDeleted);
            res |= (_bextr_u64(dicMatch[((uint32_t)v)>>6],v&63,1)&(!isNull)&(!isDeleted)) << (documentId & 63);
            if ((documentId & 63) == 63) {
                workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6)] = res;
                res = 0;
            }
            documentId += endOfDocument ? 1 : 0;
            p ++;
        }
        if ((documentId & 63) > 0) {
            workingSet[chunkId * CHUNK_SIZE / 64 + (documentId >> 6)] = res;
        }
    }
    return true;
}

bool LikeModel::prepareJob() {
    if (exprList.size() > 1) {
        auto &v = exprList.at(1)->values.at(0).at(0);
        int o = 0;
        for (const auto &r : v ) {
            if (r == '%') o++;
        }

        bool p = false;
        if (v.front() == '%') {
            if (v.back() == '%') {
                if (v.size() < 3) {
                    return false;
                }
                v = v.substr(1,v.size()-2);
                searchMode = SearchMiddle; // %foo%
                p = true;
            } else {
                if (v.size() < 2) {
                    return false;
                }
                v = v.substr(1,v.size()-1);
                searchMode = SearchEnd; // %foo
                p = true;
            }
        }
        if (!p && v.back() == '%') {
            if (v.size() < 2) {
                return false;
            }
            v = v.substr(0,v.size()-1);
            searchMode = SearchFirst;   // foo%
            p = true;
        }
        if (o == 0) {
            searchMode = SearchExactMatch; // foo
            p = true;
        }
        if (!p && o == 1) {
            searchMode = SearchFirstEnd; // f%o
            p = true;
        }
        // else this a like on the full column, so do it as an enum
        if (!p) searchMode = SearchExactMatch;
    }
    return true;
}

void LikeModel::execute(queries::builders::AST *ast) {
    if (prepareJob()) {
        executeQuery(ast);
    }
}

}