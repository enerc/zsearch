#pragma once

#include "root.hpp"

namespace queries::model {

class DistanceSphereModel : public RootModel {
public:
    using RootModel::RootModel;
    [[nodiscard]] std::string myName() const override {return "st_distance_sphere";}
    [[nodiscard]] __uint128_t getCenter();
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceDistanceSphere;};
    void getFuncResult(const __uint128_t&src, __uint128_t &res)  override;
    mapping::index_types getResultType() override { return mapping::index_type_float; }
private:
    __uint128_t center;
    bool        centerSet = false;
};

}