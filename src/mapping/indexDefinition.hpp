#pragma once

#include <map>
#include <cstring>
#include <string_view>
#include <algorithm>

#include "mapping_types.hpp"
#include "../common/config.hpp"
#include "../common/booPHF.hpp"
#include "../common/aquahash.hpp"

namespace indexes {
	class IndexManager;
}

namespace mapping {

constexpr uint32_t EntryStorageTypeSet 	= 1 << 0;
constexpr uint32_t EntryIndexTypeSet 	= 1 << 1;
constexpr uint32_t EntryArraySet 		= 1 << 2;
constexpr uint32_t EntryFormatSet 		= 1 << 3;
constexpr uint32_t EntryScaleSet 		= 1 << 4;
constexpr uint32_t EntryJoinSet 		= 1 << 5;


class Custom_string_Hasher
{
public:
	// the class should have operator () with this signature :
	uint64_t operator ()   (const std::string_view &key, uint64_t seed=0) const {
		return AquaHash::Hash((const uint8_t*)key.data(),key.size(),_mm_set_epi64x(0,(long long)seed))[0];
	}
};
typedef boomphf::mphf<  std::string_view, Custom_string_Hasher  > boophf_t;

static const mapping_entry dummy_mapping_entry {{},storage_type_unknown,index_type_none,0,0,0,0,{},{},{},{},{},nullptr,false};

class IndexDefinition {
public:
	explicit IndexDefinition(const std::string_view &index);
	~IndexDefinition();
	bool fromDisk(bool update);
	void removeIndex();
	void truncateIndex();
	void toDisk() const;
	[[nodiscard]] inline bool hasEntry(const std::string_view &key) const {  return bphf->lookup(key) != ULLONG_MAX; }
    [[nodiscard]] inline bool hasEntryNoBoo(const std::string_view &key) const {
	    return std::find(keys.begin(), keys.end(), key) != keys.end();
    }
	void addEntry(const std::string_view &key, const mapping_entry &e);
	void updateEntry(const std::string_view &key, const mapping_entry &e, bool force = false);
	// Create if not there
    [[maybe_unused]] [[nodiscard]] const mapping_entry &getEntry(const std::string_view &key) const {
        uint64_t h = bphf->lookup(key);
        return h == ULLONG_MAX ? dummy_mapping_entry : mae_boo.at((uint32_t)h);
    }
    [[nodiscard]] const mapping_entry &getEntryNoBoo(const std::string_view &key) const {
        int i=0;
        for (auto &e : keys) {
            if (e == key) {
                return mae.at(i);
            }
            i++;
        }
        return dummy_mapping_entry;
    }

	// does not create if not there
	[[nodiscard]] [[maybe_unused]] const mapping_entry &getIndexDefinition(const std::string_view &key) const {
        uint64_t h = bphf->lookup(key);
        if (h == ULLONG_MAX) {
			Log::warn("Requested field does not exists. "+std::string(key.data(),key.size()));
			return dummy_mapping_entry;
		}
		return mae_boo.at((uint32_t)h);
	}
    [[nodiscard]] const mapping_entry &getIndexDefinition(uint64_t index) const {
        return mae_boo.at((uint32_t)index);
    }

    [[nodiscard]] inline std::shared_ptr<indexes::IndexManager>getIndex(const std::string_view &key) const {
		uint64_t h = bphf->lookup(key);
		return h == ULLONG_MAX ? nullptr : idxs.at(h);
	}
	[[nodiscard]] inline indexes::IndexManager* getIndexPtr(const std::string_view &key) const {
		uint64_t h = bphf->lookup(key);
		return h == ULLONG_MAX ? nullptr : idxs.at(h).get();
	}
	[[nodiscard]] inline std::vector<std::shared_ptr<indexes::IndexManager>> getIndexes() const {
        return idxs;
    }
	[[nodiscard]] inline uint64_t getIndexOffset(const std::string_view &key) const {
		return bphf->lookup(key);
	}

    [[maybe_unused]] [[nodiscard]] inline uint64_t getIndexNbElement() const {
		return bphf->nbKeys();
	}
	[[nodiscard]] storage_types getStorageType(const std::string_view &key) const;

    [[maybe_unused]] [[nodiscard]] index_types getIndexType(const std::string_view &key) const;

    [[maybe_unused]] [[nodiscard]] bool isJoin(const std::string_view &key) const;

	void updateIndexRef();
	void dump() const;

	[[nodiscard]] const std::string& getIndexName() const {
		return indexName;
	}

	[[nodiscard]] const std::vector<std::string> &getIndexFields() const { return keys; }
    [[nodiscard]] double getScale(const std::string &field) const;
	[[nodiscard]] [[maybe_unused]] uint32_t getLength(const std::string &field) const;
private:
	std::string  indexName;
	std::vector<std::string> keys;
    std::vector<mapping_entry> mae;
	std::vector<mapping_entry> mae_boo;
	std::vector<std::shared_ptr<indexes::IndexManager>> idxs;
    boophf_t * bphf;

	[[nodiscard]] std::string getFileName() const;

};

}
