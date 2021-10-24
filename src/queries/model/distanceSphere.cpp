#include "distanceSphere.hpp"
#include "../../queries/builders/ast.hpp"

using namespace indexes;
using namespace mapping;
using namespace std;
#include "kernels.hpp"
#include "srid.hpp"

namespace queries::model {

__uint128_t DistanceSphereModel::getCenter() {
    if (centerSet) {
        return center;
    }

    for (const auto &e : exprList) {
        switch (e->getClassInstanceType()) {
            case ClassInstanceSrid: {
                auto d = static_pointer_cast<SridModel>(e);
                bool isFloat;
                center = e->getResult(isFloat);
                centerSet = true;
                return center;
            }
            default:
                break;
        }
    }
    return 0;
}

void DistanceSphereModel::getFuncResult(const __uint128_t &a, __uint128_t &res)  {
    float x,y;
    Uint64ToLocation((int64_t)a,x,y);
    *(float*)&res = distance_to_center((int64_t)a,getCenter());
}

}