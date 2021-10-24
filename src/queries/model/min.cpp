#include "min.hpp"
#include "../../common/tools.hpp"

using namespace std;
using namespace mapping;

namespace queries::model {

void MinModel::initFunc(mapping::index_types t) {
    indexType = t;
}

__uint128_t MinModel::processGlobalFunc(const __uint128_t &a,const __uint128_t &prev) const {
    __uint128_t ret = prev;
    switch (indexType) {
        case index_type_none:
        case index_type_enum:
        case index_type_boolean:
        case index_type_geopoint:
        case index_type_string:
            break;
        case index_type_signed:
            ret = (__uint128_t)min(*(__int128_t*)&prev,SignExtend(a,length));
            break;
        case index_type_unsigned:
        case index_type_date:
        case index_type_date_nano:
            ret = min(*(__uint128_t*)&prev,a);
            break;
        case index_type_float16:
            *(float*)&ret = min(*(float*)&prev,halfFloatToFloat(a));
        case index_type_float:
            *(float*)&ret = min(*(float*)&prev,*(float*)&a);
            break;
        case index_type_double:
            *(double*)&ret = min(*(double*)&prev,*(double*)&a);
            break;
    }
    return ret;
}

__uint128_t MinModel::getGlobalFuncStartVal() const {
    __uint128_t ret = 0;
    switch (indexType) {
        case index_type_none:
        case index_type_enum:
        case index_type_boolean:
        case index_type_geopoint:
        case index_type_string:
            break;
        case index_type_signed:
            ret = (__uint128_t)std::numeric_limits<__int128_t>::max();
            break;
        case index_type_unsigned:
        case index_type_date:
        case index_type_date_nano:
            ret = std::numeric_limits<__uint128_t>::max();
            break;
        case index_type_float16:
        case index_type_float:
            *(float*)&ret = std::numeric_limits<float>::max();
            break;
        case index_type_double:
            *(double*)&ret = std::numeric_limits<double>::max();
            break;
    }
    return ret;
}

}