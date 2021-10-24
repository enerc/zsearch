#include "point.hpp"

namespace queries::model {

__uint128_t PointModel::getResult(bool &isFloat) const  {
    isFloat = true;
    return locationToUint64(exprList.empty() ? 0 : (float)from128d(exprList.at(0)->getResult(isFloat)), exprList.size() > 1 ? (float)from128d(exprList.at(1)->getResult(isFloat)) : 0);
}

}