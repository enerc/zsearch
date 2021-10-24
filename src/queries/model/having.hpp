#pragma once

#include "root.hpp"

namespace queries::model {

class HavingModel : public RootModel {
public:
    using RootModel::RootModel;
    bool execShaderOnCpu() override { return false;};
    [[nodiscard]] std::string myName() const override {return "having";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceHaving;};
};

}