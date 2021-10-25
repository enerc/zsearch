#include <algorithm>
#include <execution>
#include "order.hpp"
#include "where.hpp"
#include "../builders/ast.hpp"
#include "../../repository/indexRepository.hpp"
#include "../../mapping/indexDefinitionManager.hpp"
#include "distanceSphere.hpp"
#include "point.hpp"
#include "../../devices/cpu/kernels.hpp"

using namespace std;
using namespace mapping;
using namespace indexes;

namespace queries::model {

static IndexManager *imSort0 = nullptr;     // one sort at a time even if the sort is multi-threaded
static IndexManager *imSort1 = nullptr;
static int64_t distanceSphereCenter0,distanceSphereCenter1;

typedef uint16_t float16;
typedef uint64_t enumType;

void OrderModel::execute(queries::builders::AST *_ast) {
    ast = _ast;
    uint64_t n = now();
    if (!exprList.empty()) {
        int k = 0;
        for (auto &i : exprList) {
            if (i->getClassInstanceType() == ClassInstanceDistanceSphere) {
                shared_ptr<DistanceSphereModel> distanceSphere = static_pointer_cast<DistanceSphereModel>(i);
                if (distanceSphere->getExprList().size() > 1) {
                    shared_ptr<PointModel> p = static_pointer_cast<PointModel>(distanceSphere->getExprList().at(1));
                    bool isFloat;
                    if (k == 0) {
                        distanceSphereCenter1 = distanceSphereCenter0 = (int64_t) p->getResult(isFloat);
                    } else {
                        distanceSphereCenter1 = (int64_t) p->getResult(isFloat);
                    }
                }
                k++;
            }
        }
    }

    if (!fieldsToOrder.empty())  {
        if (fieldsToOrder.size() == 1) {
            orderOnOne();
        } else if (fieldsToOrder.size() == 2) {
            orderOnTwo();
        } else {
            Log::error(ast->getQueryStatus(),"A maximum of two fields to sort on are currently supported.");
        }
    }
    Log::debug("Time spent in sorting: " + timeSpent(n) + ".");
}

void OrderModel::orderOnOne() {
    auto idx = columnMapping->getIndexOffset(fieldsToOrder.at(0));
    if (idx == ULLONG_MAX) {
        Log::error(ast->getQueryStatus(), "Order : invalid field to order on " + fieldsToOrder.at(0));
        return;
    }
    const auto me = columnMapping->getIndexDefinition(idx);
    if ((me->flags & EntryArraySet) != 0) {
        Log::error(ast->getQueryStatus(), "Ordering on array not implemented yet : " + fieldsToOrder.at(0));
        return;
    }
    if (where->getNbResults() >= 4294967295) {
        Log::error(ast->getQueryStatus(), "Sorting on more than 2^32 is not supported yet (and slow) : " + to_string(where->getNbResults()) + " items");
        return;
    }
    switch (me->s) {
        case storage_type_null:
        case storage_type_unknown:
            return;
        case storage_type_signed:
        case storage_type_signed_fixed_point:
            if (me->length <= 8) orderOne<char,Type_Numeric>();
            else if (me->length <= 16) orderOne<short,Type_Numeric>();
            else if (me->length <= 32) orderOne<int32_t,Type_Numeric>();
            else if (me->length <= 64) orderOne<int64_t,Type_Numeric>();
            else orderOne<__int128_t,Type_Numeric>();
            break;
        case storage_type_unsigned:
        case storage_type_unsigned_fixed_point:
            if (me->length <= 8) orderOne<unsigned char,Type_Numeric>();
            else if (me->length <= 16) orderOne<unsigned short,Type_Numeric>();
            else if (me->length <= 32) orderOne<uint32_t,Type_Numeric>();
            else if (me->length <= 64) orderOne<uint64_t,Type_Numeric>();
            else orderOne<__uint128_t,Type_Numeric>();
            break;
        case storage_type_float16:
            orderOne<float16,Type_Float16>();
            break;
        case storage_type_float:
            orderOne<float,Type_Numeric>();
            break;
        case storage_type_double:
            orderOne<double,Type_Numeric>();
            break;
        case storage_type_geopoint:
            orderOne<__uint64_t,Type_Distance>();
            break;
        case storage_type_enum:
            orderOne<uint64_t,Type_Enum>();
            break;
    }
}

void OrderModel::orderOnTwo() {
    auto idx0 = columnMapping->getIndexOffset(fieldsToOrder.at(0));
    if (idx0 == ULLONG_MAX) {
        Log::error(ast->getQueryStatus(), "Order : invalid field to order on " + fieldsToOrder.at(0));
        return;
    }
    auto idx1 = columnMapping->getIndexOffset(fieldsToOrder.at(1));
    if (idx1 == ULLONG_MAX) {
        Log::error(ast->getQueryStatus(), "Order : invalid field to order on " + fieldsToOrder.at(1));
        return;
    }

    const auto me0 = columnMapping->getIndexDefinition(idx0);
    if ((me0->flags & EntryArraySet) != 0) {
        Log::error(ast->getQueryStatus(), "Ordering on array not implemented yet : " + fieldsToOrder.at(0));
        return;
    }
    const auto &me1 = columnMapping->getIndexDefinition(idx1);
    if ((me1->flags & EntryArraySet) != 0) {
        Log::error(ast->getQueryStatus(), "Ordering on array not implemented yet : " + fieldsToOrder.at(1));
        return;
    }

    if (where->getNbResults() >= 4294967295) {
        Log::error(ast->getQueryStatus(), "Sorting on more than 2^32 is not supported yet (and slow) : " + to_string(where->getNbResults()) + " items");
        return;
    }

    switch (me0->s) {
        case storage_type_null:
        case storage_type_unknown:
            return;
        case storage_type_signed:
        case storage_type_signed_fixed_point:
            if (me0->length <= 8) {
                switch (me1->s) {
                    case storage_type_null:
                    case storage_type_unknown:
                        return;
                    case storage_type_signed:
                    case storage_type_signed_fixed_point:
                        if (me1->length <= 8) orderTwo<char,char,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 16) orderTwo<char,short,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 32) orderTwo<char,int,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 64) orderTwo<char,int64_t,Type_Numeric,Type_Numeric>();
                        else orderTwo<char,__int128_t,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_unsigned:
                    case storage_type_unsigned_fixed_point:
                        if (me1->length <= 8) orderTwo<char,unsigned char,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 16) orderTwo<char,unsigned short,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 32) orderTwo<char,unsigned int,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 64) orderTwo<char,uint64_t,Type_Numeric,Type_Numeric>();
                        else orderTwo<char,__uint128_t,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_float16:
                        orderTwo<char,float16,Type_Numeric,Type_Float16>();
                        break;
                    case storage_type_float:
                        orderTwo<char,float,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_double:
                        orderTwo<char,double,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_geopoint:
                        orderTwo<char,__uint64_t,Type_Numeric,Type_Distance>();
                        break;
                    case storage_type_enum:
                        orderTwo<char,enumType,Type_Numeric,Type_Enum>();
                        break;
                }
            }
            else if (me0->length <= 16) {
                switch (me1->s) {
                    case storage_type_null:
                    case storage_type_unknown:
                        return;
                    case storage_type_signed:
                    case storage_type_signed_fixed_point:
                        if (me1->length <= 8) orderTwo<short,char,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 16) orderTwo<short,short,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 32) orderTwo<short,int,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 64) orderTwo<short,int64_t,Type_Numeric,Type_Numeric>();
                        else orderTwo<short,__int128_t,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_unsigned:
                    case storage_type_unsigned_fixed_point:
                        if (me1->length <= 8) orderTwo<short,unsigned char,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 16) orderTwo<short,unsigned short,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 32) orderTwo<short,unsigned int,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 64) orderTwo<short,uint64_t,Type_Numeric,Type_Numeric>();
                        else orderTwo<short,__uint128_t,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_float16:
                        orderTwo<short,float16,Type_Numeric,Type_Float16>();
                        break;
                    case storage_type_float:
                        orderTwo<short,float,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_double:
                        orderTwo<short,double,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_geopoint:
                        orderTwo<short,__uint64_t,Type_Numeric,Type_Distance>();
                        break;
                    case storage_type_enum:
                        orderTwo<short,enumType,Type_Numeric,Type_Enum>();
                        break;
                }
            }
            else if (me0->length <= 32) {
                switch (me1->s) {
                    case storage_type_null:
                    case storage_type_unknown:
                        return;
                    case storage_type_signed:
                    case storage_type_signed_fixed_point:
                        if (me1->length <= 8) orderTwo<int32_t,char,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 16) orderTwo<int32_t,short,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 32) orderTwo<int32_t,int32_t,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 64) orderTwo<int32_t,int64_t,Type_Numeric,Type_Numeric>();
                        else orderTwo<int,__int128_t,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_unsigned:
                    case storage_type_unsigned_fixed_point:
                        if (me1->length <= 8) orderTwo<int32_t,unsigned char,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 16) orderTwo<int32_t,unsigned short,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 32) orderTwo<int32_t,uint32_t,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 64) orderTwo<int32_t,uint64_t,Type_Numeric,Type_Numeric>();
                        else orderTwo<int32_t,__uint128_t,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_float16:
                        orderTwo<int32_t,float16,Type_Numeric,Type_Float16>();
                        break;
                    case storage_type_float:
                        orderTwo<int32_t,float,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_double:
                        orderTwo<int32_t,double,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_geopoint:
                        orderTwo<int32_t,__uint64_t,Type_Numeric,Type_Distance>();
                        break;
                    case storage_type_enum:
                        orderTwo<int32_t,enumType,Type_Numeric,Type_Enum>();
                        break;
                }
            }
            else if (me0->length <= 64) {
                switch (me1->s) {
                    case storage_type_null:
                    case storage_type_unknown:
                        return;
                    case storage_type_signed:
                    case storage_type_signed_fixed_point:
                        if (me1->length <= 8) orderTwo<int64_t,char,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 16) orderTwo<int64_t,short,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 32) orderTwo<int64_t,int32_t,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 64) orderTwo<int64_t,int64_t,Type_Numeric,Type_Numeric>();
                        else orderTwo<int64_t,__int128_t,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_unsigned:
                    case storage_type_unsigned_fixed_point:
                        if (me1->length <= 8) orderTwo<int64_t,unsigned char,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 16) orderTwo<int64_t,unsigned short,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 32) orderTwo<int64_t,uint32_t,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 64) orderTwo<int64_t,uint64_t,Type_Numeric,Type_Numeric>();
                        else orderTwo<int64_t,__uint128_t,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_float16:
                        orderTwo<int64_t,float16,Type_Numeric,Type_Float16>();
                        break;
                    case storage_type_float:
                        orderTwo<int64_t,float,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_double:
                        orderTwo<int64_t,double,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_geopoint:
                        orderTwo<int64_t,__uint64_t,Type_Numeric,Type_Distance>();
                        break;
                    case storage_type_enum:
                        orderTwo<int64_t,enumType,Type_Numeric,Type_Enum>();
                        break;
                }
            }
            else {
                switch (me1->s) {
                    case storage_type_null:
                    case storage_type_unknown:
                        return;
                    case storage_type_signed:
                    case storage_type_signed_fixed_point:
                        if (me1->length <= 8) orderTwo<__int128_t, char,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 16) orderTwo<__int128_t, short,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 32) orderTwo<__int128_t, int32_t,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 64) orderTwo<__int128_t, int64_t,Type_Numeric,Type_Numeric>();
                        else orderTwo<__int128_t, __int128_t,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_unsigned:
                    case storage_type_unsigned_fixed_point:
                        if (me1->length <= 8) orderTwo<__int128_t, unsigned char,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 16) orderTwo<__int128_t, unsigned short,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 32) orderTwo<__int128_t, uint32_t,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 64) orderTwo<__int128_t, uint64_t,Type_Numeric,Type_Numeric>();
                        else orderTwo<__int128_t, __uint128_t,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_float16:
                        orderTwo<__int128_t, float16,Type_Numeric,Type_Float16>();
                        break;
                    case storage_type_float:
                        orderTwo<__int128_t, float,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_double:
                        orderTwo<__int128_t, double,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_geopoint:
                        orderTwo<__int128_t,__uint64_t,Type_Numeric,Type_Distance>();
                        break;
                    case storage_type_enum:
                        orderTwo<__int128_t, enumType,Type_Numeric,Type_Enum>();
                        break;
                }
            }
            break;
        case storage_type_unsigned:
        case storage_type_unsigned_fixed_point:
            if (me0->length <= 8) {
                switch (me1->s) {
                    case storage_type_null:
                    case storage_type_unknown:
                        return;
                    case storage_type_signed:
                    case storage_type_signed_fixed_point:
                        if (me1->length <= 8) orderTwo<unsigned char,char,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 16) orderTwo<unsigned char,short,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 32) orderTwo<unsigned char,int32_t,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 64) orderTwo<unsigned char,int64_t,Type_Numeric,Type_Numeric>();
                        else orderTwo<unsigned char,__int128_t,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_unsigned:
                    case storage_type_unsigned_fixed_point:
                        if (me1->length <= 8) orderTwo<unsigned char,unsigned char,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 16) orderTwo<unsigned char,unsigned short,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 32) orderTwo<unsigned char,uint32_t,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 64) orderTwo<unsigned char,uint64_t,Type_Numeric,Type_Numeric>();
                        else orderTwo<unsigned char,__uint128_t,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_float16:
                        orderTwo<unsigned char,float16,Type_Numeric,Type_Float16>();
                        break;
                    case storage_type_float:
                        orderTwo<unsigned char,float,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_double:
                        orderTwo<unsigned char,double,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_geopoint:
                        orderTwo<unsigned char,__uint64_t,Type_Numeric,Type_Distance>();
                        break;
                    case storage_type_enum:
                        orderTwo<unsigned char,enumType,Type_Numeric,Type_Enum>();
                        break;
                }
            }
            else if (me0->length <= 16) {
                switch (me1->s) {
                    case storage_type_null:
                    case storage_type_unknown:
                        return;
                    case storage_type_signed:
                    case storage_type_signed_fixed_point:
                        if (me1->length <= 8) orderTwo<unsigned short,char,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 16) orderTwo<unsigned short,short,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 32) orderTwo<unsigned short,int32_t,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 64) orderTwo<unsigned short,int64_t,Type_Numeric,Type_Numeric>();
                        else orderTwo<unsigned short,__int128_t,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_unsigned:
                    case storage_type_unsigned_fixed_point:
                        if (me1->length <= 8) orderTwo<unsigned short,unsigned char,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 16) orderTwo<unsigned short,unsigned short,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 32) orderTwo<unsigned short,uint32_t,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 64) orderTwo<unsigned short,uint64_t,Type_Numeric,Type_Numeric>();
                        else orderTwo<unsigned short,__uint128_t,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_float16:
                        orderTwo<unsigned short,float16,Type_Numeric,Type_Float16>();
                        break;
                    case storage_type_float:
                        orderTwo<unsigned short,float,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_double:
                        orderTwo<unsigned short,double,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_geopoint:
                        orderTwo<unsigned short,__uint64_t,Type_Numeric,Type_Distance>();
                        break;
                    case storage_type_enum:
                        orderTwo<unsigned short,enumType,Type_Numeric,Type_Enum>();
                        break;
                }
            }
            else if (me0->length <= 32) {
                switch (me1->s) {
                    case storage_type_null:
                    case storage_type_unknown:
                        return;
                    case storage_type_signed:
                    case storage_type_signed_fixed_point:
                        if (me1->length <= 8) orderTwo<uint32_t,char,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 16) orderTwo<uint32_t,short,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 32) orderTwo<uint32_t,int32_t,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 64) orderTwo<uint32_t,int64_t,Type_Numeric,Type_Numeric>();
                        else orderTwo<uint32_t,__int128_t,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_unsigned:
                    case storage_type_unsigned_fixed_point:
                        if (me1->length <= 8) orderTwo<uint32_t,unsigned char,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 16) orderTwo<uint32_t,unsigned short,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 32) orderTwo<uint32_t,uint32_t,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 64) orderTwo<uint32_t,uint64_t,Type_Numeric,Type_Numeric>();
                        else orderTwo<uint32_t,__uint128_t,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_float16:
                        orderTwo<uint32_t,float16,Type_Numeric,Type_Float16>();
                        break;
                    case storage_type_float:
                        orderTwo<uint32_t,float,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_double:
                        orderTwo<uint32_t,double,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_geopoint:
                        orderTwo<uint32_t,__uint64_t,Type_Numeric,Type_Distance>();
                        break;
                    case storage_type_enum:
                        orderTwo<uint32_t,enumType,Type_Numeric,Type_Enum>();
                        break;
                }
            }
            else if (me0->length <= 64) {
                switch (me1->s) {
                    case storage_type_null:
                    case storage_type_unknown:
                        return;
                    case storage_type_signed:
                    case storage_type_signed_fixed_point:
                        if (me1->length <= 8) orderTwo<uint64_t,char,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 16) orderTwo<uint64_t,short,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 32) orderTwo<uint64_t,int32_t,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 64) orderTwo<uint64_t,int64_t,Type_Numeric,Type_Numeric>();
                        else orderTwo<uint64_t,__int128_t,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_unsigned:
                    case storage_type_unsigned_fixed_point:
                        if (me1->length <= 8) orderTwo<uint64_t,unsigned char,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 16) orderTwo<uint64_t,unsigned short,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 32) orderTwo<uint64_t,uint32_t,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 64) orderTwo<uint64_t,uint64_t,Type_Numeric,Type_Numeric>();
                        else orderTwo<uint64_t,__uint128_t,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_float16:
                        orderTwo<uint64_t,float16,Type_Numeric,Type_Float16>();
                        break;
                    case storage_type_float:
                        orderTwo<uint64_t,float,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_double:
                        orderTwo<uint64_t,double,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_geopoint:
                        orderTwo<uint64_t,__uint64_t,Type_Numeric,Type_Distance>();
                        break;
                    case storage_type_enum:
                        orderTwo<uint64_t,enumType,Type_Numeric,Type_Enum>();
                        break;
                }
            }
            else {
                switch (me1->s) {
                    case storage_type_null:
                    case storage_type_unknown:
                        return;
                    case storage_type_signed:
                    case storage_type_signed_fixed_point:
                        if (me1->length <= 8) orderTwo<__uint128_t, char,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 16) orderTwo<__uint128_t, short,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 32) orderTwo<__uint128_t, int32_t,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 64) orderTwo<__uint128_t, int64_t,Type_Numeric,Type_Numeric>();
                        else orderTwo<__uint128_t, __int128_t,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_unsigned:
                    case storage_type_unsigned_fixed_point:
                        if (me1->length <= 8) orderTwo<__uint128_t, unsigned char,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 16) orderTwo<__uint128_t, unsigned short,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 32) orderTwo<__uint128_t, uint32_t,Type_Numeric,Type_Numeric>();
                        else if (me1->length <= 64) orderTwo<__uint128_t, uint64_t,Type_Numeric,Type_Numeric>();
                        else orderTwo<__uint128_t, __uint128_t,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_float16:
                        orderTwo<__int128_t, float16,Type_Numeric,Type_Float16>();
                        break;
                    case storage_type_float:
                        orderTwo<__int128_t, float,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_double:
                        orderTwo<__int128_t, double,Type_Numeric,Type_Numeric>();
                        break;
                    case storage_type_geopoint:
                        orderTwo<__int128_t,__uint64_t,Type_Numeric,Type_Distance>();
                        break;
                    case storage_type_enum:
                        orderTwo<__int128_t, enumType,Type_Numeric,Type_Enum>();
                        break;
                }
            }
            break;
        case storage_type_float16:
            switch (me1->s) {
                case storage_type_null:
                case storage_type_unknown:
                    return;
                case storage_type_signed:
                case storage_type_signed_fixed_point:
                    if (me1->length <= 8) orderTwo<float16,char,Type_Float16,Type_Numeric>();
                    else if (me1->length <= 16) orderTwo<float16,short,Type_Float16,Type_Numeric>();
                    else if (me1->length <= 32) orderTwo<float16,int32_t,Type_Float16,Type_Numeric>();
                    else if (me1->length <= 64) orderTwo<float16,int64_t,Type_Float16,Type_Numeric>();
                    else orderTwo<float16,__int128_t,Type_Float16,Type_Numeric>();
                    break;
                case storage_type_unsigned:
                case storage_type_unsigned_fixed_point:
                    if (me1->length <= 8) orderTwo<float16,unsigned char,Type_Float16,Type_Numeric>();
                    else if (me1->length <= 16) orderTwo<float16,unsigned short,Type_Float16,Type_Numeric>();
                    else if (me1->length <= 32) orderTwo<float16,uint32_t,Type_Float16,Type_Numeric>();
                    else if (me1->length <= 64) orderTwo<float16,uint64_t,Type_Float16,Type_Numeric>();
                    else orderTwo<float16,__uint128_t,Type_Float16,Type_Numeric>();
                    break;
                case storage_type_float16:
                    orderTwo<float16,float16,Type_Float16,Type_Float16>();
                    break;
                case storage_type_float:
                    orderTwo<float16,float,Type_Float16,Type_Numeric>();
                    break;
                case storage_type_double:
                    orderTwo<float16,double,Type_Float16,Type_Numeric>();
                    break;
                case storage_type_geopoint:
                    orderTwo<float16,__uint64_t,Type_Numeric,Type_Distance>();
                    break;
                case storage_type_enum:
                    orderTwo<float16,enumType,Type_Float16,Type_Enum>();
                    break;
            }
            break;
        case storage_type_float:
            switch (me1->s) {
                case storage_type_null:
                case storage_type_unknown:
                    return;
                case storage_type_signed:
                case storage_type_signed_fixed_point:
                    if (me1->length <= 8) orderTwo<float,char,Type_Numeric,Type_Numeric>();
                    else if (me1->length <= 16) orderTwo<float,short,Type_Numeric,Type_Numeric>();
                    else if (me1->length <= 32) orderTwo<float,int32_t,Type_Numeric,Type_Numeric>();
                    else if (me1->length <= 64) orderTwo<float,int64_t,Type_Numeric,Type_Numeric>();
                    else orderTwo<float,__int128_t,Type_Numeric,Type_Numeric>();
                    break;
                case storage_type_unsigned:
                case storage_type_unsigned_fixed_point:
                    if (me1->length <= 8) orderTwo<float,unsigned char,Type_Numeric,Type_Numeric>();
                    else if (me1->length <= 16) orderTwo<float,unsigned short,Type_Numeric,Type_Numeric>();
                    else if (me1->length <= 32) orderTwo<float,uint32_t,Type_Numeric,Type_Numeric>();
                    else if (me1->length <= 64) orderTwo<float,uint64_t,Type_Numeric,Type_Numeric>();
                    else orderTwo<float,__uint128_t,Type_Numeric,Type_Numeric>();
                    break;
                case storage_type_float16:
                    orderTwo<float,float16,Type_Numeric,Type_Float16>();
                    break;
                case storage_type_float:
                    orderTwo<float,float,Type_Numeric,Type_Numeric>();
                    break;
                case storage_type_double:
                    orderTwo<float,double,Type_Numeric,Type_Numeric>();
                    break;
                case storage_type_geopoint:
                    orderTwo<float,__uint64_t,Type_Numeric,Type_Distance>();
                    break;
                case storage_type_enum:
                    orderTwo<float,enumType,Type_Numeric,Type_Enum>();
                    break;
            }
            break;
        case storage_type_double:
            switch (me1->s) {
                case storage_type_null:
                case storage_type_unknown:
                    return;
                case storage_type_signed:
                case storage_type_signed_fixed_point:
                    if (me1->length <= 8) orderTwo<double,char,Type_Numeric,Type_Numeric>();
                    else if (me1->length <= 16) orderTwo<double,short,Type_Numeric,Type_Numeric>();
                    else if (me1->length <= 32) orderTwo<double,int32_t,Type_Numeric,Type_Numeric>();
                    else if (me1->length <= 64) orderTwo<double,int64_t,Type_Numeric,Type_Numeric>();
                    else orderTwo<double,__int128_t,Type_Numeric,Type_Numeric>();
                    break;
                case storage_type_unsigned:
                case storage_type_unsigned_fixed_point:
                    if (me1->length <= 8) orderTwo<double,unsigned char,Type_Numeric,Type_Numeric>();
                    else if (me1->length <= 16) orderTwo<double,unsigned short,Type_Numeric,Type_Numeric>();
                    else if (me1->length <= 32) orderTwo<double,uint32_t,Type_Numeric,Type_Numeric>();
                    else if (me1->length <= 64) orderTwo<double,uint64_t,Type_Numeric,Type_Numeric>();
                    else orderTwo<double,__uint128_t,Type_Numeric,Type_Numeric>();
                    break;
                case storage_type_float16:
                    orderTwo<double,float16,Type_Numeric,Type_Float16>();
                    break;
                case storage_type_float:
                    orderTwo<double,float,Type_Numeric,Type_Numeric>();
                    break;
                case storage_type_double:
                    orderTwo<double,double,Type_Numeric,Type_Numeric>();
                    break;
                case storage_type_geopoint:
                    orderTwo<double,__uint64_t,Type_Numeric,Type_Distance>();
                    break;
                case storage_type_enum:
                    orderTwo<double,enumType,Type_Numeric,Type_Enum>();
                    break;
            }
            break;
        case storage_type_geopoint:
            switch (me1->s) {
                case storage_type_null:
                case storage_type_unknown:
                    return;
                case storage_type_signed:
                case storage_type_signed_fixed_point:
                    if (me1->length <= 8) orderTwo<__uint64_t,char,Type_Distance,Type_Numeric>();
                    else if (me1->length <= 16) orderTwo<__uint64_t,short,Type_Distance,Type_Numeric>();
                    else if (me1->length <= 32) orderTwo<__uint64_t,int32_t,Type_Distance,Type_Numeric>();
                    else if (me1->length <= 64) orderTwo<__uint64_t,int64_t,Type_Distance,Type_Numeric>();
                    else orderTwo<__uint64_t,__int128_t,Type_Distance,Type_Numeric>();
                    break;
                case storage_type_unsigned:
                case storage_type_unsigned_fixed_point:
                    if (me1->length <= 8) orderTwo<__uint64_t,unsigned char,Type_Distance,Type_Numeric>();
                    else if (me1->length <= 16) orderTwo<__uint64_t,unsigned short,Type_Distance,Type_Numeric>();
                    else if (me1->length <= 32) orderTwo<__uint64_t,uint32_t,Type_Distance,Type_Numeric>();
                    else if (me1->length <= 64) orderTwo<__uint64_t,uint64_t,Type_Distance,Type_Numeric>();
                    else orderTwo<__uint64_t,__uint128_t,Type_Distance,Type_Numeric>();
                    break;
                case storage_type_float16:
                    orderTwo<__uint64_t,float16,Type_Distance,Type_Float16>();
                    break;
                case storage_type_float:
                    orderTwo<__uint64_t,float,Type_Distance,Type_Numeric>();
                    break;
                case storage_type_double:
                    orderTwo<__uint64_t,double,Type_Distance,Type_Numeric>();
                    break;
                case storage_type_geopoint:
                    orderTwo<__uint64_t,__uint64_t,Type_Distance,Type_Distance>();
                    break;
                case storage_type_enum:
                    orderTwo<__uint64_t,enumType,Type_Distance,Type_Enum>();
                    break;
            }
            break;
        case storage_type_enum:
            switch (me1->s) {
                case storage_type_null:
                case storage_type_unknown:
                    return;
                case storage_type_signed:
                case storage_type_signed_fixed_point:
                    if (me1->length <= 8) orderTwo<enumType,char,Type_Enum,Type_Numeric>();
                    else if (me1->length <= 16) orderTwo<enumType,short,Type_Enum,Type_Numeric>();
                    else if (me1->length <= 32) orderTwo<enumType,int32_t,Type_Enum,Type_Numeric>();
                    else if (me1->length <= 64) orderTwo<enumType,int64_t,Type_Enum,Type_Numeric>();
                    else orderTwo<enumType,__int128_t,Type_Enum,Type_Numeric>();
                    break;
                case storage_type_unsigned:
                case storage_type_unsigned_fixed_point:
                    if (me1->length <= 8) orderTwo<enumType,unsigned char,Type_Enum,Type_Numeric>();
                    else if (me1->length <= 16) orderTwo<enumType,unsigned short,Type_Enum,Type_Numeric>();
                    else if (me1->length <= 32) orderTwo<enumType,uint32_t,Type_Enum,Type_Numeric>();
                    else if (me1->length <= 64) orderTwo<enumType,uint64_t,Type_Enum,Type_Numeric>();
                    else orderTwo<enumType,__uint128_t,Type_Enum,Type_Numeric>();
                    break;
                case storage_type_float16:
                    orderTwo<enumType,float16,Type_Enum,Type_Float16>();
                    break;
                case storage_type_float:
                    orderTwo<enumType,float,Type_Enum,Type_Numeric>();
                    break;
                case storage_type_double:
                    orderTwo<enumType,double,Type_Enum,Type_Numeric>();
                    break;
                case storage_type_geopoint:
                    orderTwo<enumType,__uint64_t,Type_Numeric,Type_Distance>();
                    break;
                case storage_type_enum:
                    orderTwo<enumType,enumType,Type_Enum,Type_Enum>();
                    break;
            }
            break;
    }

}

