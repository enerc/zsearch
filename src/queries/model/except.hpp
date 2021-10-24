#pragma once

#include "root.hpp"

namespace queries::model {

class ExceptModel : public RootModel {
public:
    using RootModel::RootModel;
    bool execShaderOnCpu() override { return false;};
    [[nodiscard]] std::string myName() const override {return "except";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceExcept;};
};

}