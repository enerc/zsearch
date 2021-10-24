#pragma once

#include <vector>
#include <mutex>
#include <unistd.h>

#include "../mapping/indexDefinition.hpp"
#include "baseIndex.hpp"
#include "indexChunk.hpp"
#include "../dictionary/dictionaryManager.hpp"

namespace indexes {

constexpr int ChunkStateEmpty = 0;
constexpr int ChunkStateFull = 1;
constexpr int ChunkStatePartial = 2;

class IndexManager {
public:
	IndexManager(const std::string_view &_index,const std::string_view &_hash, const std::string_view &_key, const mapping::mapping_entry &me);
	explicit IndexManager(const mapping::mapping_entry &me);
	~IndexManager();
	void 	dump() const;
	void 	removeWholeIndex();
	std::shared_ptr<IndexChunk> lockChunkForWrite();
	void 	releaseChunk(std::shared_ptr<IndexChunk>&);

    [[maybe_unused]] void	showChunkStates(std::string_view a) ;
	void 	sync();

	[[nodiscard]] [[maybe_unused]] inline std::shared_ptr<dictionary::DictionaryManager> getDicManager() const {
		return dicManager;
	}

	[[nodiscard]] [[maybe_unused]] inline const std::string& getKey() const {
		return key;
	}

	[[nodiscard]] inline const std::string& getIndex() const {
		return index;
	}

	[[nodiscard]] [[maybe_unused]] inline bool isConstant() const {
        return isaConstant;
    }

    [[nodiscard]] [[maybe_unused]] inline mapping::index_types getIndexType() const {
		return me.t;
	}

	[[nodiscard]] inline const std::string& getHash() const {
		return hash;
	}

	[[nodiscard]] inline mapping::storage_types getStorageType() const {
		return me.s;
	}

    [[nodiscard]] [[maybe_unused]] inline uint32_t getLength() const {
		return me.length;
	}

	[[nodiscard]] inline const IndexStructure *getStorage() const {
		return storage;
	}

	[[nodiscard]] inline uint32_t	getState(uint32_t chunk) const {
		uint32_t k = chunk / 32;
		uint32_t p = (chunk &31)*2;
		uint32_t s = (storage->state[k] >> p) & 3;
		return s;
	}

	[[nodiscard]] inline uint64_t	getState64(uint32_t chunk) const {
		return storage->state[chunk / 32];
	}

	inline void setState(uint32_t chunk, uint64_t state) {
		uint32_t k =  chunk / 32;
		uint32_t p = (chunk &31)*2;
		storage->state[k] &= ~(3L<<p);
		storage->state[k] |= state << p;
	}

	inline bool checkChunkFull(const std::shared_ptr<IndexChunk> &a) {
		bool b = a->getBase()->getNbDocuments() >= CHUNK_SIZE;
		if (b) {
			uint32_t chunk = a->getChunk();
			setState(chunk,ChunkStateFull);
		}
		return b;
	}

    inline void checkChunkEmpty(std::shared_ptr<IndexChunk> &a) {
        auto g = (uint64_t*)a->getBase()->getDeletedInfo();
        for (int i=0; i< CHUNK_SIZE/64; i++) {
            if (g[i] != 0) return;
        }
        setState(a->getChunk(),ChunkStateEmpty);
        a->getBase()->reset();
    }

    [[nodiscard]] inline bool isStaled(uint32_t chunk) const {
		uint32_t k = chunk / 64;
		uint32_t p = (chunk&63);
		return ((storage->staled[k] >> p)&1) == 1;
	}

	inline void setStaled(uint32_t chunk, bool status) {
		uint32_t k = chunk / 64;
		uint32_t p = (chunk&63);
		storage->staled[k] |= (status ? 1L : 0L) << p;
	}

	[[nodiscard]] inline bool isLocked(uint32_t chunk) const {
		uint32_t k = chunk / 64;
		uint32_t p = (chunk&63);
		return ((storage->locked[k] >> p)&1) == 1;
	}

