#pragma once

#include "root.hpp"

namespace queries::model {

class MaxModel : public RootModel {
public:
    using RootModel::RootModel;
    [[nodiscard]] std::string myName() const override {return "max";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceMax;};
    void initFunc(mapping::index_types t) override;
    [[nodiscard]] __uint128_t processGlobalFunc(const __uint128_t &a,const __uint128_t &prev) const override;
    [[nodiscard]] __uint128_t getGlobalFuncStartVal() const override;
    mapping::index_types getResultType() override { return indexType; }
private:
    mapping::index_types indexType;
};

}
