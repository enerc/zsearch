#include "base.hpp"

using namespace std;
using namespace mapping;

namespace queries::model {

BaseModel::BaseModel(std::shared_ptr<BaseModel> &p) : parent(p),
        length(1),
        workingSet(nullptr),
        center(0),
        workingSetSize(0){
    vals.resize(2);
    values.resize(2);
};

BaseModel::~BaseModel() {
    freeWorkingSet();
}

bool BaseModel::allocWorkingSet(uint64_t size) {
    if (workingSet != nullptr) {
        return true;
    }
    workingSetSize = size;
    workingSet = (uint64_t *)aligned_alloc(32,size);    // 32 is for MMX
    if (workingSet == nullptr) {
        Log::error("SQL query. Memory allocation FAILED for a result temp buffer. Size was "+to_string(size));
        return false;
    }
    parZeroed4(workingSet,size);
    return true;
}

bool BaseModel::freeWorkingSet() {
    if (workingSet!= nullptr) {
        free(workingSet);
        workingSet = nullptr;
    }
    return true;
}

}
