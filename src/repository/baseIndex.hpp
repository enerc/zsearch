#pragma once

#include <vector>
#include "../common/int128.hpp"
#include "../../config.h"

namespace indexes {

class StorageModel;

class IndexChunkStructBase {
public:
    uint32_t    row;
	uint32_t 	length;
	uint32_t	nb_documents;
	uint32_t	chunk;uint64_t    dummy[2];
    uint64_t    isDeleted[CHUNK_SIZE/64];
};

class IndexChunkStructSingle : public IndexChunkStructBase  {
public:
    __m256i		*xmmData;
};

class IndexChunkStructArray : public IndexChunkStructBase  {
public:
    uint16_t    blocOffsetH[CHUNK_SIZE/16];
    uint16_t    blocOffsetL[CHUNK_SIZE];
    __m256i		*xmmData;
};

typedef struct  {
	uint64_t 	upperChunks;
	uint64_t 	allocatedChunks;
	uint64_t	state[MAX_NUMBER_OF_CHUNKS/32];
	uint64_t	locked[MAX_NUMBER_OF_CHUNKS/64];
	uint64_t	staled[MAX_NUMBER_OF_CHUNKS/64];
	double  	minMax[MAX_NUMBER_OF_CHUNKS][2];
} IndexStructure;

class BaseIndex {
public:
	BaseIndex(IndexChunkStructBase *,mapping::storage_types _type, bool isArray);
	virtual 	~BaseIndex() = default;
	void		setLength(uint32_t length);
    virtual void add(const __uint128_t &v, bool isNull, bool endOFDocument) = 0;
	virtual void dump() const = 0;
    virtual void getLastValue(__uint128_t &v, bool &isNull, bool &endOfDocument,bool &isDeleted)  = 0;
    virtual void getValue(uint32_t loc,__uint128_t &v, bool &isNull, bool &endOfDocument,bool &isDeleted) const = 0;
    virtual std::string_view getLastStringValue(bool &isNull, bool &endOfDocument, bool &isDeleted) = 0;
    void setArrayStart(uint32_t docId);

	[[nodiscard]] inline	uint32_t getNbRows() const {
		return data->row;
	}

	[[nodiscard]] inline uint32_t getNbDocuments() const {
		return data->nb_documents;
	}

	[[nodiscard]] inline uint32_t getLength() const {
		return length;
	}

    [[nodiscard]] inline void *getYmmData() const {
	    if (isArray) {
            auto *b = (IndexChunkStructArray*)data;
            return &b->xmmData;
	    } else {
            auto *b = (IndexChunkStructSingle*)data;
            return &b->xmmData;
	    }
	}

    static void *buildYmmDataPtr(void *data, bool isArray) {
        if (isArray) {
            auto *b = (IndexChunkStructArray*)data;
            return &b->xmmData;
        } else {
            auto *b = (IndexChunkStructSingle*)data;
            return &b->xmmData;
        }
    }

    [[nodiscard]] inline void *getDeletedInfo() const {
	    return &data->isDeleted;
	}

    static void *buildDeletedInfoPtr(void *data) {
       auto *b = (IndexChunkStructSingle*)data;
       return &b->isDeleted;
    }

    [[maybe_unused]] [[nodiscard]] inline IndexChunkStructBase *getIndexChunkStruct() const { return data; }

    void updateDocIndex(bool endOfDocument) {
	    if (isArray&&endOfDocument) {
            auto *b = (IndexChunkStructArray*)data;
            if (b->nb_documents%16 == 0) {
                b->blocOffsetH[b->nb_documents >> 4] = b->row >> 16;
            }
            b->blocOffsetL[b->nb_documents] = (uint16_t)b->row;
	    }
	}

    void reset();

    void setDeleted(uint32_t row, bool state) const {
        auto o = (uint64_t*) getDeletedInfo();
        if (state) {
            o[row>>6] &= ~(1L << (row&63));
        } else {
            o[row>>6] |= 1L << (row&63);
        }
    }

	[[nodiscard]] virtual bool isFixed() const = 0;
protected:
	uint32_t	length;
    bool        isArray;
	uint64_t	*data64;
	uint32_t    readRow;
    IndexChunkStructBase	*data;
	mapping::storage_types type;
	std::shared_ptr<StorageModel> storageModel;

private:
};

}
