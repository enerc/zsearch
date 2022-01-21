#pragma once

#include "root.hpp"

namespace queries::model {

class NotLikeModel : public RootModel {
public:
    using RootModel::RootModel;
    [[nodiscard]] std::string myName() const override {return "notLike";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceNotLike;};
};

}
