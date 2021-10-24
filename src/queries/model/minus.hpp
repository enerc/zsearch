#pragma once

#include "root.hpp"

namespace queries::model {

class MinusModel : public RootModel {
public:
    using RootModel::RootModel;
    bool execShaderOnCpu() override {return false;};
    __uint128_t getResult(bool &isFloat) const override;
    [[nodiscard]] std::string myName() const override {return "-";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceMinus;};
};

}