#pragma once

#include "root.hpp"

namespace queries::model {

class IntersectModel : public RootModel {
public:
    using RootModel::RootModel;
    [[nodiscard]] std::string myName() const override {return "intersect";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceIntersect;};
};

}
