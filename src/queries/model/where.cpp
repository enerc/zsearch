#include "where.hpp"

using namespace std;

namespace queries::model {

const uint64_t *WhereModel::getWorkingSet() const {
    return !exprList.empty() ? exprList.at(0)->getWorkingSet() : nullptr;
}

uint64_t WhereModel::getWorkingSetSize() const {
    return !exprList.empty() ? exprList.at(0)->getWorkingSetSize() : 0;
}

uint64_t WhereModel::getNbResults() const {
    return !exprList.empty() ? exprList.at(0)->getNbResults() : 0;
}

}