template <typename T>
struct DataSortOne  {
    char v[4+sizeof(T)];
};

template<typename T> T DoGetSame(const T &a) { return a;}
template<typename T,typename Tx> Tx DoGetFloat16(const T &a) { return halfFloatToFloat(a);}
template<typename T, typename Tx> Tx DoGetDistance0(const T &a) { return distance_to_center(a,distanceSphereCenter0);}
template<typename T, typename Tx> Tx DoGetDistance1(const T &a) { return distance_to_center(a,distanceSphereCenter1);}
template<typename T> bool DoCompareNumbersAsc(const T &a, const T &b) { return a < b;}
template<typename T> bool DoCompareNumbersDesc(const T &a, const T &b) { return a > b;}
template<typename T> bool DoCompareNumbersEq(const T &a, const T &b) { return a == b;}
template<typename T> bool DoCompareEnum0Asc(const T &a, const T &b) {  return imSort0->compareTwoEnumsAsc(a,b); }
template<typename T> bool DoCompareEnum0Desc(const T&a, const T &b) {  return imSort0->compareTwoEnumsDesc(a,b); }
template<typename T> bool DoCompareEnum0Eq(const T &a , const T &b) {  return imSort0->compareTwoEnumsEq(a,b); }
template<typename T> bool DoCompareEnum1Asc(const T &a, const T &b) {  return imSort1->compareTwoEnumsAsc(a,b); }
template<typename T> bool DoCompareEnum1Desc(const T &a, const T &b) {  return imSort1->compareTwoEnumsDesc(a,b); }

