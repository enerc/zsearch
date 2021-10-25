#include "mappingParser.hpp"
#include <cstring>
#include "indexDefinitionManager.hpp"

#define MAX_PATH_LENGTH 32768
using namespace std;
using namespace queries;
using namespace mapping;

namespace mapping {

MappingParser::MappingParser(const std::shared_ptr<IndexDefinition>&_m,const char *json,shared_ptr<QueryStatus>&qs) {
    queryStatus = qs;
    path = nullptr;
	if (d.Parse<kParseFullPrecisionFlag>(json).HasParseError()) {
		Log::error(queryStatus,"Parse error " + string(json));
	} else {
	    m = _m;
	    path = (char*) malloc(MAX_PATH_LENGTH);
	    pathLength = 0;
	    m->toDisk();
	    m->updateIndexRef();
	}
}

MappingParser::~MappingParser() {
	free(path);
}

void MappingParser::decodeNull(const GenericValue<rapidjson::UTF8<> >&val)  {
    Log::error(queryStatus,string("mapping field must be a string describing storage type")+getJson());
}

void MappingParser::decodeBoolean(const GenericValue<rapidjson::UTF8<> >&val)  {
    Log::error(queryStatus,string("mapping field must be a string describing storage type")+getJson());
}

void MappingParser::decodeNumber(const GenericValue<rapidjson::UTF8<> >&val)  {
	uint32_t l = pathLength-1;
	while (path[l] != '.') l--;
	l--;
	while (path[l] != '.') l--;
	l++;
	string prop(path+l,pathLength-l-1);
	string p = string(path,l-1);
	mapping_entry e = m->getEntryNoBoo(p);

	if (string("scale") == prop) {
		Log::debug(string("Add scale to ")+ p + " scale="+to_string((int)val.GetDouble()));
		e.scale = (int)val.GetDouble();
		e.flags |= EntryScaleSet;
		if (e.scale != 0) {
			if (e.s == storage_type_signed) {
				e.s = storage_type_signed_fixed_point;
			} else if (e.s == storage_type_unsigned) {
				e.s = storage_type_unsigned_fixed_point;
			}
		}
	}

	if (string("max_array_size") == prop) {
		Log::debug(string("Add max_array_size . Max array size=")+to_string((int)val.GetDouble()));
		try  {
			e.max_array_size = (int)val.GetDouble();
		} catch (const std::invalid_argument& e) {
		    Log::error(queryStatus,"Mapping has non number max_array_size:"+to_string((int)val.GetDouble()));
		}
		e.flags |= EntryArraySet;
	}

	m->updateEntry(p,e,true);
}

void MappingParser::decodeObject(const GenericValue<rapidjson::UTF8<> >&val)  {
	for (Value::ConstMemberIterator itr = val.MemberBegin(); itr != val.MemberEnd(); ++itr) {
		bool isProperty = string("properties") == string(itr->name.GetString(),itr->name.GetStringLength());
		if (!isProperty) {
			pathStack.push((int)pathLength);
			memcpy(path+pathLength,itr->name.GetString(), itr->name.GetStringLength());
			pathLength += itr->name.GetStringLength();
			path[pathLength] = '.';
			pathLength ++;
		}
		decodeDispatch(itr->value);
		if (!isProperty) {
			pathLength = pathStack.top();
			pathStack.pop();
		}
	}
}

void MappingParser::decodeArray(const GenericValue<rapidjson::UTF8<> >&val)  {
    Log::error(queryStatus,string("mapping field can't be an array")+getJson());
}

void MappingParser::decodeString(const GenericValue<rapidjson::UTF8<> >&val)  {
	string sval = string(val.GetString(),val.GetStringLength());
	uint32_t l = pathLength-1;
	while (path[l] != '.') l--;
	l--;
	while (path[l] != '.') l--;
	l++;
	string prop(path+l,pathLength-l-1);
	bool h = false;
	string p = string(path,l-1);
	mapping_entry e = m->getEntryNoBoo(p);
	e.max_array_size = 1;

	if (string("type") == prop) {
		Log::debug(string("Add mapping to ")+ p + " property="+sval);
		for (int i=0; i < getTypeDefinitionSize(); i++) {
			if (sval == mapping::types[i].json_name) {
				if ((e.flags & EntryStorageTypeSet) != 0) {
				    Log::error(queryStatus,string("Cannot change storage type at ")+string(path,l-1)+" "+getJson());
				} else {
					e.s = mapping::types[i].type;
					e.flags |= EntryStorageTypeSet;
					e.length = mapping::types[i].length;
				}
				if ((e.flags & EntryIndexTypeSet) != 0) {
				    Log::error(queryStatus,string("Cannot change index type at ")+string(path,l-1)+" "+getJson());
				} else {
					e.t = mapping::types[i].index_type;
					e.flags |= EntryIndexTypeSet;
				}
				h = true;
			}
		}
        if (sval == "join") {
            e.flags |= EntryJoinSet;
            h = true;
        }
        if (!h) {
            Log::error(queryStatus,string("Mapping has unknown field type: ") + sval + " in "+getJson());
		} else {
			m->updateEntry(p,e);
		}
	}

	if (string("format") == prop) {
		Log::debug(string("Add format to ")+ p + " property="+sval);
		e.format = sval;
		e.flags |= EntryFormatSet;
        e.isFTDate = e.format == "%F %T";
	}

	if (string("scale") == prop) {
		Log::debug(string("Add scale. path = ")+ p + " property="+sval);
		try  {
			e.scale = stoi(sval,nullptr,10);
			if (e.scale != 0) {
				if (e.s == storage_type_signed) {
					e.s = storage_type_signed_fixed_point;
				} else if (e.s == storage_type_unsigned) {
					e.s = storage_type_unsigned_fixed_point;
				}
			}
		} catch (const std::invalid_argument& e) {
		    Log::error(queryStatus,"Mapping has non number scale:"+sval);
		}
		e.flags |= EntryScaleSet;
	}

	if (string("max_array_size") == prop) {
		Log::debug(string("Add max_array_size . Max array size=")+sval);
		try  {
			e.max_array_size = stoi(sval,nullptr,10);
		} catch (const std::invalid_argument& e) {
		    Log::error(queryStatus,"Mapping has non number max_array_size:"+sval);
		}
		e.flags |= EntryArraySet;
	}

    if (string("index") == prop) {
        Log::debug("Found an index for a join field");
        e.joinIndex = sval;
    }

    if (string("this_key") == prop) {
        Log::debug("Found a this_key for a join field");
        e.joinThisKey = sval;
    }

    if (string("join_key") == prop) {
        Log::debug("Found a join_key for a join field");
        e.joinRemoteKey = sval;
    }

    if (string("value") == prop) {
        Log::debug("Found a value for a join field");
        e.joinValue = sval;
    }

    m->updateEntry(p,e);
}

void MappingParser::decodeDispatch(const GenericValue<rapidjson::UTF8<> >&val)  {
	switch (val.GetType()) {
	case kNullType:
		decodeNull(val);
		break;
	case kFalseType:
	case kTrueType:
		decodeBoolean(val);
		break;
	case kNumberType:
		decodeNumber(val);
		break;
	case kObjectType:
		pathStack.push((int)pathLength);
		decodeObject(val);
		pathLength = pathStack.top();
		pathStack.pop();
		break;
	case kArrayType:
		decodeArray(val);
		break;
	case kStringType:
		decodeString(val);
		break;
	}
}

void MappingParser::decodeProperty(const GenericValue<rapidjson::UTF8<> >&val)  {
	bool hasProperties = false;
	for (Value::ConstMemberIterator itr = val.MemberBegin(); itr != val.MemberEnd(); ++itr) {
		if (string("properties") == string(itr->name.GetString(), itr->name.GetStringLength())) {
			hasProperties = true;
			decodeDispatch(itr->value);
		}
	}
	if (!hasProperties) {
	    Log::error(queryStatus,string("No properties found in ")+getJson());
	}
}

void MappingParser::parse() {
    if (queryStatus->status != queries::HTTP_200) return;

	if (d == NULL) {
		Log::error(queryStatus,"Can't parse because of parsing error. Check logs above.");
	}
	pathLength = 0;
	bool hasMappings = false;

	if (d.IsObject()) {
		for (Value::ConstMemberIterator itr = d.MemberBegin(); itr != d.MemberEnd(); ++itr) {
			if (string("mappings") == string(itr->name.GetString(), itr->name.GetStringLength())) {
				hasMappings = true;
				decodeProperty(itr->value);
			}
		}
	}

	if (!hasMappings) {
	    Log::error(queryStatus,string("No mappings section found in ")+getJson());
	}

    joinResolver();
	m->updateIndexRef();
	m->toDisk();

	// Debug
	//m->fromDisk();
}

string MappingParser::getJson() const {
	StringBuffer buffer;
	Writer<StringBuffer> writer(buffer);
	d.Accept(writer);
	return buffer.GetString();
}

void MappingParser::dump() const {
	cout << "MappingParser::dump" << endl;
	m->dump();
}

void MappingParser::joinResolver() {
    for (auto &e : m->getIndexFields()) {
        auto f = m->getEntryNoBoo(e);
        if (f.flags&EntryJoinSet) {
            if (f.joinIndex.empty()) {
                Log::error(queryStatus,"In a type definition, when a join is used the index must be specified");
                return;
            }
            if (f.joinThisKey.empty()) {
                Log::error(queryStatus,"In a type definition, when a join is used a this_key must be specified");
                return;
            }
            if (f.joinRemoteKey.empty()) {
                Log::error(queryStatus,"In a type definition, when a join is used a join_key must be specified");
                return;
            }
            if (f.joinValue.empty()) {
                Log::error(queryStatus,"In a type definition, when a join is used a value must be specified");
                return;
            }
            shared_ptr<IndexDefinition> i = IndexDefinitionManager::getInstance()->getIndex(f.joinIndex);
            if (i == nullptr) {
                Log::error(queryStatus,f.joinIndex+" is an unknown index");
                return;
            }
            if (!i->hasEntryNoBoo(f.joinRemoteKey)) {
                Log::error(queryStatus,f.joinRemoteKey+" is an unknown field in index "+f.joinIndex);
                return;
            }
            if (!i->hasEntryNoBoo(f.joinValue)) {
                Log::error(queryStatus,f.joinValue+" is an unknown field in index "+f.joinIndex);
                return;
            }
            if (i->getEntryNoBoo(f.joinRemoteKey).length > 64) {
                Log::error(queryStatus,"Joins with key size larger than 64 bits are not supported yet (:");
                return;
            }
            auto &n = i->getEntryNoBoo(f.joinValue);

            f.length = n.length;
            f.t = n.t;
            f.s = n.s;
            f.scale = n.scale;
            f.format = n.format;
            f.max_array_size = n.max_array_size;
            f.flags |= n.flags;
            m->updateEntry(e,f,true);
        }
    }
}
}
