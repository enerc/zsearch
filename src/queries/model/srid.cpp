#include "srid.hpp"
#include "point.hpp"

using namespace std;

namespace queries::model {

__uint128_t SridModel::getResult(bool &isFloat) const{
    for (const auto &e : exprList) {
        switch (e->getClassInstanceType()) {
            case ClassInstancePoint: {
                auto d = static_pointer_cast<PointModel>(e);
                return d->getResult(isFloat);
            }
            default:
                break;
        }
    }
    return 0;
}

}
