#include "count.hpp"

using namespace std;

namespace queries::model {

void CountModel::getFuncResult(const __uint128_t&src, __uint128_t &res) {
    res = count;
}

void CountModel::initCount(const std::shared_ptr<RootModel>& where) {
    count = where->getNbResults();
}

}