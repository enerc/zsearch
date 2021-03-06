#pragma once

#include "root.hpp"

namespace queries::model {

class ILikeModel : public RootModel {
public:
    using RootModel::RootModel;
    [[nodiscard]] std::string myName() const override {return "ilike";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceILike;};
};

}
