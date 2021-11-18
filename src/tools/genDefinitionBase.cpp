#include "genDefinitionBase.hpp"
#include <iostream>
#include "../common/simdjson.h"
#include "../common/int128.hpp"

using namespace std;
using namespace repository;
using namespace simdjson;

constexpr double MAX_SCALE_INT = 1e6;
constexpr int    MAX_SCALE = 4;       // 4 digits

namespace tools {

string GenDefinitionBase::doBaseGenImport(ImportFileBase *b) {
   std::map<std::string,mapping::mapping_entry> maps;
   if (!b->initImport())
        return {};
    if (!b->openFile())
        return {};
    if (!b->decodeHeader())
        return {};
    if (!b->allocBuffers())
        return {};
    if (!b->fetch()) {
        free(b->buffer0);
        b->buffer0 = nullptr;
        free(b->buffer1);
        b->buffer1 = nullptr;
        return {};
    }
    b->estimateLineWorkSize();
    b->position = 0;

    try {
        workerInitParams = new WorkerInitParam[b->processor_count]();
        for (int i=0; i< b->processor_count; i++) {
            new(&workerInitParams[i]) WorkerInitParam();
        }
    } catch (const bad_alloc& e) {
        Log::error(b->queryStatus,"Failed to create file import workers. Allocated "+ to_string(b->processor_count*sizeof(WorkerInitParam)));
        return {};
    }

    while (b->len >= 0) {
        b->pos = b->toSkip;
        char *next = b->buffers[b->workingBuffer]+b->toSkip;

        for (int i=0; i< b->processor_count; i++) {
            char *start_pos = next;

            int m = 0;
            while (b->pos < b->usefullBufferSize[b->workingBuffer]) {
                char *prev = next;
                next = (char*)memchr((const void*)next,0xa,b->usefullBufferSize[b->workingBuffer]-b->pos);
                if (next == nullptr) {
                    next = prev;
                    break;
                }
                for (int k=0; k < b->lineSize ;  k+=64) {
                    __builtin_prefetch (next+2*b->lineSize+k, 0, 0);
                }
                next ++;
                if (*next == 0x0d)
                    next ++;
                b->pos += next - prev;
                m++;
                if (m == b->lines_per_thread-1)
                    break;
            }
            if (m > 0) {
                workerInitParams[i].index = b->index;
                workerInitParams[i].t = this;
                workerInitParams[i].lines = string_view(start_pos,next-start_pos-1);
                workerInitParams[i].queryStatus = b->queryStatus;
                pthread_attr_t attribute;
                pthread_t t;
                pthread_attr_init(&attribute);
                pthread_attr_setstacksize(&attribute,4096);
                pthread_create(&t,&attribute,InternalThreadEntryFunc,workerInitParams+i);
                b->threads.push_back(t);
            }
        }
        b->position += b->pos;
        b->fetch();
        for (auto &i : b->threads) {
            void* status;
            pthread_join(i, &status);
        }
        b->threads.clear();

        // partial merge
        for (int i=0; i< b->processor_count; i++) {
            for (const auto &e: workerInitParams[i].maps) {
                if (maps.find(e.first) == maps.end()) {
                    maps[e.first] = e.second;
                } else {
                    auto &o = maps.find(e.first)->second;
                    if (o.length < e.second.length) o.length = e.second.length;
                    if (o.scale < e.second.scale) o.scale = e.second.scale;
                    if (o.max_array_size < e.second.max_array_size) o.max_array_size = e.second.max_array_size;
                }
            }
        }
        for (int i=0; i< b->processor_count; i++) {
            for (auto &e: workerInitParams[i].maps) {
                e.second.length = maps[e.first].length;
                e.second.scale = maps[e.first].scale;
            }
        }

    }

    // final merge
    for (int i=0; i< b->processor_count; i++) {
        for (const auto &e: workerInitParams[i].maps) {
            if (maps.find(e.first) == maps.end()) {
                maps[e.first] = e.second;
            } else {
                auto &o = maps.find(e.first)->second;
                if (o.length < e.second.length) o.length = e.second.length;
                if (o.scale < e.second.scale) o.scale = e.second.scale;
                if (o.max_array_size < e.second.max_array_size) o.max_array_size = e.second.max_array_size;
            }
        }
    }
    delete[] workerInitParams;
    workerInitParams = nullptr;
    free(b->buffer0);
    b->buffer0 = nullptr;
    free(b->buffer1);
    b->buffer1 = nullptr;

    return outputDefinition(maps);
}

void GenDefinitionBase::processingLoop(WorkerInitParam *o) {
    dom::parser parser;
    simdjson::dom::document_stream docs;
    auto error = parser.parse_many(o->lines.data(),o->lines.size(),dom::DEFAULT_BATCH_SIZE).get(docs);
    if (error) {
        Log::error(o->queryStatus,string("Parse error - ")+o->lines.data());
    }
    try {
        for (auto d : docs) {
            const dom::element &d1 = d.value();
            if (d1.is_object()) {
                const auto &doc = d1.get_object();
                for (const auto &e : doc) {
                    const string ek = string(e.key);
                    //if (ek != "passenger_count") continue;
                    if (o->maps.find(ek) == o->maps.end()) {
                        mapping::mapping_entry m;
                        m.scale = 0;
                        m.s = mapping::storage_type_unknown;
                        m.length = 0;
                        m.max_array_size = 0;
                        m.format = "x";
                        m.t = mapping::index_type_none;
                        o->maps[ek] = m;
                    }
                    auto &m = o->maps.at(ek);

                    if (e.value.is_int64()) {
                        decodeInt(e.value.get_int64(), m);
                    } else if (e.value.is_double()) {
                        //auto c = e.value.;
                        stringstream ms;
                        ms << e.value << endl;
                        //cout << ms.str() << endl;
                        const string g = ms.str();
                        //if (ek == "total_amount")
                        decodeDouble(e.value.get_double(), g, m);
                    } else if (e.value.is_string()) {
                        decodeString(e.value.get_string(), m);
                    } else if (e.value.is_array()) {
                        auto a = e.value.get_array();
                        m.max_array_size = max(m.max_array_size, (uint32_t) a.size());
                        bool inGeoRange = true;
                        for (const auto &r : a) {
                            if (r.is_int64()) {
                                //decodeInt(r.get_int64(),m);
                                if (fabs((double) r.get_int64()) > 180) inGeoRange = false;
                            } else if (r.is_double()) {
                                //decodeDouble(r.get_double(),m);
                                if (fabs(r.get_double()) > 180) inGeoRange = false;
                            } else if (r.is_string()) {
                                decodeString(r.get_string(), m);
                            }
                        }
                        if (m.max_array_size == 2 && !inGeoRange) m.format = "noGeo";
                    }
                }
            }
        }
    } catch (simdjson_error &e) {
        Log::error(o->queryStatus, string("Parse error - ") + o->lines.data());
    }

}

void GenDefinitionBase::decodeDouble(double a,const std::string &s, mapping::mapping_entry &m)  {
    const int l = (int)(s.size()-1);
    int k = l;
    while (s.at(k) != '.' && k > 0) k--;
    k++;
    uint32_t w = m.scale;
    m.scale = max(m.scale,(uint32_t)(l-k));
    m.length += 4*(m.scale-w);              // factor of 16, where 10 is only needed.
    if (m.scale > 0 && m.scale <= MAX_SCALE) {
        if (m.s == mapping::storage_type_unsigned_fixed_point) {
            if (a < 0) {
                m.s = mapping::storage_type_signed_fixed_point;
            }
        } else if (m.s == mapping::storage_type_signed_fixed_point) {
        } else {
            m.s = a < 0 ? mapping::storage_type_signed_fixed_point : mapping::storage_type_unsigned_fixed_point;
        }
        auto v = (__uint128_t)(abs(a)*pow(10,m.scale));
        m.length = max(m.length,(uint32_t)getBitLength(v));
    } else {
        m.s = mapping::storage_type_float;
        m.length = 32;
    }

}

void GenDefinitionBase::decodeInt(int64_t c, mapping::mapping_entry &m) {
    uint64_t d = c < 0 ? -c : c;
    // in case this was previously a scaled int
    d *= (int)pow(10,m.scale);
    if (m.s == mapping::storage_type_unknown) {
        m.s = c < 0 ? mapping::storage_type_signed : mapping::storage_type_unsigned;
        m.length = max(m.length,(uint32_t)getBitLength(d));
    } else if (m.s == mapping::storage_type_unsigned) {
        if (c < 0) {
            m.s = mapping::storage_type_signed ;
        }
        m.length = max(m.length,(uint32_t)getBitLength(d));
    } else if (m.s == mapping::storage_type_signed) {
        m.length = max(m.length,(uint32_t)getBitLength(d));
    } else {
        // already a float
    }
}

void GenDefinitionBase::decodeString(const std::string_view &s, mapping::mapping_entry &m)  {
    if (m.t == mapping::index_type_date || m.t == mapping::index_type_date_nano) {
    } else {
        bool fail = true;
        if (m.s == mapping::storage_type_unknown) {
            stringToDate(false,false,"%F %T", s,fail);
        }
        if (!fail) {
            m.t = mapping::index_type_date;
            m.s = mapping::storage_type_unsigned;
            m.length = 32;
        } else if (m.length <= 128) {
            m.t = mapping::index_type_enum;
            bool isNum = true;
            bool isNeg = false;
            int i = 0;
            for (auto &p : s) {
                i++;
                if (p == '-' && i == 1) {
                    isNeg = true;
                    continue;
                }
                if (p == '+') continue;
                if (p<'0' || p>'9') isNum = false;
            }
            if (!isNum) {
                m.length = 129; // one of them is not a number
            }
            if (m.length <= 128 && isNum) {
                if (isNeg) {
                    __int128_t a = parseSigned128(s,128);
                    m.length = max(m.length,(uint32_t)getBitLength(-a ));
                    m.format = "n";
                } else {
                    __uint128_t a = parseUnsigned128(s,128);
                    m.length = max(m.length,(uint32_t)getBitLength(a));
                }
            }
        }
    }
}

string GenDefinitionBase::outputDefinition(const std::map<std::string,mapping::mapping_entry>&maps) {
    string ret;
    ret = R"({)";
    ret += '\n';
    ret += R"(  "mappings": {)";
    ret += '\n';
    ret += R"(      "properties": {)";
    ret += '\n';
    auto z = maps.size();

    for (const auto&i : maps) {
        ret += R"(         ")";
        ret+= i.first;
        ret += R"(" : { "type": ")";
        const auto m = i.second;
        if (m.max_array_size == 2 && m.format == "x"){
            ret += "location";
            ret += R"(" )";
        } else if (m.t == mapping::index_type_date_nano) {
            ret += "date_nano";
            ret += R"(" )";
        } else if (m.t == mapping::index_type_date) {
            ret +=  R"(date", "format" : "%F %T")";
        } else if (m.s == mapping::storage_type_unsigned) {
            ret += "uint" + to_string(m.length)+"_t";
            ret += R"(" )";
        } else if (m.s == mapping::storage_type_signed) {
            ret += "int" + to_string(m.length+1)+"_t";
            ret += R"(" )";
        }  else if (m.s == mapping::storage_type_float) {
            ret += "float";
            ret += R"(" )";
        } else if (m.s == mapping::storage_type_double) {
            ret += "double";
            ret += R"(" )";
        } else if (m.s == mapping::storage_type_unsigned_fixed_point) {
            ret += "uint" + to_string(m.length) + R"(_t" , "scale" : )";
            ret += to_string(m.scale);
        } else if (m.s == mapping::storage_type_signed_fixed_point) {
            ret += "int" + to_string(m.length+1) + R"(_t" , "scale" : )";
            ret += to_string(m.scale);
        } else if (m.t == mapping::index_type_enum) {
            if (m.length > 128) {
                ret += "enum";
            } else {
                if (m.format == "n") {
                    ret += "int" + to_string(m.length+1)+"_t";
                } else {
                    ret += "uint" + to_string(m.length)+"_t";
                }
            }
            ret += R"(" )";
        }
        if (m.max_array_size > 0 && !(m.max_array_size==2 && m.format == "x")) {
            ret += R"( , "max_array_size" : )";
            ret += to_string(m.max_array_size);
        }
        if (z > 1) {
            ret += R"(},)";
        } else {
            ret += R"(})";
        }
        ret += '\n';
        z--;
    }
    ret += R"(      })";
    ret += '\n';
    ret += R"(   })";
    ret += '\n';
    ret += R"(})";
    ret += '\n';
    return ret;
}

}