	inline void setLocked(uint32_t chunk, bool status) {
		uint32_t k = chunk / 64;
		uint32_t p = (chunk&63);
		if (status) {
			storage->locked[k] |= 1L << p;
		} else {
			storage->locked[k] &= ~(1L << p);
		}
	}

	[[nodiscard]] uint64_t 	getNumberOfChunks() const;

	[[nodiscard]] inline double getScale() const {
		return scale;
	}

	[[nodiscard]] std::shared_ptr<IndexChunk> getChunkForRead(uint32_t chunk) const;
    [[nodiscard]] std::shared_ptr<IndexChunk> getChunkForWrite(uint32_t chunk) const;
    [[nodiscard]] [[maybe_unused]] uint32_t getMaxArraySize() const { return me.max_array_size; }

    [[nodiscard]] inline const mapping::mapping_entry &getMapping() const { return me; }
    void  adjustStorageSize();

    [[nodiscard]] bool compareTwoEnumsAsc(uint64_t a, uint64_t b) const {
        return dicManager->getDictionary(a>>32)->getString(a&0xffff) < dicManager->getDictionary(b>>32)->getString(b&0xffff);
    }
    [[nodiscard]] bool compareTwoEnumsDesc(uint64_t a, uint64_t b) const {
        return dicManager->getDictionary(a>>32)->getString(a&0xffff) > dicManager->getDictionary(b>>32)->getString(b&0xffff);
    }
    [[nodiscard]] bool compareTwoEnumsEq(uint64_t a, uint64_t b) const {
        return dicManager->getDictionary(a>>32)->getString(a&0xffff) == dicManager->getDictionary(b>>32)->getString(b&0xffff);
    }
    [[nodiscard]] std::shared_ptr<StorageModel> getFixedStorageModel(uint32_t chunk,bool isArray) const;
    static void getFixedValue(const std::shared_ptr<StorageModel> &, uint32_t docId, uint32_t arrayIndex, __uint128_t &v, bool &isNull, bool &endOfDocument,bool &isDeleted, bool isArray);
    std::string_view getEnumValue(uint32_t chunk, uint32_t docId, uint32_t arrayIndex, __uint128_t &v, bool &isNull, bool &endOfDocument,bool &isDeleted, bool isArray) const;
    [[nodiscard]] void *getData(uint32_t chunk) const;
    [[nodiscard]] void *getDeletedPtr(uint32_t chunk) const;
    [[nodiscard]] [[maybe_unused]] std::shared_ptr<StorageModel> getStorageModel() const { return storageModel; }

    [[nodiscard]] bool compareLess(uint32_t chunk,const __int128_t &, bool isFloat) const;
    [[nodiscard]] bool compareLessEq(uint32_t chunk,const __int128_t &, bool isFloat) const;
    [[nodiscard]] bool compareGreater(uint32_t chunk,const __int128_t &, bool isFloat) const;
    [[nodiscard]] bool compareGreaterEq(uint32_t chunk,const __int128_t &, bool isFloat) const;

private:
	std::string index;
	std::string key;
	std::string hash;
	int			fdIndex;
    mapping::mapping_entry me;
    bool		isFixed;
	IndexStructure *storage;
	std::shared_ptr<dictionary::DictionaryManager> dicManager;
	double 		scale;
    size_t      indexSize;
    bool        isaConstant;
    std::shared_ptr<StorageModel> storageModel;
    static constexpr uint64_t chunkIncreaseSize = 1024;   // must be enough given IMPORT_BUFFER_SIZE and line size


    [[nodiscard]] std::string getFileFolder() const;
	[[nodiscard]] std::string getIndexFileName() const;
	void 		createFile();
	[[nodiscard]] size_t 		getIndexInitialFileSize() const;
    [[nodiscard]] size_t 		getIndexFileSize() const;
	std::shared_ptr<BaseIndex>	makeIndex(IndexChunkStructBase *a,mapping::storage_types storageType,std::shared_ptr<dictionary::Dictionary> &dic) const;
	[[nodiscard]] static uint64_t	getChunksBaseOffset() ;
	void calcIndexSize();
};

}
