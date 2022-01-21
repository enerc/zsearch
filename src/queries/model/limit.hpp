#pragma once

#include "root.hpp"

namespace queries::model {

class LimitModel : public RootModel {
public:
    using RootModel::RootModel;
    [[nodiscard]] std::string myName() const override {return "limit";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceLimit;};
};

}
