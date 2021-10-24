#pragma once

#include "root.hpp"

namespace queries::model {

class UnionModel : public RootModel {
public:
    using RootModel::RootModel;
    bool execShaderOnCpu() override {return false;};
    [[nodiscard]] std::string myName() const override {return "union";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceUnion;};
};

}