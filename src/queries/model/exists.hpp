#pragma once

#include "root.hpp"

namespace queries::model {

class ExistsModel : public RootModel {
public:
    using RootModel::RootModel;
    bool execShaderOnCpu() override {return false;};
    [[nodiscard]] std::string myName() const override {return "exists";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceExists;};
};

}