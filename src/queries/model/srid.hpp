#pragma once

#include "root.hpp"
#include "../../common/int128.hpp"

namespace queries::model {

class SridModel : public RootModel {
public:
    using RootModel::RootModel;
    bool execShaderOnCpu() override {return false;};
    __uint128_t getResult(bool &isFloat) const override;
    [[nodiscard]] std::string myName() const override {return "Srid";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceSrid;};
};

}