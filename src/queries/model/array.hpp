#pragma once

#include "root.hpp"

namespace queries::model {

class ArrayModel : public RootModel {
public:
    using RootModel::RootModel;
    [[nodiscard]] std::string myName() const override {return "array";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceArray;};
};

}
