#include <map>
#include "datajoin.hpp"
#include "storageModel.hpp"
#include "../dictionary/crc32c.hpp"
#include "../mapping/indexDefinitionManager.hpp"

using namespace std;
using namespace dictionary;
using namespace mapping;

namespace indexes {

DataJoinFactory *DataJoinFactory::instance = nullptr;
std::map<std::string,std::shared_ptr<DataJoin> > DataJoinFactory::joinMap;
std::mutex	DataJoinFactory::mu;

inline string getKey(const std::string &joinIndex, const std::string &joinRemoteKey, const std::string &joinValue) {
    return joinIndex+"_"+joinRemoteKey+"_"+joinValue;
}

shared_ptr<DataJoin> DataJoinFactory::getInstance(const std::string &joinIndex, const std::string &joinRemoteKey, const std::string &joinValue,std::shared_ptr<queries::QueryStatus> queryStatus) {
    shared_ptr<DataJoin> ret;
    mu.lock();
    auto k = getKey(joinIndex,joinRemoteKey,joinValue);
    if (joinMap.find(k) != joinMap.end()) {
        ret = joinMap.find(k)->second;
    } else {
        shared_ptr<mapping::IndexDefinition> idRemote = IndexDefinitionManager::getInstance()->getIndex(joinIndex);
        if (idRemote == nullptr) {
            Log::error(queryStatus,"DataImporter : invalid index :"+joinIndex);
            return nullptr;
        }
        IndexManager *keyRemote = idRemote->getIndexPtr(joinRemoteKey);
        if (keyRemote == nullptr) {
            Log::error(queryStatus,"DataImporter : invalid join key :"+joinRemoteKey);
            return nullptr;
        }

        IndexManager *dataRemote = idRemote->getIndexPtr(joinValue);
        if (dataRemote == nullptr) {
            Log::error(queryStatus,"DataImporter : invalid join value :"+joinValue);
            return nullptr;
        }
        ret = make_shared<DataJoin>(keyRemote,dataRemote);
        joinMap[k] = ret;
    }
    mu.unlock();
    return ret;
}

void DataJoinFactory::free() {
    mu.lock();
    joinMap.clear();
    mu.unlock();
}

DataJoin::DataJoin(const IndexManager *_keym, const IndexManager *_datam) {
    keym = _keym;
    datam = _datam;
    boo = nullptr;
    lastChunk = 0;
    init(keym->getStorageType() == mapping::storage_type_enum);
}

DataJoin::~DataJoin() {
    free(boo);
}

inline uint64_t getHash(const string_view &o) {
    return AquaHash::Hash((uint8_t*)o.data(),o.size(),_mm_set_epi64x(0,0))[0];
}

void  DataJoin::init(bool isEnum) {
    map<uint64_t,bool>keys;
    bool isNull;
    bool endOfDocument;
    bool isDeleted;
    __uint128_t v;

    // get all keys
    for (int chunk=0; chunk < keym->getNumberOfChunks(); chunk++) {
        shared_ptr<IndexChunk> ck = keym->getChunkForRead(chunk);
        if (isEnum) {
            shared_ptr<Dictionary> dic = ck->getDictionary();
            for (int j = 0; j < dic->getNbEntries(); j++) {
                keys[getHash(dic->getString(j))] = true;
            }
        } else {
            for (int j=0; j < ck->getBase()->getNbDocuments(); j++) {
                ck->getBase()->getValue(j,v,isNull,endOfDocument,isDeleted);
                keys[(uint64_t)v] = true;
            }
        }
    }

    vector<uint64_t> m;
    m.resize(keys.size());
    int i = 0;
    for (const auto &o : keys) {
        m.at(i)=o.first;
        i++;
    }

    // free memory
    keys.clear();

    boo = new boomphf::mphf<uint64_t,Custom_Hasher>(m.size(), m, 1, 100);
    valueIndex.resize(m.size());

    for (int chunk=0; chunk < keym->getNumberOfChunks(); chunk++) {
        shared_ptr<IndexChunk> ck = keym->getChunkForRead(chunk);
        if (isEnum) {
            shared_ptr<Dictionary> dic = ck->getDictionary();
            for (int j=0; j < ck->getBase()->getNbDocuments(); j++) {
                ck->getBase()->getValue(j,v,isNull,endOfDocument,isDeleted);
                auto s = dic->getString(v);
                uint64_t p = boo->lookup(getHash(s));
                if (p != ULLONG_MAX) {
                    valueIndex.at(p) = chunk<<CHUNK_SIZE_LOG2 | j;
                } else {
                    Log::debug("Fixme: boo MPH conflict.....");
                }
            }
        } else {
            for (int j=0; j < ck->getBase()->getNbDocuments(); j++) {
                ck->getBase()->getValue(j,v,isNull,endOfDocument,isDeleted);
                uint64_t p = boo->lookup((uint64_t)v);
                if (p != ULLONG_MAX) {
                    valueIndex.at(p) = chunk<<CHUNK_SIZE_LOG2 | j;
                } else {
                    Log::debug("Fixme: boo MPH conflict.....");
                }
            }
        }
    }
    lastChunk = 0;
    lastChunkIndex = datam->getChunkForRead(0)->getSharedBase();
    lastDic = datam->getChunkForRead(0)->getDictionary();
}

__uint128_t DataJoin::getValue(const std::string_view &s,bool &isNull,bool &endOfDocument,bool &isDeleted,shared_ptr<queries::QueryStatus> &queryStatus) {
    __uint128_t v=0;
    uint64_t p = boo->lookup(getHash(s));
    if (p != ULLONG_MAX) {
        uint64_t h = valueIndex.at(p);
        uint32_t chunk = h>>CHUNK_SIZE_LOG2;
        uint32_t loc = h &(CHUNK_SIZE-1);
        if (lastChunk != chunk) {
            lastChunkIndex = datam->getChunkForRead(chunk)->getSharedBase();
            lastChunk = chunk;
        }
        lastChunkIndex->getValue(loc,v,isNull,endOfDocument,isDeleted);
    } else {
        Log::warn(string("Data join failed. Key not found in the joined table. Value=")+string(s.data(),s.size()));
        queryStatus->addWarnings(1);
        isNull = true;
    }
    return v;
}

__uint128_t DataJoin::getValue(const __uint128_t &s,bool &isNull,bool &endOfDocument,bool &isDeleted,shared_ptr<queries::QueryStatus> &queryStatus) {
    __uint128_t v=0;
    uint64_t p = boo->lookup((uint64_t)s);
    if (p != ULLONG_MAX) {
        uint64_t h = valueIndex.at(p);
        uint32_t chunk = h>>CHUNK_SIZE_LOG2;
        uint32_t loc = h &(CHUNK_SIZE-1);
        if (lastChunk != chunk) {
            lastChunkIndex = datam->getChunkForRead(chunk)->getSharedBase();
            lastChunk = chunk;
        }
        lastChunkIndex->getValue(loc,v,isNull,endOfDocument,isDeleted);
    } else {
        Log::warn(string("Data join failed. Key not found in the joined table. ")+to_string((uint64_t)s));
        queryStatus->addWarnings(1);
        isNull = true;
    }
    return v;
}

string_view DataJoin::getStringValue(const std::string_view &s,bool &isNull,bool &endOfDocument,bool &isDeleted,shared_ptr<queries::QueryStatus> &queryStatus) {
    __uint128_t v;
    uint64_t p = boo->lookup(getHash(s));
    if (p != ULLONG_MAX) {
        uint64_t h = valueIndex.at(p);
        uint32_t chunk = h>>CHUNK_SIZE_LOG2;
        uint32_t loc = h &(CHUNK_SIZE-1);
        if (lastChunk != chunk) {
            lastChunkIndex = datam->getChunkForRead(chunk)->getSharedBase();
            lastDic = datam->getChunkForRead(chunk)->getDictionary();
            lastChunk = chunk;
        }
        lastChunkIndex->getValue(loc,v,isNull,endOfDocument,isDeleted);
        return lastDic->getString((uint64_t)v);
    } else {
        Log::warn(string("Data join failed. Key not found in the joined table. ")+string(s.data(),s.size()));
        queryStatus->addWarnings(1);
        isNull = true;
        return {};
    }
}

string_view DataJoin::getStringValue(const __uint128_t &s,bool &isNull,bool &endOfDocument,bool &isDeleted,shared_ptr<queries::QueryStatus> &queryStatus) {
    __uint128_t v;
    uint64_t p = boo->lookup((uint64_t)s);
    if (p != ULLONG_MAX) {
        uint64_t h = valueIndex.at(p);
        uint32_t chunk = h>>CHUNK_SIZE_LOG2;
        uint32_t loc = h &(CHUNK_SIZE-1);
        if (lastChunk != chunk) {
            lastChunkIndex = datam->getChunkForRead(chunk)->getSharedBase();
            lastDic = datam->getChunkForRead(chunk)->getDictionary();
            lastChunk = chunk;
        }
        lastChunkIndex->getValue(loc,v,isNull,endOfDocument,isDeleted);
        return lastDic->getString((uint64_t)v);
    } else {
        Log::warn(string("Data join failed. Key not found in the joined table. ")+to_string((uint64_t)s));
        queryStatus->addWarnings(1);
        isNull = true;
        return {};
    }
}

}
