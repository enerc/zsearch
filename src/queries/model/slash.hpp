#pragma once

#include "root.hpp"
#include <cmath>

namespace queries::model {

class SlashModel : public RootModel {
public:
    using RootModel::RootModel;
    __uint128_t getResult(bool &isFloat) const override ;
    [[nodiscard]] std::string myName() const override {return "/";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceSlash;};
};

}
