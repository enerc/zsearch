#pragma once
#include <vector>
#include "root.hpp"

namespace queries::model {

class ValuesModel : public RootModel {
public:
    using RootModel::RootModel;
    [[nodiscard]] std::string myName() const override {return "values";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceValues;};

    std::vector<std::shared_ptr<BaseModel>> items;
};

}