class GetSame {};
class GetFloat16 {};
class GetDistance {};
class CompareNumbersAsc {};
class CompareNumbersDesc {};
class CompareNumbersEq {};
class CompareEnum0Asc {};
class CompareEnum0Desc {};
class CompareEnum0Eq {};
class CompareEnum1Asc {};
class CompareEnum1Desc {};

template<typename T,typename Tx,class GetFunc,class CompareFunc> static bool sortOne(const DataSortOne<T> &lhs, const DataSortOne<T> &rhs) {
    Tx (*funcT)(const T&a);
    if constexpr(is_same<GetFunc,GetSame>::value) funcT = DoGetSame<T>;
    if constexpr(is_same<GetFunc,GetFloat16>::value) funcT = DoGetFloat16<T,Tx>;
    if constexpr(is_same<GetFunc,GetDistance>::value) funcT = DoGetDistance0<T,Tx>;

    bool (*cmpT)(const Tx&a, const Tx&b);
    if constexpr(is_same<CompareFunc,CompareNumbersAsc>::value) cmpT = DoCompareNumbersAsc<Tx>;
    if constexpr(is_same<CompareFunc,CompareNumbersDesc>::value) cmpT = DoCompareNumbersDesc<Tx>;
    if constexpr(is_same<CompareFunc,CompareEnum0Asc>::value) cmpT = DoCompareEnum0Asc<Tx>;
    if constexpr(is_same<CompareFunc,CompareEnum0Desc>::value) cmpT = DoCompareEnum0Desc<Tx>;

    return cmpT(funcT(* (T*)(lhs.v+4)),funcT(* (T*)(rhs.v+4)));
}

