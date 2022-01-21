#pragma once

#include "root.hpp"
#include "../../common/int128.hpp"

namespace queries::model {

class PointModel : public RootModel {
public:
    using RootModel::RootModel;
    __uint128_t getResult(bool &isFloat) const override;
    [[nodiscard]] std::string myName() const override {return "point";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstancePoint;};
};

}
