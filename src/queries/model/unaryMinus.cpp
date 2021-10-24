#include "unaryMinus.hpp"

namespace queries::model {

__uint128_t UnaryMinusModel::getResult(bool &isFloat) const {
    double af = 0;
    __uint128_t ai = 0;
    isFloat = false;

    if (!exprList.empty()) {
        if (exprList.at(0)->getClassInstanceType() == ClassInstanceFloat) {
            af = from128d(exprList.at(0)->vals.at(0));
            isFloat = true;
        }
        if (exprList.at(0)->getClassInstanceType() == ClassInstanceInteger) {
            ai = exprList.at(0)->vals.at(0);
        }
    }
    if (isFloat) {
        return convertTo128d(-af);
    } else {
        return -ai;
    }
}

}