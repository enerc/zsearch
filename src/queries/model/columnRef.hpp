#pragma once

#include "root.hpp"

namespace queries::model {

class ColumnRefModel : public RootModel {
public:
    using RootModel::RootModel;
    [[nodiscard]] std::string myName() const override {return "columnRef";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceColumnRef;};
};

}
