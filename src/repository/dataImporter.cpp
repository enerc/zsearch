#include "dataImporter.hpp"
#include "../mapping/indexDefinitionManager.hpp"
#include "../repository/indexRepository.hpp"
#include <sstream>

using namespace simdjson;
using namespace std;
using namespace mapping;
using namespace queries;
using namespace indexes;
using namespace std::chrono;


namespace indexes {

DataImporter::DataImporter(const std::string &_index,const string_view &lines,shared_ptr<QueryStatus> &_queryStatus) : index(_index), queryStatus(_queryStatus), idx(0) {
	//cout <<lines.substr(0,100)<<  " ....";
	dom::parser parser;
	simdjson::dom::document_stream docs;
	auto error = parser.parse_many(lines.data(),lines.size(),dom::DEFAULT_BATCH_SIZE).get(docs);
	if (error) {
	    Log::error(queryStatus,string("Parse error - ")+lines.data());
	}
	try {
        //cout << " ERR:" << error << endl;
        id = IndexDefinitionManager::getInstance()->getIndex(index);
        myChunks = IndexRepository::getInstance()->getChunksForWrite(index, capacity);
        csize = myChunks.size() / 64 + 1;
        columnSet.resize(csize);
        columnSetMask.resize(csize);
        joins.resize(myChunks.size());
        for (int i = 0; i < csize; i++) {
            columnSetMask[i] = 0;
        }
        for (int i = (int)myChunks.size(); i < 64 * csize; i++) {
            uint32_t o = i >> 6;
            uint32_t p = i % 64;
            columnSetMask[o] |= 1L << p;
        }
        for (auto doc : docs) {
            processJsonInput(doc.value());
        }
    } catch (simdjson_error &e) {
	    documentErrors ++;
	    Log::error(queryStatus,string("Parse error - ")+lines.data());
	}
	IndexRepository::getInstance()->releaseLockedChunks(index,myChunks);
	queryStatus->addErrors((int)documentErrors);
	queryStatus->addFailedDocuments((int)documentFailed);
	queryStatus->addProcessedDocuments((int)documentImported);
	queryStatus->addWarnings((int)documentWarnings);
}

void DataImporter::processJsonInput(const dom::element &doc) {
	clearColumnSet();
	if (doc.is_object()) {
		processJsonObject(doc.get_object());
	} else {
		cout << "Fixme:: processJsonInput not an object " << endl;
	}
	completeColumnSet();
	capacity ++;

	if (capacity == CHUNK_SIZE) {
		//cout << "CHUNK FULL ";
		// chunks full
		IndexRepository::getInstance()->releaseLockedChunks(index,myChunks);
		// a new chunks
		myChunks = IndexRepository::getInstance()->getChunksForWrite(index,capacity);
		//cout <<  "New capacity " << capacity << endl;
	}
	documentImported++;
}

void DataImporter::processJsonObject(const dom::object &doc) {
	for (const auto &e : doc) {
		idx = id->getIndexOffset(e.key);
		if (idx == ULLONG_MAX) {
			Log::warn("DataImporter: Undefined key "+string(e.key.data())+" in index "+string(index.data(),index.size()));
			documentWarnings ++;
			return;
		}
        if (setColumnSet(idx)) {
            const auto &me = id->getIndexDefinition(idx);
            if ((me.flags & EntryJoinSet) != 0) {
                // should not be here ....
            } else {
                processJsonObjectDefined(e, me);
            }
        } else {
            Log::error(queryStatus,string("Column already set : ")+string(e.key.data(),e.key.size()));
        }
	}
}

void DataImporter::processJsonObjectDefined(const dom::key_value_pair &e, const mapping_entry &me) {
    if (me.t == index_type_none) {
        Log::error(queryStatus,"DataImporter : invalid key "+string(e.key));
        documentErrors++;
    } else {
        uint32_t length = me.length;
        switch (me.s) {
            case storage_type_null:
                break;
            case storage_type_signed:
                if (e.value.is_array()) {
                    uint32_t len = e.value.get_array().size();
                    for (const auto &i : e.value.get_array()) {
                        len --;
                        myChunks[idx]->addSigned(convertSigned(length,i,e.key),len == 0);
                    }
                } else {
                    myChunks[idx]->addSigned(convertSigned(length,e.value,e.key),true);
                }
                break;
            case storage_type_unsigned:
                if (e.value.is_array()) {
                    uint32_t len = e.value.get_array().size();
                    for (const auto &i : e.value.get_array()) {
                        len --;
                        myChunks[idx]->addUnsigned(convertUnsigned(length,i,e.key,me),len == 0);
                    }
                } else {
                    myChunks[idx]->addUnsigned(convertUnsigned(length,e.value,e.key,me),true);
                }
                break;
            case storage_type_float16:
                if (e.value.is_array()) {
                    uint32_t len = e.value.get_array().size();
                    for (const auto &i : e.value.get_array()) {
                        len --;
                        myChunks[idx]->addFloat16((float)convertDouble(i,e.key),len == 0);
                    }
                } else {
                    myChunks[idx]->addFloat16((float)convertDouble(e.value,e.key),true);
                }
                break;
            case storage_type_float:
                if (e.value.is_array()) {
                    uint32_t len = e.value.get_array().size();
                    for (const auto &i : e.value.get_array()) {
                        len --;
                        myChunks[idx]->addFloat((float)convertDouble(i,e.key),len == 0);
                    }
                } else {
                    myChunks[idx]->addFloat((float)convertDouble(e.value,e.key),true);
                }
                break;
            case storage_type_double:
                if (e.value.is_array()) {
                    uint32_t len = e.value.get_array().size();
                    for (const auto &i : e.value.get_array()) {
                        len --;
                        myChunks[idx]->addDouble(convertDouble(i,e.key),len == 0);
                    }
                } else {
                    myChunks[idx]->addDouble(convertDouble(e.value,e.key),true);
                }
                break;
            case storage_type_geopoint:
                if (e.value.is_array()) {
                    bool isArray;
                    for (const auto &i : e.value.get_array()) {
                        isArray = i.is_array();
                        break;
                    }

                    // an array of 2 elements
                    if (isArray) {
                        auto len = e.value.get_array().size();
                        for (const auto &i : e.value.get_array()) {
                            const auto &j = i.get_array();
                            if (j.size() != 2) {
                                Log::error(queryStatus,string("Expected a 2 value location for ")+e.key.data());
                            } else {
                                double p[2];
                                int l = 0;
                                for (const auto &k : j) {
                                    p[l++] = convertDouble(k,e.key);
                                }
                                len --;
                                myChunks[idx]->addLocation((float)p[0],(float)p[1],len == 0);
                            }
                        }
                    } else {
                        uint32_t len = e.value.get_array().size();
                        if (len == 2) {
                            double p[2];
                            int l = 0;
                            for (const auto &i : e.value.get_array()) {
                                p[l++] = convertDouble(i,e.key);
                            }
                            myChunks[idx]->addLocation((float)p[0],(float)p[1],true);
                        } else {
                            Log::error(queryStatus,string("Expected a 2 value location for ")+e.key.data());
                        }
                    }
                } else {
                    Log::error(queryStatus,string("Expected an array for a location for ")+e.key.data());
                }
                break;
            case storage_type_enum:
                if (e.value.is_array()) {
                    uint32_t len = e.value.get_array().size();
                    for (const auto &i : e.value.get_array()) {
                        len --;
                        myChunks[idx]->addRef(convertString(i,e.key),len == 0);
                    }
                } else {
                    myChunks[idx]->addRef(convertString(e.value,e.key),true);
                }
                break;
            case storage_type_signed_fixed_point:
                if (e.value.is_array()) {
                    uint32_t len = e.value.get_array().size();
                    for (const auto &i : e.value.get_array()) {
                        len --;
                        myChunks[idx]->addSignedScaled(convertDouble(i,e.key),len == 0);
                    }
                } else {
                    myChunks[idx]->addSignedScaled(convertDouble(e.value,e.key),true);
                }
                break;
            case storage_type_unsigned_fixed_point:
                if (e.value.is_array()) {
                    uint32_t len = e.value.get_array().size();
                    for (const auto &i : e.value.get_array()) {
                        len --;
                        myChunks[idx]->addUnsignedScaled(convertDouble(i,e.key),len == 0);
                    }
                } else {
                    myChunks[idx]->addUnsignedScaled(convertDouble(e.value,e.key),true);
                }
                break;
            case storage_type_unknown:
                break;
        }
    }
}

void DataImporter::processJsonObjectJoin(int z, const mapping_entry &me) {
    shared_ptr<DataJoin> p = joins.at(z);
    if (p == nullptr) {
        p = DataJoinFactory::getInstance(me.joinIndex,me.joinRemoteKey,me.joinValue,queryStatus);
        joins.at(z) = p;
    }
    if (p == nullptr) return;

    uint64_t m = id->getIndexOffset(me.joinThisKey);
    bool isNull;
    bool endOfDocument;
    bool isDeleted;
    if (myChunks[m]->isFixed()) {
        __uint128_t v;
        myChunks[m]->getBase()->getLastValue(v,isNull,endOfDocument,isDeleted);
        if (myChunks[z]->isFixed()) {
            v = p->getValue(v,isNull,endOfDocument,isDeleted,queryStatus);
            if (isNull) {
                myChunks[z]->addNull(endOfDocument);
            } else {
                myChunks[z]->addUnsigned(v,endOfDocument);
            }
        } else {
            auto h = p->getStringValue(v,isNull,endOfDocument,isDeleted,queryStatus);
            if (isNull) {
                myChunks[z]->addNull(endOfDocument);
            } else {
                myChunks[z]->addRef(h,endOfDocument);
            }
        }
    } else {
        string_view s = myChunks[m]->getBase()->getLastStringValue(isNull,endOfDocument,isDeleted);
        if (myChunks[z]->isFixed()) {
            __uint128_t v = p->getValue(s,isNull,endOfDocument,isDeleted,queryStatus);
            if (isNull) {
                myChunks[z]->addNull(endOfDocument);
            } else {
                myChunks[z]->addUnsigned(v,endOfDocument);
            }
        } else {
            auto h = p->getStringValue(s,isNull,endOfDocument,isDeleted,queryStatus);
            if (isNull) {
                myChunks[z]->addNull(endOfDocument);
            } else {
                myChunks[z]->addRef(h,endOfDocument);
            }
        }
    }
}

__int128_t DataImporter::convertSigned(uint32_t length, const dom::element &e, const string_view &f)  {
	if (e.is_string()) {
		return parseSigned128(e.get_string(),length);
	} else if (e.is_int64()) {
		return convertTo128iSigned(e.get_int64());
	} else {
		Log::error(queryStatus,string("unexpected signed value for field : ") + f.data());
		documentErrors++;
		return 0;
	}
}

__uint128_t DataImporter::convertUnsigned(uint32_t length, const dom::element &e, const string_view &f,const mapping::mapping_entry &me)  {
    if (me.t == mapping::index_type_date || me.t == mapping::index_type_date_nano) {
        if (me.format.empty()) {
            Log::error(queryStatus, string("You must provide a date format for field : ") + f.data());
            documentErrors++;
            return 0;
        }
        if (me.t == mapping::index_type_date ) {
            bool fail;
            uint64_t d =  stringToDate(me.isFTDate,false,me.format,e.get_string(),fail);
            if (fail) {
                string_view t(e.get_string());;
                Log::error(queryStatus, string("Wrong date format for field : ") + f.data() + " value is:" + string(t.data(), t.size()));
                documentErrors++;
                return 0;
            }
            return d;
        } else {
            bool fail;
            uint64_t d =  stringToDate(me.isFTDate,true,me.format,e.get_string(),fail);
            if (fail) {
                string_view t(e.get_string());;
                Log::error(queryStatus, string("Wrong date format for field : ") + f.data() + " value is:" + string(t.data(),t.size()));
                documentErrors++;
                return 0;
            }
            return d;
        }
    } else {
        if (e.is_string()) {
            return parseUnsigned128(e.get_string(), length);
        } else if (e.is_int64() || e.is_uint64()) {
            return convertTo128iUnsigned(e.get_uint64());
        } else {
            Log::error(queryStatus, string("unexpected unsigned value for field : ") + f.data());
            documentErrors++;
            return 0;
        }
    }
}

double DataImporter::convertDouble(const dom::element &e, const string_view &f)  {
	if (e.is_double()) {
		return e.get_double();
	} else if (e.is_int64() || e.is_uint64()) {
		return (double)e.get_int64();
	} else {
		Log::error(queryStatus,string("unexpected float value for field : ") + f.data());
		documentErrors++;
		return 0;
	}
}

string_view DataImporter::convertString(const dom::element &e, const string_view &f)  {
	if (e.is_string()) {
		return {e.get_string()};
	} else if (e.is_int64()) {
        numberAsString = to_string(e.get_int64().value());
        return {numberAsString.c_str(),numberAsString.size()};
	} else if (e.is_uint64()) {
        numberAsString = to_string(e.get_uint64().value());
        return {numberAsString.c_str(),numberAsString.size()};
	} else if (e.is_double()) {
        numberAsString = to_string(e.get_double().value());
        return {numberAsString.c_str(),numberAsString.size()};
	} else{
		Log::error(queryStatus,string("unexpected datatype for an enum for field : ") + f.data());
		documentErrors++;
		return "";
	}
}

void DataImporter::clearColumnSet() {
	for (auto &i : columnSet) {
		i = 0;
	}
}

void DataImporter::completeColumnSet() {
	for (int i=0; i< csize; i++) {
		uint64_t j = columnSet[i] | columnSetMask[i];
		if (j == 0xffffffffffffffffL) {
			continue;	// all set
		} else {
			j = ~j;
            int k;
            while ((k=__builtin_ffsl((int64_t)j)) > 0) {
                int z = i*64+k-1;
                mapping_entry e = id->getIndexDefinition(z);
                if ((e.flags & EntryJoinSet) != 0) {
                    processJsonObjectJoin(z,e);
                } else {
                    myChunks[z]->addNull(true);
                }
                j &= ~(1L<<(k-1));
            }
        }
	}
}

bool DataImporter::setColumnSet(uint32_t _index) {
    uint64_t k = _bextr_u64(columnSet[_index>>6],_index&63,1);
    if (k) {
        return false;
    } else {
        columnSet[_index>>6] |= 1L << (_index &63);
        return true;
    }
}

}
