#pragma once

#include "root.hpp"

namespace queries::model {

class ConcatModel : public RootModel {
public:
    using RootModel::RootModel;
    bool execShaderOnCpu() override { return false;};
    [[nodiscard]] std::string myName() const override {return "concat";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceConcat;};
};

}