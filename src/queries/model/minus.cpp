#include "minus.hpp"

namespace queries::model {

__uint128_t MinusModel::getResult(bool &isFloat) const {
    double af = 0;
    double bf = 0;
    __uint128_t ai = 0;
    __uint128_t bi = 0;
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
    if (exprList.size() > 1) {
        if (exprList.at(1)->getClassInstanceType()== ClassInstanceFloat) {
            bf = from128d(exprList.at(1)->vals.at(0));
            if (!isFloat) {
                af = (double)ai;
            }
            isFloat = true;
        }
        if (exprList.at(1)->getClassInstanceType() == ClassInstanceInteger) {
            bi = exprList.at(1)->vals.at(0);
            if (isFloat) {
                bf = (double)bi;
            }
        }
    }
    if (isFloat) {
        af -= bf;
        return convertTo128d(af);
    } else {
        ai -= bi;
        return ai;
    }
}

}