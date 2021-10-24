#pragma once

#include <unordered_map>
#include <utility>

#include "../common/config.hpp"
#include "../common/log.hpp"
#include "../../config.h"

namespace dictionary {

// for enums and strings
typedef struct {
	uint32_t nextEntry;
	uint32_t lcrc : 31;
	uint32_t hasCollision : 1;
	uint32_t value;
} entry;						// 96 bits

// for strings ands
typedef struct {
	// fast lookup hash.
	uint64_t	fastLookupHash[8192];
	//
	uint32_t	nbEntries;
	uint32_t	nbExtraEntries;
	// first level of lookup
	entry		entries[4096];
	//
	entry		extraEntries[1];
} hash_storage;

typedef struct {
    uint32_t    container;  // string offsets
} strings_properties;


class Dictionary {
public:
	Dictionary(hash_storage *, strings_properties *, uint32_t _maxArraySize, bool create=false);
	[[nodiscard]] uint32_t getIndex(uint32_t crc32c,const std::string_view &key, bool forceCheck) const ;
	// return the index
    [[nodiscard]] uint32_t addKey(const std::string_view &key) ;
    [[nodiscard]] uint32_t getKey(const std::string &key) const ;
    [[nodiscard]] uint32_t getNbEntries() const  { return hs->nbEntries;}
    [[nodiscard]] const uint8_t *getValuePtr() const  { return strings;}
    [[nodiscard]] const uint32_t *getSizes() const { return sizes; }
    [[nodiscard]] std::string_view getString(uint32_t index) const;

    [[maybe_unused]] void prefetch(uint32_t crc32c) ;
	void dump() ;
private:
	//[[nodiscard]] std::string getHashFileName(const Config &) const;
	[[nodiscard]] bool fastlookup(uint32_t crc32c) const;
	[[nodiscard]] bool hasThisHash(uint32_t crc32c) const;
	void updateFastLookup(uint32_t crc32c);
	void setDuplicate(uint32_t crc32c);

	//
    hash_storage    	*hs;
    strings_properties	*st;
    uint8_t		        *strings;	// original string
    uint32_t            maxArraySize;
    uint32_t            *sizes;

};

[[maybe_unused]] inline void  Dictionary::prefetch(uint32_t crc32c) {
	crc32c &= 0x3ffff;
    uint32_t l = crc32c >> 6;
	__builtin_prefetch(&hs->fastLookupHash[l],0,0);
}

}
