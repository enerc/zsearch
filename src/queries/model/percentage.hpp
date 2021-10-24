#pragma once

#include "root.hpp"

namespace queries::model {

class PercentageModel : public RootModel {
public:
    using RootModel::RootModel;
    bool execShaderOnCpu() override {return false;};
    [[nodiscard]] std::string myName() const override {return "%";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstancePercentage;};
};

}