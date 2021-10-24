#include "avg.hpp"

using namespace std;
using namespace mapping;

namespace queries::model {

void AvgModel::initFunc(mapping::index_types t) {
    indexType = t;
}

__uint128_t AvgModel::processGlobalFunc(const __uint128_t &a,const __uint128_t &prev) const {
    __uint128_t ret = prev;
    switch (indexType) {
        case index_type_none:
        case index_type_enum:
        case index_type_boolean:
        case index_type_geopoint:
        case index_type_string:
            break;
        case index_type_signed:
            ret = (__uint128_t)((__int128_t)ret + SignExtend(a,length));
            break;
        case index_type_unsigned:
        case index_type_date:
        case index_type_date_nano:
            ret += a;
            break;
        case index_type_float16:
            *(float*)ret += halfFloatToFloat(a);
            break;
        case index_type_float:
            *(float*)ret += *(float*)&a;
            break;
        case index_type_double:
            *(double*)ret += *(double*)&a;
            break;
    }
    return ret;
}

__uint128_t AvgModel::getGlobalFuncStartVal() const {
    return 0;
}

__uint128_t AvgModel::getFuncResult(const vector<__uint128_t> &a, __uint128_t count) const {
    __uint128_t ret = getGlobalFuncStartVal();
    for (const auto &i: a) {
        switch (indexType) {
            case index_type_none:
            case index_type_enum:
            case index_type_boolean:
            case index_type_geopoint:
            case index_type_string:
                break;
            case index_type_signed:
                ret = (__uint128_t)((__int128_t)ret +(__int128_t)(i));
                break;
            case index_type_unsigned:
            case index_type_date:
            case index_type_date_nano:
                ret += i;
                break;
            case index_type_float16:
            case index_type_float:
                *(float*)ret += *(float*)&i;
                break;
            case index_type_double:
                *(double*)ret += *(double*)&i;
                break;
        }
    }

    switch (indexType) {
        case index_type_none:
        case index_type_enum:
        case index_type_boolean:
        case index_type_geopoint:
        case index_type_string:
            break;
        case index_type_signed:
            *(float*)&ret = (float)(__int128_t)ret/(float)count;
            break;
        case index_type_unsigned:
        case index_type_date:
        case index_type_date_nano:
            *(float*)&ret = (float)ret/(float)count;
            break;
        case index_type_float16:
        case index_type_float:
            *(float*)&ret = *(float*)&ret/(float)count;
            break;
        case index_type_double:
            *(float*)&ret = (float)(*(double*)&ret/(double)count);
            break;
    }
    return ret;
}

}