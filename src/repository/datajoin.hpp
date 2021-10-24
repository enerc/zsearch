#pragma once
#include "../common/avx_conf.h"
#include "indexManager.hpp"
#include "../common/booPHF.hpp"
#include "../common/simdjson.h"

namespace indexes {

class Custom_Hasher
{
public:
    // the class should have operator () with this signature :
    uint64_t operator ()   (uint64_t a, uint64_t seed=0) const {
        (void)seed;
        return a;
    }
};
typedef boomphf::mphf<uint64_t, Custom_Hasher  > boophf_t;

class DataJoin {
public:
    DataJoin(const IndexManager *keym, const IndexManager *datam);
    ~DataJoin();
    __uint128_t getValue(const std::string_view &s,bool &isNull,bool &endOfDocument,bool &isDeleted,std::shared_ptr<queries::QueryStatus> &queryStatus);
    __uint128_t getValue(const __uint128_t &s,bool &isNull,bool &endOfDocument,bool &isDeleted,std::shared_ptr<queries::QueryStatus> &queryStatus);
    std::string_view getStringValue(const std::string_view &s,bool &isNull,bool &endOfDocument,bool &isDeleted,std::shared_ptr<queries::QueryStatus> &queryStatus);
    std::string_view getStringValue(const __uint128_t &s,bool &isNull,bool &endOfDocument,bool &isDeleted,std::shared_ptr<queries::QueryStatus> &queryStatus);
private:
    void  init(bool isEnum);


    const IndexManager* keym;
    const IndexManager* datam;
    boophf_t *boo;
    std::vector<uint64_t>valueIndex;
    uint32_t lastChunk;
    std::shared_ptr<BaseIndex> lastChunkIndex;
    std::shared_ptr<dictionary::Dictionary> lastDic;
};

class DataJoinFactory {
public:
    static std::shared_ptr<DataJoin> getInstance(const std::string &joinIndex, const std::string &joinRemoteKey, const std::string &joinValue,std::shared_ptr<queries::QueryStatus> queryStatus);
    static void free();
private:
    static DataJoinFactory *instance;
    static std::map<std::string,std::shared_ptr<DataJoin> > joinMap;
    static std::mutex	mu;
};

}