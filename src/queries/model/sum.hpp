#pragma once

#include "root.hpp"

namespace queries::model {

class SumModel : public RootModel {
public:
    using RootModel::RootModel;
    [[nodiscard]] std::string myName() const override {return "sum";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceSum;};
    void initFunc(mapping::index_types t) override;
    [[nodiscard]] __uint128_t processGlobalFunc(const __uint128_t &a,const __uint128_t &prev) const override;
    [[nodiscard]] __uint128_t getGlobalFuncStartVal() const override;
    mapping::index_types getResultType() override { return indexType; }
    [[nodiscard]] __uint128_t getFuncResult(const std::vector<__uint128_t> &a, __uint128_t count) const override;
private:
    mapping::index_types indexType;
};

}
