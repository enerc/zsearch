#pragma once

#include "root.hpp"

namespace queries::model {

class OrModel : public RootModel {
public:
    using RootModel::RootModel;
    void execute(queries::builders::AST *) override;
    [[nodiscard]] std::string myName() const override {return "or";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceOr;};
};

}