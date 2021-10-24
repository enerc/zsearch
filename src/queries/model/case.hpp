#pragma once

#include "root.hpp"

namespace queries::model {

class CaseModel : public RootModel {
public:
    using RootModel::RootModel;
    bool execShaderOnCpu() override {return false;};
    [[nodiscard]] std::string myName() const override {return "case";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceCase;};
};

}