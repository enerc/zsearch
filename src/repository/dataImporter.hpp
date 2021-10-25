#pragma once

#include <memory>
#include <map>
#include <list>

#include "../common/avx_conf.h"
#include "../queries/queryStatus.hpp"
#include "../mapping/indexDefinition.hpp"
#include "indexChunk.hpp"
#include "datajoin.hpp"

namespace indexes {

class DataImporter {
public:
	DataImporter(const std::string &index,const std::string_view &lines,std::shared_ptr<queries::QueryStatus> &_queryStatus);
private:
	const std::string &index;
	std::shared_ptr<queries::QueryStatus> queryStatus;
	std::vector<std::shared_ptr<indexes::IndexChunk>> myChunks;
	uint32_t capacity = 0;
	uint32_t csize = 0;
	uint64_t documentImported = 0;
	uint64_t documentFailed = 0;
	uint64_t documentWarnings = 0;
	uint64_t documentErrors = 0;
    uint64_t idx;
	std::vector<uint64_t> columnSet;
	std::vector<uint64_t> columnSetMask;
	std::shared_ptr<mapping::IndexDefinition> id;
	std::string numberAsString;
	std::vector<std::shared_ptr<DataJoin>> joins;

	void 	processJsonInput(const simdjson::dom::element &doc);
	void 	processJsonObject(const simdjson::dom::object &doc);
    void 	processJsonObjectDefined(const simdjson::dom::key_value_pair &e, const mapping::mapping_entry *me);
    void 	processJsonObjectJoin(int z,const mapping::mapping_entry *me);

	__int128_t convertSigned(uint32_t length, const simdjson::dom::element &e, const std::string_view &f) ;
	__uint128_t convertUnsigned(uint32_t length, const simdjson::dom::element &e, const std::string_view &f,const mapping::mapping_entry *indexType);
	double convertDouble(const simdjson::dom::element &e, const std::string_view &f) ;
	std::string_view convertString(const simdjson::dom::element &e, const std::string_view &f) ;
	void	clearColumnSet();
	void 	completeColumnSet();
	bool 	setColumnSet(uint32_t _index);
};
}