template<typename T, typename sortDataType> void OrderModel::orderOne() {
    const int blocSize = 4+sizeof(T);
    columnMapped = IndexDefinitionManager::getIndexManager(fieldsToOrder.at(0), fromTables, ast->getQueryStatus());
    if (columnMapped == nullptr) {
        return; // error set in getIndexManager
    }
    imSort0 = IndexRepository::getInstance()->getIndex(columnMapped->getIndexName(), fieldsToOrder.at(0)).get();
    if (imSort0 == nullptr) {
        return; // should not happen
    }
    shared_ptr<IndexChunk> chunk = nullptr;
    uint32_t chunkIdx = -1;
    auto ws = where->getWorkingSet();
    auto iMax = where->getWorkingSetSize()/8;
    uint32_t o = 0;
    __uint128_t v;
    bool isNull;
    bool endOfDocument;
    bool isDeleted;
    sortDataRaw = malloc(where->getNbResults()*sizeof(DataSortOne<T>));
    if (sortDataRaw == nullptr) {
        Log::error(ast->getQueryStatus(), "Not enough memory to process sorting.");
        return;
    }
    typedef DataSortOne<T> myDataSort;
    vector<myDataSort,PreAllocator<myDataSort>> dataSort(where->getNbResults(),PreAllocator<myDataSort>((myDataSort*)sortDataRaw, where->getNbResults()));

    // Fill
    for (uint64_t i=0; i < iMax; i++) {
        if (ws[i] != 0) {
            uint64_t a = ws[i];
            if ((i*64)>>CHUNK_SIZE_LOG2 != chunkIdx) {
                chunkIdx = (i*64)>>CHUNK_SIZE_LOG2;
                chunk = imSort0->getChunkForRead(chunkIdx);
            }
            int j;
            while ((j=__builtin_ffsl((long)a)) > 0) {
                char *p = ((char*)sortDataRaw)+o*blocSize; // &dataSort.at(o);
                auto *index = (uint32_t*)p;
                uint64_t loc = i*64 + j -1;
                uint32_t cloc = loc - ((loc>>CHUNK_SIZE_LOG2)<<CHUNK_SIZE_LOG2);
                chunk->getBase()->getValue(cloc,v,isNull,endOfDocument,isDeleted);
                if (is_same<T, enumType>::value) {
                    v |= ((uint64_t)chunkIdx) << 32;
                }
                *index = loc;
                T *m = (T*)(p+4);
                *m = (T)v;
                a &= ~(1L<<(j-1));
                o++;
            }
        }
    }

    // Sort
    if constexpr (is_same<sortDataType,Type_Float16>::value) {
        sort(execution::par_unseq, dataSort.begin(), dataSort.end(), orderAsc.at(0) ? sortOne<T,float,GetFloat16,CompareNumbersAsc> : sortOne<T,float,GetFloat16,CompareNumbersDesc>);
    } else if constexpr(is_same<sortDataType,Type_Enum>::value) {
        sort(execution::par_unseq, dataSort.begin(), dataSort.end(), orderAsc.at(0) ? sortOne<T,T,GetSame,CompareEnum0Asc> : sortOne<T,T,GetSame,CompareEnum0Desc>);
    } else if constexpr (is_same<sortDataType,Type_Distance>::value) {
        sort(execution::par_unseq, dataSort.begin(), dataSort.end(), orderAsc.at(0) ? sortOne<T,float,GetDistance,CompareNumbersAsc> : sortOne<T,float,GetDistance,CompareNumbersDesc>);
    } else {
        sort(execution::par_unseq, dataSort.begin(), dataSort.end(), orderAsc.at(0) ? sortOne<T,T,GetSame,CompareNumbersAsc> : sortOne<T,T,GetSame,CompareNumbersDesc>);
    }
    pack(o,blocSize);
}

