#pragma once

#include <mutex>
#include "indexDefinition.hpp"
#include "../queries/queryStatus.hpp"

namespace mapping {


class IndexDefinitionManager {
public:
	static IndexDefinitionManager* getInstance();
	std::shared_ptr<IndexDefinition> getIndex(const std::string_view &index);
	static std::shared_ptr<mapping::IndexDefinition> getIndexManager(const std::string_view &s, const std::vector<std::string>&indexes,std::shared_ptr<queries::QueryStatus>& queryStatus);
	bool removeIndex(const std::string_view &index);
	bool truncateIndex(const std::string_view &index);
	std::shared_ptr<IndexDefinition> createIndex(const std::string_view &index);
private:
	static IndexDefinitionManager *instance;
	std::map<std::string,std::shared_ptr<IndexDefinition> > indexDefinitionMap;
	std::mutex	mu;
};

}
