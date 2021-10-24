#pragma once

#include "root.hpp"
#include "../../common/int128.hpp"

namespace queries::model {

class StarModel : public RootModel {
public:
    using RootModel::RootModel;
    bool execShaderOnCpu() override {return false;};
    [[nodiscard]] std::string myName() const override {return "*";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceStar;};
};

}