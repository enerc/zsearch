#pragma once

#include <memory>
#include <unordered_map>
#include <stack>
#include <mutex>


#include "../mapping/indexDefinition.hpp"
#include "../queries/queryStatus.hpp"
#include "indexManager.hpp"

namespace indexes {
class IndexRepository {
public:
	static IndexRepository* getInstance();

	std::vector<std::shared_ptr<indexes::IndexChunk>> getChunksForWrite(const std::string &index, uint32_t &capacity);
	void releaseLockedChunks(const std::string &index, std::vector<std::shared_ptr<indexes::IndexChunk>> &);
	std::shared_ptr<IndexManager> getIndex(const std::string &_index,const std::string &key);
	std::shared_ptr<IndexManager> getIndex(const std::string &_index,const std::string &key,const mapping::mapping_entry &);
	[[maybe_unused]] static std::shared_ptr<IndexManager> getIntegerIndex();
    [[maybe_unused]] static std::shared_ptr<IndexManager> getDoubleIndex();

	void removeIndex(const std::string &index,const std::string &hash);
	static void ingest(const std::string &index,const std::string_view &lines,std::shared_ptr<queries::QueryStatus> queryStatus);
	void adjustStorageSize(const std::string &index);
    void deleteDocuments(const std::string &index, const uint64_t *data, uint64_t count);
	void sync();
	[[nodiscard]] uint32_t getNumberOfChunks(const std::string &_index) const;
	[[nodiscard]] std::string getOneMember(const std::string &_index) const;
private:
	static IndexRepository *instance;
	std::map<const std::string,std::shared_ptr<IndexManager> > indexMap;
	char 	*path;
	std::mutex	mu;

	[[nodiscard]] static std::string getKey(const std::string &_index,const std::string &_field) ;
};

}
