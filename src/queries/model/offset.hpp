#pragma once

#include "root.hpp"

namespace queries::model {

class OffsetModel : public RootModel {
public:
    using RootModel::RootModel;
    [[nodiscard]] std::string myName() const override {return "offset";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceOffset;};
};

}