template <typename T,typename U>
struct DataSortTwo  {
    char v[4+sizeof(T)+sizeof(U)];
};


template <typename T,typename Tx,typename U,typename Ux, typename GetFuncT, typename GetFuncU, typename CompareT, typename CompareU, typename OperEQT> static bool sortTwo(const DataSortTwo<T,U> &lhs, const DataSortTwo<T,U> &rhs) {
    Tx (*funcT)(const T&a);
    if constexpr(is_same<GetFuncT,GetSame>::value) funcT = DoGetSame<T>;
    if constexpr(is_same<GetFuncT,GetFloat16>::value) funcT = DoGetFloat16<T,Tx>;
    if constexpr(is_same<GetFuncT,GetDistance>::value) funcT = DoGetDistance0<T,Tx>;

    Ux (*funcU)(const U&a);
    if constexpr(is_same<GetFuncU,GetSame>::value) funcU = DoGetSame<U>;
    if constexpr(is_same<GetFuncU,GetFloat16>::value) funcU = DoGetFloat16<U,Ux>;
    if constexpr(is_same<GetFuncU,GetDistance>::value) funcU = DoGetDistance1<U,Ux>;

    bool (*cmpT)(const Tx&a, const Tx&b);
    if constexpr(is_same<CompareT,CompareNumbersAsc>::value) cmpT = DoCompareNumbersAsc<Tx>;
    if constexpr(is_same<CompareT,CompareNumbersDesc>::value) cmpT = DoCompareNumbersDesc<Tx>;
    if constexpr(is_same<CompareT,CompareEnum0Asc>::value) cmpT = DoCompareEnum0Asc<Tx>;
    if constexpr(is_same<CompareT,CompareEnum0Desc>::value) cmpT = DoCompareEnum0Desc<Tx>;

    bool (*cmpU)(const Ux&a, const Ux&b);
    if constexpr(is_same<CompareU,CompareNumbersAsc>::value) cmpU = DoCompareNumbersAsc<Ux>;
    if constexpr(is_same<CompareU,CompareNumbersDesc>::value) cmpU = DoCompareNumbersDesc<Ux>;
    if constexpr(is_same<CompareU,CompareEnum1Asc>::value) cmpU = DoCompareEnum1Asc<Ux>;
    if constexpr(is_same<CompareU,CompareEnum1Desc>::value) cmpU = DoCompareEnum1Desc<Ux>;

    bool (*cmpEq)(const Tx&a, const Tx&b);
    if constexpr(is_same<OperEQT,CompareNumbersEq>::value) cmpEq = DoCompareNumbersEq<Tx>;
    if constexpr(is_same<OperEQT,CompareEnum0Eq>::value) cmpEq = DoCompareEnum0Eq<Tx>;

    return cmpT(funcT(* (T*)(lhs.v+4)),funcT(* (T*)(rhs.v+4))) || (
            cmpEq(funcT(* (T*)(lhs.v+4)),funcT(* (T*)(rhs.v+4))) &&
            cmpU(funcU(* (U*)(lhs.v+4+sizeof(T))),funcU(* (U*)(rhs.v+4+sizeof(T))))
            );
}

