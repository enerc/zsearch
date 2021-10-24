#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <iomanip>
#include <filesystem>
#include <cmath>

#include "../../config.h"
#include "indexManager.hpp"
#include "fixedIndex.hpp"
#include "variableIndex.hpp"
#include "../dictionary/dictionaryAllocator.hpp"
#include "baseIndex.hpp"
#include "indexChunk.hpp"
#include "storageModel.hpp"

using namespace std;
using namespace dictionary;
using namespace mapping;

namespace indexes {

IndexManager::IndexManager(const string_view &_index,const string_view &_hash, const string_view &_key, const mapping_entry &_me) {
	index = _index;
	key = _key;
	hash = _hash;
	me = _me;
	scale = pow(10,me.scale);
	if (me.length > 128) Log::panic("IndexManager: length > 128: "+to_string(me.length) + " index:" + string(_index.data(),_index.size()) + " field:" + string(_key.data(),_key.size()));
    isFixed = me.s != mapping::storage_type_enum;
    isaConstant = false;
    calcIndexSize();

    dicManager = nullptr;
	if ((me.t == mapping::index_type_enum) || (me.t == mapping::index_type_string)) {
		dicManager = DictionaryAllocator::getInstance()->getManager(index,hash,me.max_array_size);
	}

	// open files
	bool created = false;
	fdIndex =  open(getIndexFileName().c_str(), O_RDWR, (mode_t)0600);
	if (fdIndex < 0) {
		createFile();
		created = true;
		fdIndex =  open(getIndexFileName().c_str(), O_RDWR, (mode_t)0600);
		if (fdIndex < 0) {
			Log::panic("Cannot open file "+getIndexFileName());
		}
	}

	// Map files
	storage = (IndexStructure*)mmap(nullptr, getIndexFileSize(), PROT_READ|PROT_WRITE, MAP_SHARED, fdIndex, 0);
	if (storage == MAP_FAILED) {
		Log::panic("Failed to map "+getIndexFileName()+" size "+to_string(getIndexFileSize()) + " error=" + to_string(errno));
	}

	if (created) {
		Log::debug("Index " + getIndexFileName() + " created");
        storage->allocatedChunks = 2*chunkIncreaseSize;
	}

    storageModel = StorageModelFactory::getModel(nullptr,nullptr,me.length);
}

IndexManager::IndexManager(const mapping::mapping_entry &me) {
    fdIndex = -1;
    this->me = me;
    isFixed = true;
    storage = nullptr;
    scale = 0;
    indexSize = 0;
    isaConstant = true;
}

IndexManager::~IndexManager() {
	sync();
    if (storage != nullptr) {
        munmap(storage, getIndexFileSize());
    }
	close(fdIndex);
}

void IndexManager::calcIndexSize() {
    // If fixed -> length + isNull + endOfDocument
    // if enum :  CHUNK_SIZE_LOG2+sizeChanged+isNull + endOfDocument
    size_t n;
    if (isFixed) n = CHUNK_SIZE*StorageModelFactory::getModelSize(me.length) * me.max_array_size;
    else n = CHUNK_SIZE*(CHUNK_SIZE_LOG2+1+1) * me.max_array_size / 8 + 1;
    n += me.max_array_size > 1 ? sizeof(IndexChunkStructArray) : sizeof(IndexChunkStructSingle);
    n += sizeof(IndexChunkStructBase);
    indexSize = (n+4095) & (~4095);
}

uint64_t IndexManager::getChunksBaseOffset() {
	return (sizeof(IndexStructure)+4095) & (~4095);
}

shared_ptr<BaseIndex> IndexManager::makeIndex(IndexChunkStructBase *a,storage_types _storageType, shared_ptr<Dictionary> &dic) const {
    bool isArray = (me.flags & EntryArraySet) != 0;
	if (isFixed) {
		shared_ptr<FixedIndex> b(new FixedIndex(a,_storageType,me.length,isArray));
		return b;
	} else {
		shared_ptr<VariableIndex> b(new VariableIndex(a,dic,isArray));
		b->setLength(1);
		return b;
	}
}

void IndexManager::sync() {
	msync(storage,getIndexFileSize(),MS_SYNC);
	if (dicManager != nullptr) {
        dicManager->sync();
	}
}

string IndexManager::getFileFolder() const {
	return  Config::getInstance()->getIndexPath() + "/" +  index + ".";
}

string IndexManager::getIndexFileName() const{
	string s = getFileFolder() + hash + ".idx";
	return s;
}

size_t	IndexManager::getIndexInitialFileSize() const {
	return getChunksBaseOffset() + 2*chunkIncreaseSize * indexSize;
}

size_t	IndexManager::getIndexFileSize() const {
    return getMapSize(fdIndex);
}

// Create index sparse file
void IndexManager::createFile() {
	string h = getIndexFileName();
	fdIndex = open(h.c_str(), O_WRONLY | O_CREAT, (mode_t)0600);
	if (fdIndex < 0) {
		Log::error("Failed to create file "+h);
	}
	size_t fileSize = getIndexInitialFileSize();
	lseek(fdIndex,(off_t)fileSize,SEEK_CUR);
	char a = 0;
	if (write(fdIndex,&a,1) < 0) {
		Log::error("fail to create "+h);
	}
	close(fdIndex);
}

uint64_t IndexManager::getNumberOfChunks() const {
	if (storage == nullptr) {
		return 0;
	} else {
		return storage->upperChunks+1;
	}
}

shared_ptr<IndexChunk> IndexManager::getChunkForRead(uint32_t chunk) const {
	uint64_t dataOffset = getChunksBaseOffset() + chunk * indexSize;
	auto *a = (IndexChunkStructBase*)( ((char*)storage)+dataOffset);
	std::shared_ptr<Dictionary> dic = nullptr;
	if (dicManager != nullptr) {
		dic = dicManager->getDictionary(chunk);
	}
	shared_ptr<BaseIndex> b = makeIndex(a,me.s,dic);
	return make_shared<IndexChunk>(b,dic,a,storage,chunk,scale);
}

void *IndexManager::getData(uint32_t chunk) const {
    uint64_t dataOffset = getChunksBaseOffset() + chunk * indexSize;
    auto a = ((char*)storage)+dataOffset;
    return BaseIndex::buildYmmDataPtr((void*)a,me.max_array_size>1);
}

void *IndexManager::getDeletedPtr(uint32_t chunk) const {
    uint64_t dataOffset = getChunksBaseOffset() + chunk * indexSize;
    auto a = (IndexChunkStructBase*)(((char*)storage)+dataOffset);
    return (void*)a->isDeleted;
}

shared_ptr<IndexChunk> IndexManager::getChunkForWrite(uint32_t chunk) const{
 	shared_ptr<IndexChunk> r =  getChunkForRead(chunk);
	r->setAquiredForWriting(true);
	return r;
}

// thread safe enforced in Repository manager
shared_ptr<IndexChunk> IndexManager::lockChunkForWrite() {
	constexpr uint64_t mask = 0x5555555555555555L;
	for (int i=0; i < storage->allocatedChunks; i++ ) {
		if (((i%32) == 0) && (__builtin_popcountl(getState64(i) & mask) == 32)) {
			i+= 32;
			continue;
		}
		uint32_t s = getState(i);
		if  ( s == ChunkStateEmpty || s == ChunkStatePartial) {
			if (!isLocked(i)) {
				setLocked(i,true);
				shared_ptr<IndexChunk> ret = getChunkForWrite(i);
				if (s == ChunkStateEmpty ) {
					setState(i,ChunkStatePartial);
					if (i > storage->upperChunks) {
						storage->upperChunks = i;
					}
				}
				return ret;
			}
		}
	}
	Log::error("Max index size reached on " + index + "." + key);
	return nullptr;
}

void IndexManager::releaseChunk(shared_ptr<IndexChunk> &a) {
	uint32_t chunk = a->getChunk();
	checkChunkFull(a);
    checkChunkEmpty(a);
	setLocked(chunk,false);
	if (a->isAquiredForWriting()) {
		setStaled(chunk,true);
	}
}

void IndexManager::dump() const {
	for (uint32_t i = 0; i < storage->upperChunks;i++) {
		cout << "Dump Index " << i << endl;
		shared_ptr<IndexChunk>  b = getChunkForRead(i);
		b->getBase()->dump();
	}
}

[[maybe_unused]] void IndexManager::showChunkStates(const std::string_view e)  {
	constexpr const char *S = "EFPX";
	cout << "Key " << e << " has " << to_string(storage->upperChunks) << " touched chunks" << endl;
	cout << "[0] ";
	for (uint32_t i = 0; i < storage->upperChunks; i++) {
		auto o = getState(i);
		cout << S[o];
		if (i%32 == 31) {
			cout << " ";
		}
		if (i%64 == 63) {
			cout << endl;
			cout << "[" << to_string(i) << "] ";
		}
	}
	cout << endl;
}

void IndexManager::removeWholeIndex() {
    if (storage != nullptr) {
        munmap(storage, getIndexFileSize());
    }
    close(fdIndex);
    remove(getIndexFileName().c_str());
	if (me.t == mapping::index_type_enum || me.t == mapping::index_type_string) {
		dicManager->removeDictionary();
	}
}

void IndexManager::adjustStorageSize() {
    if (storage->upperChunks + chunkIncreaseSize > storage->allocatedChunks) {
        if(dicManager != nullptr) {
            dicManager->extendSpace(chunkIncreaseSize);
        }
        if (storage->allocatedChunks + chunkIncreaseSize > MAX_NUMBER_OF_CHUNKS) {
            Log::error("Max number of chunks reached ("+ to_string(MAX_NUMBER_OF_CHUNKS)+") for index "+index);
            Log::error("There is more than " + to_string(MAX_NUMBER_OF_CHUNKS*CHUNK_SIZE) +" documents in this index");
            sync();
            Log::panic("Increase MAX_NUMBER_OF_CHUNKS in config.h, recompile and reindex everything. Aborting");
            // not going here....
        }
        storage = (IndexStructure*)extendMapping(fdIndex,chunkIncreaseSize*indexSize,(char*)storage);
        storage->allocatedChunks += chunkIncreaseSize;
    }
}

shared_ptr<StorageModel> IndexManager::getFixedStorageModel(uint32_t chunk,bool isArray) const {
    uint64_t dataOffset = getChunksBaseOffset() + chunk * indexSize;
    auto *d = (IndexChunkStructBase*)(((char*)storage)+dataOffset);
    auto storageModel2 = StorageModelFactory::getModel(BaseIndex::buildYmmDataPtr(d,isArray),BaseIndex::buildDeletedInfoPtr(d),d->length);
    storageModel2->setIndexStructArray(d);
    return storageModel2;
}

void IndexManager::getFixedValue(const shared_ptr<StorageModel> &storageModel2, uint32_t docId, uint32_t arrayIndex, __uint128_t &v, bool &isNull, bool &endOfDocument, bool &isDeleted, bool isArray) {
    uint32_t r;
    if (isArray) {
        auto *b = (IndexChunkStructArray*)storageModel2->getIndexStructArray();
        r = b->blocOffsetH[docId>>4] << 16;
        r |= b->blocOffsetL[docId];
        r += arrayIndex;
    } else {
        r = docId;
    }
    storageModel2->get(r,v,isNull,endOfDocument,isDeleted);
}

std::string_view IndexManager::getEnumValue(uint32_t chunk, uint32_t docId, uint32_t arrayIndex, __uint128_t &v, bool &isNull, bool &endOfDocument,bool &isDeleted, bool isArray) const {
    uint64_t dataOffset = getChunksBaseOffset() + chunk * indexSize;
    auto *d = (IndexChunkStructBase*)(((char*)storage)+dataOffset);
    shared_ptr<StorageModel> sm = StorageModelFactory::getModel(BaseIndex::buildYmmDataPtr(d,isArray), BaseIndex::buildDeletedInfoPtr(d),d->length);
    sm->setDeletedPtr(BaseIndex::buildDeletedInfoPtr(d));
    uint32_t r;
    if (isArray) {
        auto *b = (IndexChunkStructArray*)d;
        r = b->blocOffsetH[docId>>4] << 16;
        r |= b->blocOffsetL[docId];
       r += arrayIndex;
    } else {
        r = docId;
    }
    sm->get(r,v,isNull,endOfDocument,isDeleted);
    if (dicManager != nullptr) {
        std::shared_ptr<Dictionary>dic = dicManager->getDictionary(chunk);
        if (isNull) {
            return {};
        } else {
            return dic->getString((uint32_t) v);
        }
    } else {
        return {};
    }
}

bool IndexManager::compareLess(uint32_t chunk,const __int128_t &v, bool isFloat) const {
    if (me.s == mapping::storage_type_float || me.s == mapping::storage_type_double) {
        return (isFloat ? from128d(v) : (double)v)*scale > storage->minMax[chunk][0];
    } else {
        return (isFloat ? *(double*)&v : (double)v)*scale > storage->minMax[chunk][0];
    }
}

bool IndexManager::compareLessEq(uint32_t chunk,const __int128_t &v, bool isFloat) const {
    if (me.s == mapping::storage_type_float || me.s == mapping::storage_type_double) {
        return (isFloat ? from128d(v) : (double)v)*scale >= storage->minMax[chunk][0];
    } else {
        return (isFloat ? *(double*)&v : (double)v)*scale >= storage->minMax[chunk][0];
    }
}

bool IndexManager::compareGreater(uint32_t chunk,const __int128_t &v, bool isFloat) const {
    if (me.s == mapping::storage_type_float || me.s == mapping::storage_type_double) {
        return (isFloat ? from128d(v) : (double)v)*scale < storage->minMax[chunk][1];
    } else {
        return (isFloat ? *(double*)&v : (double)v)*scale < storage->minMax[chunk][1];
    }
}

bool IndexManager::compareGreaterEq(uint32_t chunk,const __int128_t &v, bool isFloat) const {
    if (me.s == mapping::storage_type_float || me.s == mapping::storage_type_double) {
        return (isFloat ? from128d(v) : (double)v)*scale <= storage->minMax[chunk][1];
    } else {
        return (isFloat ? *(double*)&v : (double)v)*scale <= storage->minMax[chunk][1];
    }
}

}
