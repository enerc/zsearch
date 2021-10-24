#include <memory>
#include <mutex>
#include "indexRepository.hpp"
#include "dataImporter.hpp"
#include "../mapping/indexDefinitionManager.hpp"
#include "storageModel.hpp"


using namespace std;
using namespace simdjson;
using namespace mapping;

constexpr const char *dot = ".";

namespace indexes {

IndexRepository* IndexRepository::instance = nullptr;

IndexRepository* IndexRepository::getInstance() {
	if (instance == nullptr)  {
		instance = new IndexRepository();
	}
	return instance;
}

string IndexRepository::getKey(const string &_index,const string &_field) {
	return _index + string(dot) + _field;
}

static string splitIndex(const string &u) {
    if (u.find(dot) != string::npos) {
        return u.substr(0,u.find(dot));
    } else {
        return {};
    }
}

shared_ptr<IndexManager> IndexRepository::getIndex(const string &_index,const string &key) {
	shared_ptr<IndexManager> ret = nullptr;
	auto hash = key;
	auto s = getKey(_index,hash);
	mu.lock();
	auto o = indexMap.find(s);
	if (o != indexMap.end()) {
		ret = o->second;
	}
	mu.unlock();
	if (ret == nullptr) {
		shared_ptr<IndexDefinition> id = IndexDefinitionManager::getInstance()->getIndex(_index);
		if (id != nullptr) {
            const mapping_entry me = id->getEntryNoBoo(key);
            if (me.s != storage_type_unknown) {
                ret = make_shared<IndexManager>(_index, hash, key, me);
                mu.lock();
                indexMap.emplace(s, ret);
                mu.unlock();
            }
        }
	}
	return ret;
}

shared_ptr<IndexManager> IndexRepository::getIndex(const string &_index,const string &key,const mapping::mapping_entry &me) {
	shared_ptr<IndexManager> ret;
	string hash = key;
	auto s = getKey(_index,hash);
	mu.lock();
	auto o = indexMap.find(s);
	if (o != indexMap.end()) {
		ret = o->second;
	} else {
		ret = make_shared<IndexManager>(_index,hash,key,me);
		indexMap.emplace(s,ret);
	}
	mu.unlock();
	return ret;
}

vector<shared_ptr<IndexChunk>> IndexRepository::getChunksForWrite(const string &index, uint32_t &capacity) {
	shared_ptr<IndexDefinition> id = IndexDefinitionManager::getInstance()->getIndex(index);
	auto m = id->getIndexFields();
	vector<shared_ptr<IndexChunk>> ret;
	ret.resize(m.size());

	// lock so all chunks are the same chunkId (all threads grow one after the other)
	mu.lock();
	for (const auto &i : m) {
		uint64_t idx = id->getIndexOffset(i);
		IndexManager *im = id->getIndexPtr(i);
		if (im == nullptr) {
		    Log::error("Index "+string(i.data(),i.size())+ " not found !");
		    ret.resize(0);
		    return ret;
		}
		ret[idx] = im->lockChunkForWrite();
        capacity = ret[idx]->getBase()->getNbDocuments();	// should be same for all
	}
	mu.unlock();
	return ret;
}

void IndexRepository::adjustStorageSize(const string &index) {
    mu.lock();
    shared_ptr<IndexDefinition> id = IndexDefinitionManager::getInstance()->getIndex(index);
    auto m = id->getIndexFields();
    for (const auto &i : m) {
        id->getIndexPtr(i)->adjustStorageSize();
    }
    mu.unlock();
}

void IndexRepository::releaseLockedChunks(const string &index, std::vector<std::shared_ptr<indexes::IndexChunk>> &c) {
	shared_ptr<IndexDefinition> id = IndexDefinitionManager::getInstance()->getIndex(index);
	auto m = id->getIndexFields();
	mu.lock();
	for (const auto &i : m) {
		uint64_t idx = id->getIndexOffset(i);
		IndexManager *im = id->getIndexPtr(i);
		c[idx]->flush();    // flush min/max
		im->releaseChunk(c[idx]);
	}
	mu.unlock();
}

void IndexRepository::removeIndex(const string &index,const string &hash) {
	auto s = getKey(index,hash);
	mu.lock();
	auto o = indexMap.find(s);
	if (o != indexMap.end()) {
		o->second->removeWholeIndex();
		indexMap.erase(o);
	}
	mu.unlock();
}

void IndexRepository::ingest(const string &index,const string_view &lines,shared_ptr<queries::QueryStatus> queryStatus) {
	DataImporter d(index,lines,queryStatus);
}

void IndexRepository::sync() {
	for (const auto &i : indexMap) {
		i.second->sync();
	}
}

[[maybe_unused]]std::shared_ptr<IndexManager> IndexRepository::getIntegerIndex() {
    mapping_entry me;
    me.s = mapping::storage_type_signed;
    shared_ptr<IndexManager> p = make_shared<IndexManager>(me);
    return p;
}

[[maybe_unused]] std::shared_ptr<IndexManager> IndexRepository::getDoubleIndex() {
    mapping_entry me;
    me.s = mapping::storage_type_double;
    shared_ptr<IndexManager> p = make_shared<IndexManager>(me);
    return p;
}

void IndexRepository::deleteDocuments(const std::string &index, const uint64_t *data, uint64_t count) {
    shared_ptr<IndexDefinition> id = IndexDefinitionManager::getInstance()->getIndex(index);
    auto m = id->getIndexFields();
    vector<shared_ptr<IndexChunk>> myChunks;
    myChunks.resize(m.size());
    for (auto &n : myChunks) {
        n = nullptr;
    }

    int lastChunkId = -1;
    for (int i=0; i < count/8; i++) {
        uint64_t l = data[i];
        if (l == 0) continue;
        for (int k = 0; k < 64; k++) {
            if (_bextr_u64(l,k,1) == 0) continue;
            int c = (i*64)>>CHUNK_SIZE_LOG2;
            if (c != lastChunkId) {
                int h = 0;
                for (auto &n : m) {
                    getIndex(index,n);
                    if (myChunks[h] != nullptr) {
                        indexMap.at(getKey(index,n))->releaseChunk(myChunks[h]);
                        myChunks[h] = nullptr;
                    }
                    myChunks[h] = indexMap.at(getKey(index,n))->getChunkForWrite(c);
                    h++;
                }
                lastChunkId = c;
            }
            for (auto &d : myChunks) {
                d->getBase()->setDeleted(i * 64 + k - (c<<CHUNK_SIZE_LOG2), true);
            }
        }
    }
    int h = 0;
    for (auto &n : m) {
        if (myChunks[h] != nullptr) {
            indexMap.at(getKey(index,n))->releaseChunk(myChunks[h]);
        }
        h++;
    }
}

uint32_t IndexRepository::getNumberOfChunks(const std::string &_index) const {
    for (const auto &i : indexMap) {
        if (splitIndex(i.first) == _index) {
            return i.second->getNumberOfChunks();
        }
    }
    return 0;
}

string IndexRepository::getOneMember(const string &_index) const {
    for (const auto &i : indexMap) {
        if (splitIndex(i.first) == _index) {
            return i.first.substr(i.first.find(dot)+1);
        }
    }
    return {};
}

}