bool OrderModel::prepareOrderTwo(int sizeObject) {
    columnMapped = IndexDefinitionManager::getIndexManager(fieldsToOrder.at(0), fromTables,ast->getQueryStatus());
    if (columnMapped == nullptr) {
        return false;
    }
    imSort0 = IndexRepository::getInstance()->getIndex(columnMapped->getIndexName(), fieldsToOrder.at(0)).get();
    if (imSort0 == nullptr) {
        return false; // should not happen
    }
    columnMapped = IndexDefinitionManager::getIndexManager(fieldsToOrder.at(1), fromTables,ast->getQueryStatus());
    if (columnMapped == nullptr) {
        return false; // error set in getIndexManager
    }
    imSort1 = IndexRepository::getInstance()->getIndex(columnMapped->getIndexName(), fieldsToOrder.at(1)).get();
    if (imSort1 == nullptr) {
        return false; // should not happen
    }
    sortDataRaw = malloc(where->getNbResults()*sizeObject);
    if (sortDataRaw == nullptr) {
        Log::error(ast->getQueryStatus(), "Not enough memory to process sorting.");
        return false;
    }
    return true;
}

template<typename T,typename U,typename dt1,typename dt2> void OrderModel::orderTwo() {
    const int blocSize = 4+sizeof(T)+sizeof(U);
    if (!prepareOrderTwo(sizeof(DataSortTwo<T,U>))) {
        return;
    }

    shared_ptr<IndexChunk> chunk0 = nullptr;
    shared_ptr<IndexChunk> chunk1 = nullptr;
    uint32_t chunkIdx = -1;
    auto ws = where->getWorkingSet();
    auto iMax = where->getWorkingSetSize()/8;
    uint32_t o = 0;
    __uint128_t v;
    bool isNull;
    bool endOfDocument;
    bool isDeleted;
    typedef DataSortTwo<T,U> myDataSort;
    vector<myDataSort,PreAllocator<myDataSort>> dataSort(where->getNbResults(),PreAllocator<myDataSort>((myDataSort*)sortDataRaw, where->getNbResults()));

    // Fill
    for (uint64_t i=0; i < iMax; i++) {
        if (ws[i] != 0) {
            uint64_t a = ws[i];
            if ((i*64)>>CHUNK_SIZE_LOG2 != chunkIdx) {
                chunkIdx = (i*64)>>CHUNK_SIZE_LOG2;
                chunk0 = imSort0->getChunkForRead(chunkIdx);
                chunk1 = imSort1->getChunkForRead(chunkIdx);
            }
            int j;
            while ((j=__builtin_ffsl((long)a)) > 0) {
                char *p = ((char*)sortDataRaw)+o*blocSize; // &dataSort.at(o);
                auto *index = (uint32_t*)p;
                uint64_t loc = i*64 + j -1;
                uint32_t cloc = loc - ((loc>>CHUNK_SIZE_LOG2)<<CHUNK_SIZE_LOG2);
                chunk0->getBase()->getValue(cloc,v,isNull,endOfDocument,isDeleted);
                if (is_same<T, enumType>::value) {
                    v |= ((uint64_t)chunkIdx) << 32;
                }
                *index = loc;
                T *m = (T*)(p+4);
                *m = (T)v;

                chunk1->getBase()->getValue(cloc,v,isNull,endOfDocument,isDeleted);
                if (is_same<U, enumType>::value) {
                    v |= ((uint64_t)chunkIdx) << 32;
                }
                U *n = (U*)(p+4+sizeof(T));
                *n = (U)v;

                a &= ~(1L<<(j-1));
                o++;
            }
        }
    }

    // Sort
    if constexpr(is_same<dt1,Type_Float16>::value) {
        if constexpr(is_same<dt2,Type_Float16>::value) {
            sort(execution::par_unseq, dataSort.begin(), dataSort.end(),
                 orderAsc.at(0) ?
                 (orderAsc.at(1) ?  sortTwo<T,float,U,float,GetFloat16,GetFloat16,CompareNumbersAsc,CompareNumbersAsc,CompareNumbersEq> :
                                       sortTwo<T,float,U,float,GetFloat16,GetFloat16,CompareNumbersAsc,CompareNumbersDesc,CompareNumbersEq>) :
                 (orderAsc.at(1) ? sortTwo<T,float,U,float,GetFloat16,GetFloat16,CompareNumbersDesc,CompareNumbersAsc,CompareNumbersEq>
                                    : sortTwo<T,float,U,float,GetFloat16,GetFloat16,CompareNumbersDesc,CompareNumbersDesc,CompareNumbersEq>) );
        } else if constexpr(is_same<dt2, Type_Enum>::value) {
            sort(execution::par_unseq, dataSort.begin(), dataSort.end(),
                orderAsc.at(0) ?
                (orderAsc.at(1) ? sortTwo<T,float,U,U,GetFloat16,GetSame,CompareNumbersAsc, CompareEnum1Asc,CompareNumbersEq>
                                   : sortTwo<T,float,U,U,GetFloat16,GetSame,CompareNumbersAsc,CompareEnum1Desc,CompareNumbersEq>) :
                (orderAsc.at(1) ? sortTwo<T,float,U,U,GetFloat16,GetSame,CompareNumbersDesc,CompareEnum1Asc,CompareNumbersEq>
                                   : sortTwo<T,float,U,U,GetFloat16,GetSame,CompareNumbersDesc,CompareEnum1Desc,CompareNumbersEq>) );
        } else if constexpr(is_same<dt2, Type_Distance>::value) {
            sort(execution::par_unseq, dataSort.begin(), dataSort.end(),
                 orderAsc.at(0) ?
                 (orderAsc.at(1) ? sortTwo<T,float,U,float,GetFloat16,GetDistance,CompareNumbersAsc,CompareNumbersAsc,CompareNumbersEq>
                                    : sortTwo<T,float,U,float,GetFloat16,GetDistance,CompareNumbersAsc,CompareNumbersDesc,CompareNumbersEq>) :
                 (orderAsc.at(1) ? sortTwo<T,float,U,float,GetFloat16,GetDistance,CompareNumbersDesc,CompareNumbersAsc,CompareNumbersEq>
                                    : sortTwo<T,float,U,float,GetFloat16,GetDistance,CompareNumbersDesc,CompareNumbersDesc,CompareNumbersEq>) );
        } else {
            sort(execution::par_unseq, dataSort.begin(), dataSort.end(),
                 orderAsc.at(0) ?
                 (orderAsc.at(1) ? sortTwo<T,float,U,U,GetFloat16,GetSame,CompareNumbersAsc,CompareNumbersAsc,CompareNumbersEq>
                                    : sortTwo<T,float,U,U,GetFloat16,GetSame,CompareNumbersAsc,CompareNumbersDesc,CompareNumbersEq>) :
                 (orderAsc.at(1) ? sortTwo<T,float,U,U,GetFloat16,GetSame,CompareNumbersDesc,CompareNumbersAsc,CompareNumbersEq>
                                    : sortTwo<T,float,U,U,GetFloat16,GetSame,CompareNumbersDesc,CompareNumbersDesc,CompareNumbersEq>) );
        }
    } else if constexpr(is_same<dt1, Type_Enum>::value) {
        if constexpr(is_same<dt2,Type_Float16>::value) {
            sort(execution::par_unseq, dataSort.begin(), dataSort.end(),
                 orderAsc.at(0) ?
                 (orderAsc.at(1) ? sortTwo<T,T,U,float,GetSame,GetFloat16,CompareEnum0Asc,CompareNumbersAsc,CompareEnum0Eq>
                                    : sortTwo<T,T,U,float,GetSame,GetFloat16,CompareEnum0Asc,CompareNumbersDesc,CompareEnum0Eq>) :
                 (orderAsc.at(1) ? sortTwo<T,T,U,float,GetSame,GetFloat16,CompareEnum0Desc,CompareNumbersAsc,CompareEnum0Eq>
                                    : sortTwo<T,T,U,float,GetSame,GetFloat16,CompareEnum0Desc,CompareNumbersDesc,CompareEnum0Eq>) );
        } else if constexpr(is_same<dt2, Type_Enum>::value) {
            sort(execution::par_unseq, dataSort.begin(), dataSort.end(),
                 orderAsc.at(0) ?
                 (orderAsc.at(1) ? sortTwo<T,T,U,U,GetSame,GetSame,CompareEnum0Asc,CompareEnum1Asc,CompareEnum0Eq>
                                    : sortTwo<T,T,U,U,GetSame,GetSame,CompareEnum0Asc,CompareEnum1Desc,CompareEnum0Eq>) :
                 (orderAsc.at(1) ? sortTwo<T,T,U,U,GetSame,GetSame,CompareEnum0Desc,CompareEnum1Asc,CompareEnum0Eq>
                                    : sortTwo<T,T,U,U,GetSame,GetSame,CompareEnum0Desc,CompareEnum1Desc,CompareEnum0Eq>) );
        } else if constexpr(is_same<dt2, Type_Distance>::value) {
            sort(execution::par_unseq, dataSort.begin(), dataSort.end(),
                 orderAsc.at(0) ?
                 (orderAsc.at(1) ? sortTwo<T,T,U,float,GetSame,GetDistance,CompareEnum0Asc,CompareNumbersAsc,CompareEnum0Eq>
                                    : sortTwo<T,T,U,float,GetSame,GetDistance,CompareEnum0Asc,CompareNumbersDesc,CompareEnum0Eq>) :
                 (orderAsc.at(1) ? sortTwo<T,T,U,float,GetSame,GetDistance,CompareEnum0Desc,CompareNumbersAsc,CompareEnum0Eq>
                                    : sortTwo<T,T,U,float,GetSame,GetDistance,CompareEnum0Desc,CompareNumbersDesc,CompareEnum0Eq>) );
        } else {
            sort(execution::par_unseq, dataSort.begin(), dataSort.end(),
                 orderAsc.at(0) ?
                 (orderAsc.at(1) ? sortTwo<T,T,U,U,GetSame,GetSame,CompareEnum0Asc,CompareNumbersAsc,CompareNumbersEq>
                                    : sortTwo<T,T,U,U,GetSame,GetSame,CompareEnum0Asc,CompareNumbersAsc,CompareEnum0Eq>) :
                 (orderAsc.at(1) ? sortTwo<T,T,U,U,GetSame,GetSame,CompareEnum0Desc,CompareNumbersAsc,CompareNumbersEq>
                                    : sortTwo<T,T,U,U,GetSame,GetSame,CompareEnum0Desc,CompareNumbersDesc,CompareEnum0Eq>) );
        }
    } else if constexpr(is_same<dt1, Type_Distance>::value) {
        if constexpr(is_same<dt2, Type_Float16>::value) {
            sort(execution::par_unseq, dataSort.begin(), dataSort.end(),
                 orderAsc.at(0) ?
                 (orderAsc.at(1) ? sortTwo<T,float,U,float,GetDistance, GetFloat16, CompareNumbersAsc, CompareNumbersAsc, CompareNumbersEq>
                                    : sortTwo<T,float,U,float,GetDistance, GetFloat16, CompareNumbersAsc, CompareNumbersDesc, CompareNumbersEq>) :
                 (orderAsc.at(1) ? sortTwo<T,float,U,float,GetDistance, GetFloat16, CompareNumbersDesc, CompareNumbersAsc, CompareNumbersEq>
                                    : sortTwo<T,float,U,float,GetDistance, GetFloat16, CompareNumbersDesc, CompareNumbersDesc, CompareEnum0Eq>));
        } else if constexpr(is_same<dt2, Type_Enum>::value) {
            sort(execution::par_unseq, dataSort.begin(), dataSort.end(),
                 orderAsc.at(0) ?
                 (orderAsc.at(1) ? sortTwo<T,float,U,U,GetDistance, GetSame, CompareNumbersAsc, CompareEnum1Asc, CompareNumbersEq>
                                    : sortTwo<T,float,U,U,GetDistance, GetSame, CompareNumbersAsc, CompareEnum1Desc, CompareNumbersEq>) :
                 (orderAsc.at(1) ? sortTwo<T,float,U,U,GetDistance, GetSame, CompareNumbersDesc, CompareEnum1Asc, CompareNumbersEq>
                                    : sortTwo<T,float,U,U,GetDistance, GetSame, CompareNumbersDesc, CompareEnum1Desc, CompareNumbersEq>));
        } else if constexpr(is_same<dt2, Type_Distance>::value) {
            sort(execution::par_unseq, dataSort.begin(), dataSort.end(),
                 orderAsc.at(0) ?
                 (orderAsc.at(1) ? sortTwo<T,float,U,float,GetDistance, GetDistance, CompareNumbersAsc, CompareNumbersAsc, CompareNumbersEq>
                                    : sortTwo<T,float,U,float,GetDistance, GetDistance, CompareNumbersAsc, CompareNumbersDesc, CompareNumbersEq>) :
                 (orderAsc.at(1) ? sortTwo<T,float,U,float,GetDistance, GetDistance, CompareNumbersDesc, CompareNumbersAsc, CompareNumbersEq>
                                    : sortTwo<T,float,U,float,GetDistance, GetDistance, CompareNumbersDesc, CompareNumbersDesc, CompareNumbersEq>));
        } else {
            sort(execution::par_unseq, dataSort.begin(), dataSort.end(),
                 orderAsc.at(0) ?
                 (orderAsc.at(1) ? sortTwo<T,float,U,U,GetDistance, GetSame, CompareNumbersAsc, CompareNumbersAsc, CompareNumbersEq>
                                    : sortTwo<T,float,U,U,GetDistance, GetSame, CompareNumbersAsc, CompareNumbersDesc, CompareNumbersEq>) :
                 (orderAsc.at(1) ? sortTwo<T,float,U,U,GetDistance, GetSame, CompareNumbersDesc, CompareNumbersAsc, CompareNumbersEq>
                                    : sortTwo<T,float,U,U,GetDistance, GetSame, CompareNumbersDesc, CompareNumbersDesc, CompareNumbersEq>));
        }
    } else {
        if constexpr(is_same<dt2, Type_Float16>::value) {
            sort(execution::par_unseq, dataSort.begin(), dataSort.end(),
                 orderAsc.at(0) ?
                 (orderAsc.at(1) ? sortTwo<T,T,U,float,GetSame, GetFloat16, CompareNumbersAsc, CompareNumbersAsc, CompareNumbersEq>
                                    : sortTwo<T,T,U,float,GetSame, GetFloat16, CompareNumbersAsc, CompareNumbersDesc, CompareNumbersEq>) :
                 (orderAsc.at(1) ? sortTwo<T,T,U,float,GetSame, GetFloat16, CompareNumbersDesc, CompareNumbersAsc, CompareNumbersEq>
                                    : sortTwo<T,T,U,float,GetSame, GetFloat16, CompareNumbersDesc, CompareNumbersDesc, CompareNumbersEq>));
        } else if constexpr(is_same<dt2, Type_Enum>::value) {
            sort(execution::par_unseq, dataSort.begin(), dataSort.end(),
                 orderAsc.at(0) ?
                 (orderAsc.at(1) ? sortTwo<T,T,U,U,GetSame, GetSame, CompareNumbersAsc, CompareEnum1Asc, CompareNumbersEq>
                                    : sortTwo<T,T,U,U,GetSame, GetSame, CompareNumbersAsc, CompareEnum1Desc, CompareNumbersEq>) :
                 (orderAsc.at(1) ? sortTwo<T,T,U,U,GetSame, GetSame, CompareNumbersDesc, CompareEnum1Asc, CompareNumbersEq>
                                    : sortTwo<T,T,U,U,GetSame, GetSame, CompareNumbersDesc, CompareEnum1Desc, CompareNumbersEq>));
        } else if constexpr(is_same<dt2, Type_Distance>::value) {
            sort(execution::par_unseq, dataSort.begin(), dataSort.end(),
                 orderAsc.at(0) ?
                 (orderAsc.at(1) ? sortTwo<T,T,U,float,GetSame, GetDistance, CompareNumbersAsc, CompareNumbersAsc, CompareNumbersEq>
                                    : sortTwo<T,T,U,float,GetSame, GetDistance, CompareNumbersAsc, CompareNumbersDesc, CompareNumbersEq>) :
                 (orderAsc.at(1) ? sortTwo<T,T,U,float,GetSame, GetDistance, CompareNumbersDesc, CompareNumbersAsc, CompareNumbersEq>
                                    : sortTwo<T,T,U,float,GetSame, GetDistance, CompareNumbersDesc, CompareNumbersDesc, CompareNumbersEq>));
        } else {
            sort(execution::par_unseq, dataSort.begin(), dataSort.end(),
                 orderAsc.at(0) ?
                 (orderAsc.at(1) ? sortTwo<T,T,U,U,GetSame, GetSame, CompareNumbersAsc, CompareNumbersAsc, CompareNumbersEq>
                                    : sortTwo<T,T,U,U,GetSame, GetSame, CompareNumbersAsc, CompareNumbersDesc, CompareNumbersEq>) :
                 (orderAsc.at(1) ? sortTwo<T,T,U,U,GetSame, GetSame, CompareNumbersDesc, CompareNumbersAsc, CompareNumbersEq>
                                    : sortTwo<T,T,U,U,GetSame, GetSame, CompareNumbersDesc, CompareNumbersDesc, CompareNumbersEq>));
        }
    }
    pack(o,blocSize);
}

void OrderModel::pack(uint32_t size,int blocSize) {
    // Pack
    auto p0 = (char*)sortDataRaw;
    auto p1 = (uint32_t*)sortDataRaw;
    p0 += blocSize;
    for (int i=1; i< size; i++) {
        p1[i] = *(uint32_t*)p0;
        p0 += blocSize;
    }
}

}
