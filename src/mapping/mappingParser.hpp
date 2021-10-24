#pragma once

#include <iostream>
#include <list>
#include <sstream>
#include <memory>
#include <stack>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/reader.h"
#include "rapidjson/stringbuffer.h"
#include "../mapping/indexDefinition.hpp"
#include "../queries/queryStatus.hpp"
#include "mappingParser.hpp"

using namespace rapidjson;

namespace mapping {

class MappingParser {
public:
    MappingParser(const std::shared_ptr<IndexDefinition>&,const char *json,std::shared_ptr<queries::QueryStatus>&qs);
	~MappingParser();
	void parse();
	void dump() const;
private:
	void decodeNull(const GenericValue<rapidjson::UTF8<> >&val);
	void decodeBoolean(const GenericValue<rapidjson::UTF8<> >&val);
	void decodeNumber(const GenericValue<rapidjson::UTF8<> >&val);
	void decodeArray(const GenericValue<rapidjson::UTF8<> >&val);
	void decodeObject(const GenericValue<rapidjson::UTF8<> >&val);
	void decodeString(const GenericValue<rapidjson::UTF8<> >&val);
	void decodeDispatch(const GenericValue<rapidjson::UTF8<> >&);
	void decodeProperty(const GenericValue<rapidjson::UTF8<> >&);
	void joinResolver();
	[[nodiscard]] std::string getJson() const;

	Document d;
	std::shared_ptr<IndexDefinition> m;
	char 	*path;
	uint32_t pathLength;
	std::stack<int> pathStack;
	std::shared_ptr<queries::QueryStatus> queryStatus;
};
}
