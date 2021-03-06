#pragma once

#include "root.hpp"

namespace queries::model {

class ExtractModel : public RootModel {
public:
    using RootModel::RootModel;
    [[nodiscard]] std::string myName() const override {return "extract";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceExtract;};
};

}
