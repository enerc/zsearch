#include "select.hpp"
#include "../../repository/indexRepository.hpp"
#include "where.hpp"
#include "order.hpp"
#include "count.hpp"
#include <sys/random.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

using namespace std;
using namespace mapping;
using namespace indexes;

namespace queries::model {

static void setNull(char *buffer,size_t &size) {
    buffer[size++] = 'n';
    buffer[size++] = 'u';
    buffer[size++] = 'l';
    buffer[size++] = 'l';
}

size_t SelectModel::resultToJson(uint64_t loc, char *buffer) {
    size_t size = 0;
    buffer[size++] = '{';
    uint32_t chunk = loc >> CHUNK_SIZE_LOG2;
    loc -= chunk << CHUNK_SIZE_LOG2;
    auto cloc = (uint32_t)loc;
    __uint128_t v;
    bool isNull;
    bool endOfDocument;
    bool isDeleted;
    int k = 0;

    for (const auto &o : fieldsToReturn) {
        const IndexManager *im = ca.indexManagerVector.at(k);
        const mapping_entry &mae = ca.mappingVector.at(k);
        bool isArray = (mae.flags & EntryArraySet) != 0;
        std::shared_ptr<StorageModel> model = im->getFixedStorageModel(chunk,isArray);

        if (k > 0) {
            buffer[size++] = ',';
        }
        buffer[size++] = '"';
        memcpy(buffer + size, o.alias.c_str(), o.alias.size());
        size += o.alias.size();
        buffer[size++] = '"';
        buffer[size++] = ':';

        if (o.function == nullptr) {
            if (mae.s == storage_type_enum) {
                if (!isArray) {
                    auto s = im->getEnumValue(chunk, cloc, 0, v, isNull, endOfDocument,isDeleted,isArray);
                    if (isNull) {
                        setNull(buffer, size);
                    } else {
                        buffer[size++] = '"';
                        if (!s.empty()) {
                            memcpy(buffer + size, s.data(), s.size());
                            size += s.size();
                        }
                        buffer[size++] = '"';
                    }
                } else {
                    buffer[size++] = '[';
                    endOfDocument = false;
                    int i = 0;
                    while (!endOfDocument) {
                        if (i > 0) {
                            buffer[size++] = ',';
                        }
                        auto s = im->getEnumValue(chunk, cloc,i, v, isNull, endOfDocument,isDeleted, isArray);
                        i++;
                        if (isNull) {
                            setNull(buffer, size);
                        } else {
                            buffer[size++] = '"';
                            if (!s.empty()) {
                                memcpy(buffer + size, s.data(), s.size());
                                size += s.size();
                            }
                            buffer[size++] = '"';
                        }
                    }
                    buffer[size++] = ']';
                }
            } else {
                if (!isArray) {
                    IndexManager::getFixedValue(model, cloc, 0, v, isNull, endOfDocument, isDeleted, isArray);
                    if (!isNull) {
                        size += convertToString(v, mae, buffer + size);
                    } else {
                        setNull(buffer, size);
                    }
                } else {
                    buffer[size++] = '[';
                    endOfDocument = false;
                    int i = 0;
                    while (!endOfDocument) {
                        if (i > 0) {
                            buffer[size++] = ',';
                        }
                        IndexManager::getFixedValue(model, cloc,i, v, isNull, endOfDocument, isDeleted,isArray);
                        i++;
                        if (!isNull) {
                            size += convertToString(v, mae, buffer + size);
                        } else {
                            setNull(buffer, size);
                        }
                    }
                    buffer[size++] = ']';
                }
            }
            k++;
        } else {
            IndexManager::getFixedValue(model, cloc, 0, v, isNull, endOfDocument, isDeleted,isArray);
            __uint128_t r;
            if (!isNull || o.function->getClassInstanceType() == ClassInstanceCount) {
                r = o.globalFuncResult;
            } else {
                r = 0;
            }
            mapping::mapping_entry me = im->getMapping();
            me.t = o.function->getResultType();
            me.scale = o.function->adjustScaling((int)me.scale);
            me.length = 128;
            size += convertToString(r, me, buffer + size);
        }
    }
    buffer[size++] = '}';
    buffer[size++] = '\n';

    return size;
}

void  *SelectModel::openPrintFile(uint32_t me,int &tmpFileFd,size_t &tmpFileSize) {
    string tmpFilePath = tmpFileRootPath+to_string(me)+".res";
    tmpFileFd = open(tmpFilePath.c_str(), O_RDWR| O_CREAT, (mode_t)0600);
    if (tmpFileFd < 0) {
        Log::error(queryStatus,"Failed to create result file "+tmpFilePath);
        return nullptr;
    }
    tmpFileSize = 2*fileExtendSize;
    lseek(tmpFileFd,(off_t)tmpFileSize,SEEK_CUR);
    if (write(tmpFileFd,"\0",1) < 0) {
        Log::error(queryStatus,"Failed to create result file (space?)"+tmpFilePath);
        close(tmpFileFd);
        return nullptr;
    }
    lseek(tmpFileFd,0,SEEK_SET);

    char *tmpFileData = (char*)mmap(nullptr,tmpFileSize , PROT_READ|PROT_WRITE, MAP_SHARED, tmpFileFd, 0);
    if (tmpFileData == MAP_FAILED) {
        Log::error(queryStatus,"Failed to mmap result file "+tmpFilePath + " error=" + to_string(errno));
        return nullptr;
    }
    return tmpFileData;
}

void SelectModel::execGetResultAsFileThreadNoOrder(SelectWorkerInitParam *o) {
    int tmpFileFd;
    size_t tmpFileSize;
    char *tmpFileData = (char*)openPrintFile(o->me,tmpFileFd,tmpFileSize);
    if (tmpFileData == nullptr) {
        return;
    }
    size_t tmpFilePosition = 0;
    auto ws = o->where->getWorkingSet();

    for (uint64_t i=o->start; i < o->start+o->workingSize; i++) {
        if (ws[i] != 0) {
            uint64_t a = ws[i];
            int j;
            while ((j=__builtin_ffsl((long)a)) > 0 && o->myLimit > 0) {
                if (o->toSkip > 0) {
                    o->toSkip --;
                } else {
                    tmpFilePosition += resultToJson(i*64 + j -1, tmpFileData + tmpFilePosition);
                    o->myLimit --;
                }
                a &= ~(1L<<(j-1));
            }

            if (tmpFilePosition + fileExtendSize > tmpFileSize) {
                tmpFileData = extendMapping(tmpFileFd,fileExtendSize,tmpFileData);
                if (tmpFileData == nullptr) {
                    Log::error(queryStatus,"Failed to extend mmap result file. Error=" + to_string(errno));
                    close(tmpFileFd);
                    return;
                }
                tmpFileSize += fileExtendSize;
            }
        }
    }
    tmpFileData[tmpFilePosition] = 0;
    msync(tmpFileData,tmpFileSize,MS_SYNC);
    munmap(tmpFileData,tmpFileSize);
    resultSize += tmpFilePosition;
    close(tmpFileFd);
}

void SelectModel::execGetResultAsFileOnlyGlobalFunction() {
    int tmpFileFd;
    size_t tmpFileSize;
    char *tmpFileData = (char*)openPrintFile(0,tmpFileFd,tmpFileSize);
    if (tmpFileData == nullptr) {
        return;
    }
    size_t tmpFilePosition = 0;
    tmpFilePosition += resultToJson(0, tmpFileData + tmpFilePosition);
    tmpFileData[tmpFilePosition] = 0;
    msync(tmpFileData,tmpFileSize,MS_SYNC);
    munmap(tmpFileData,tmpFileSize);
    resultSize += tmpFilePosition;
    close(tmpFileFd);
}

void SelectModel::execGetResultAsFileThreadOrdered(const SelectWorkerInitParam *o) {
    int tmpFileFd;
    size_t tmpFileSize;
    char *tmpFileData = (char*)openPrintFile(o->me,tmpFileFd,tmpFileSize);
    if (tmpFileData == nullptr) {
        return;
    }

    size_t tmpFilePosition = 0;
    auto ws = (uint32_t*)order->getSortDataRaw();

    for (uint64_t i=o->start; i < o->start+o->workingSize ; i++) {
        tmpFilePosition += resultToJson(ws[i], tmpFileData + tmpFilePosition);

        if (tmpFilePosition + fileExtendSize > tmpFileSize) {
            tmpFileData = extendMapping(tmpFileFd,fileExtendSize,tmpFileData);
            if (tmpFileData == nullptr) {
                Log::error(queryStatus,"Failed to extend mmap result file. Error=" + to_string(errno));
                close(tmpFileFd);
                return;
            }
            tmpFileSize += fileExtendSize;
        }
    }
    tmpFileData[tmpFilePosition] = 0;
    msync(tmpFileData,tmpFileSize,MS_SYNC);
    munmap(tmpFileData,tmpFileSize);
    resultSize += tmpFilePosition;
    close(tmpFileFd);
}

void SelectModel::processGlobalFunctions(shared_ptr<WhereModel> &where) {
    hasGlobalFunc = false;
    onlyHasGlobalFunc = true;
    for (auto &p : fieldsToReturn) {
        if (p.function != nullptr) {
            switch (p.function->getClassInstanceType()) {
                case ClassInstanceCount:
                    static_pointer_cast<CountModel>(p.function)->initCount(where);
                    p.globalFuncResult = static_pointer_cast<CountModel>(p.function)->getCount();
                    hasGlobalFunc = true;
                    break;
                case ClassInstanceMin:
                case ClassInstanceMax:
                case ClassInstanceSum:
                case ClassInstanceAvg: {
                    bool hasOrder = (order != nullptr) && (order->getSortDataRaw() != nullptr);
                    hasGlobalFunc = true;
                    uint32_t m = hasOrder ? where->getNbResults() : where->getWorkingSetSize() / 8;
                    auto processor_count = min(thread::hardware_concurrency(), (unsigned int) m);
                    if (processor_count == 0) {
                        processor_count = 1;
                    }
                    vector<pthread_t> threads;
                    vector<SelectWorkerInitParam> w;
                    tmpResults.resize(processor_count);
                    tmpResultsCount.resize(processor_count);
                    unsigned int ip = m / processor_count;
                    for (int i = 0; i < processor_count; i++) {
                        SelectWorkerInitParam e;
                        e.t = this;
                        e.me = i;
                        e.start = i * ip;
                        e.workingSize = i < processor_count - 1 ? ip : m - i * ip;
                        e.where = where.get();
                        e.fieldToReturn = p;
                        w.push_back(e);
                    }
                    for (int i = 0; i < processor_count; i++) {
                        pthread_t t;
                        if (hasOrder) {
                            pthread_create(&t, nullptr, InternalThreadEntryGlobalFuncOrdered, &w.at(i));
                        } else {
                            pthread_create(&t, nullptr, InternalThreadEntryGlobalFuncNoOrder, &w.at(i));
                        }
                        threads.push_back(t);
                    }
                    for (auto &i : threads) {
                        void *status;
                        pthread_join(i, &status);
                    }
                    threads.clear();
                    w.clear();
                    shared_ptr<RootModel> gf = static_pointer_cast<RootModel>(p.function);
                    uint64_t k = 0;
                    for (auto &i:tmpResultsCount) {
                        k+= i;
                    }
                    p.globalFuncResult = gf->getFuncResult(tmpResults, k);
                }
                default:
                    break;
            }
        } else {
            onlyHasGlobalFunc = false;
        }
    }
}

string SelectModel::getResultAsFileThread(shared_ptr<WhereModel> &where,std::shared_ptr<QueryStatus> &_queryStatus, int &nbFiles,uint64_t &_resultSize, uint64_t _offset, uint64_t _limit) {
    limit = _limit;
    offset = _offset;
    queryStatus = _queryStatus;
    bool hasOrder = (order != nullptr) && (order->getSortDataRaw() != nullptr);

    unsigned char tmpBuffer[64];
    ssize_t o = getrandom(tmpBuffer, 64, 0);
    char tmpFileName[128];
    for (int i = 0; i < o; i++) {
        sprintf(tmpFileName + 2 * i, "%02x", tmpBuffer[i]);
    }

    tmpFileRootPath = Config::getInstance()->getResultTempPath() + "/" + tmpFileName + "_";
    ca.indexManagerVector.resize(fieldsToReturn.size());
    ca.mappingVector.resize(fieldsToReturn.size());
    int k = 0;
    for (const auto &z : fieldsToReturn) {
        ca.indexManagerVector.at(k) = IndexRepository::getInstance()->getIndex(columnMapped->getIndexName(), z.field).get();
        if (ca.indexManagerVector.at(k) == nullptr) {
            Log::error(queryStatus,"Invalid column selected : " + z.field);
            return tmpFileRootPath;
        }
        ca.mappingVector.at(k) = ca.indexManagerVector.at(k)->getMapping();
        k++;
    }
    processGlobalFunctions(where);

    auto nbResults = min(limit,where->getNbResults());
    if (hasGlobalFunc) nbResults = max(1UL,nbResults);
    if (offset > where->getNbResults()) {
        Log::error(queryStatus,"Offset is greater than number of query results");
        return  tmpFileRootPath;
    }
    auto processor_count = min(nbResults/minLinesPerResultFile,(uint64_t)thread::hardware_concurrency());
    if (nbResults > 0 && processor_count==0) {
        processor_count = 1;
    }
    nbFiles = (int)processor_count;
    resultSize = 0;
    if (processor_count > 0) {
        vector<pthread_t> threads;
        vector<SelectWorkerInitParam> w;
        if (onlyHasGlobalFunc) {
            execGetResultAsFileOnlyGlobalFunction();
        } else if (!hasOrder) {
            if (offset != 0) {
                // make things easier
                processor_count = 1;
                nbFiles = (int)processor_count;
            }
            uint64_t m = 0;
            uint64_t p = 0;
            uint64_t itemsToScan = (nbResults==1 && hasGlobalFunc) ? 1 : where->getWorkingSetSize() * 8;
            uint64_t itemsToPrint = (nbResults==1 && hasGlobalFunc) ? 1 : nbResults;
            auto nbResultsPerProc = itemsToPrint / processor_count;
            // Skip begin
            uint64_t b = 0;
            if (offset > 64) {
                while (b < offset-64 && p<itemsToScan/64) {
                    b += __builtin_popcountl(where->getWorkingSet()[p]);
                    p++;
                }
            }
            m = p;
            uint64_t offset2 = offset - b;

            for (int i = 0; i < processor_count; i++) {
                if (i == processor_count - 1) {
                    p = itemsToScan/64;
                    b = itemsToPrint;
                } else {
                    b = 0;
                    while (b < nbResultsPerProc) {
                        b += __builtin_popcountl(where->getWorkingSet()[p]);
                        p++;
                    }
                }
                itemsToPrint -= b;

                SelectWorkerInitParam e;
                e.t = this;
                e.where = where.get();
                e.order = nullptr;
                e.me = i;
                e.start = m;
                e.workingSize = p - m;
                e.toSkip = i == 0 ? offset2 : 0;
                e.myLimit = b;
                w.push_back(e);
                m = p;
            }

            for (int i = 0; i < processor_count; i++) {
                pthread_t t;
                pthread_create(&t, nullptr, SelectModel::InternalThreadEntryFuncNoOrder, &w.at(i));
                threads.push_back(t);
            }
            for (auto &i : threads) {
                void *status;
                pthread_join(i, &status);
            }
            threads.clear();
            w.clear();
        } else {
            auto nbResultsPerProc = nbResults / processor_count;
            for (int i = 0; i < processor_count; i++) {
                SelectWorkerInitParam e;
                e.t = this;
                e.where = where.get();
                e.order = order.get();
                e.me = i;
                e.start = offset+i*nbResultsPerProc;
                e.workingSize = i < processor_count -1 ? nbResultsPerProc : nbResults-i*nbResultsPerProc;
                w.push_back(e);
            }
            for (int i = 0; i < processor_count; i++) {
                pthread_t t;
                pthread_create(&t, nullptr, SelectModel::InternalThreadEntryFuncOrdered, &w.at(i));
                threads.push_back(t);
            }
            for (auto &i : threads) {
                void *status;
                pthread_join(i, &status);
            }
            threads.clear();
            w.clear();
        }
    }
    _resultSize = resultSize;
    if (resultSize == 0) {
        queryStatus->status = HTTP_204;
        queryStatus->customMessage = "{}";
    }
    return tmpFileRootPath;
}

void SelectModel::execGlobalFuncNoOrder(const WhereModel *where, uint32_t me, uint64_t start, uint64_t len,const FieldAndFunction &fieldToReturn) {
    const auto ws = where->getWorkingSet();
    const IndexManager *im = IndexRepository::getInstance()->getIndex(columnMapped->getIndexName(), fieldToReturn.field).get();
    shared_ptr<RootModel> gf = static_pointer_cast<RootModel>(fieldToReturn.function);
    gf->initFunc(im->getIndexType());
    gf->setLength(im->getLength());
    auto mae = im->getMapping();
    bool isArray = (mae.flags & EntryArraySet) != 0;
    __uint128_t v;
    bool isNull;
    bool endOfDocument;
    bool isDeleted;
    __uint128_t r = gf->getGlobalFuncStartVal();
    uint64_t c = 0;
    uint32_t lastChunk = -1;
    std::shared_ptr<StorageModel> model;
    for (uint64_t i = start; i < start+len; i++) {
        if (ws[i] != 0) {
            uint64_t a = ws[i];
            int j;
            while ((j = __builtin_ffsl((long) a)) > 0) {
                uint64_t loc = i * 64 + j - 1;
                uint32_t chunk = loc >> CHUNK_SIZE_LOG2;
                if (chunk != lastChunk) {
                    model = im->getFixedStorageModel(chunk,isArray);
                    lastChunk = chunk;
                }
                loc -= chunk << CHUNK_SIZE_LOG2;
                auto cloc = (uint32_t) loc;
                IndexManager::getFixedValue(model, cloc, 0,v, isNull, endOfDocument,isDeleted, isArray);
                if (!isNull) {
                    r = gf->processGlobalFunc(v,r);
                    c++;
                }
                a &= ~(1L << (j - 1));
            }
        }
    }
    tmpResults.at(me) = r;
    tmpResultsCount.at(me) = c;
}


void SelectModel::execGlobalFuncNoOrdered(uint32_t me, uint64_t start, uint64_t workingSize,const FieldAndFunction &fieldToReturn) {
    auto ws = (uint32_t*)order->getSortDataRaw();
    const IndexManager *im = IndexRepository::getInstance()->getIndex(columnMapped->getIndexName(), fieldToReturn.field).get();
    shared_ptr<RootModel> gf = static_pointer_cast<RootModel>(fieldToReturn.function);
    gf->initFunc(im->getIndexType());
    gf->setLength(im->getLength());
    auto mae = im->getMapping();
    bool isArray = (mae.flags & EntryArraySet) != 0;
    __uint128_t v;
    bool isNull;
    bool endOfDocument;
    bool isDeleted;
    __uint128_t r = gf->getGlobalFuncStartVal();
    tmpResultsCount.at(me) = 0;
    uint32_t lastChunk = -1;
    std::shared_ptr<StorageModel> model;
    for (uint64_t i = start; i < start+workingSize; i++) {
        uint64_t loc = ws[i];
        uint32_t chunk = loc >> CHUNK_SIZE_LOG2;
        if (chunk != lastChunk) {
            model = im->getFixedStorageModel(chunk,isArray);
            lastChunk = chunk;
        }
        loc -= chunk << CHUNK_SIZE_LOG2;
        auto cloc = (uint32_t) loc;
        IndexManager::getFixedValue(model, cloc, 0,v, isNull, endOfDocument,isDeleted, isArray);
        if (!isNull) {
            r = gf->processGlobalFunc(v,r);
            tmpResultsCount.at(me) ++;
        }
    }
    tmpResults.at(me) = r;
}

}