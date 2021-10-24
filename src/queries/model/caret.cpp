#include "caret.hpp"

namespace queries::model {

__uint128_t CaretModel::getResult(bool &isFloat) const {
    __uint128_t ai = 0;
    __uint128_t bi = 0;
    isFloat = false;

    if (!exprList.empty()) {
        if (exprList.at(0)->getClassInstanceType() == ClassInstanceFloat) {
            ai = (double)from128d(exprList.at(0)->vals.at(0));
        }
        if (exprList.at(0)->getClassInstanceType() == ClassInstanceInteger) {
            ai = exprList.at(0)->vals.at(0);
        }
    }
    if (exprList.size() > 1) {
        if (exprList.at(1)->getClassInstanceType()== ClassInstanceFloat) {
            bi = (double)from128d(exprList.at(1)->vals.at(0));
        }
        if (exprList.at(1)->getClassInstanceType() == ClassInstanceInteger) {
            bi = exprList.at(1)->vals.at(0);
        }
    }
    ai ^= bi;
    return ai;
}

}