#pragma once

#include "root.hpp"

namespace queries::model {

class NotModel : public RootModel {
public:
    using RootModel::RootModel;
    [[nodiscard]] std::string myName() const override {return "not";}
    void execute(queries::builders::AST *) override;
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceNot;};
};

}
