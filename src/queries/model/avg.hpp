#pragma once

#include "root.hpp"

namespace queries::model {

class AvgModel : public RootModel {
public:
    using RootModel::RootModel;
    [[nodiscard]] std::string myName() const override {return "avg";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceAvg;};
    void initFunc(mapping::index_types t) override;
    [[nodiscard]] __uint128_t processGlobalFunc(const __uint128_t &a,const __uint128_t &prev) const override;
    [[nodiscard]] __uint128_t getGlobalFuncStartVal() const override;
    [[nodiscard]] __uint128_t getFuncResult(const std::vector<__uint128_t>&,__uint128_t count) const override;
    mapping::index_types getResultType() override { return mapping::index_type_float; }
private:
    mapping::index_types indexType;
};

}
