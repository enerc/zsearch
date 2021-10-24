#pragma once

#include "root.hpp"

namespace queries::model {

class AndModel : public RootModel {
public:
    using RootModel::RootModel;
    void execute(queries::builders::AST *) override;
    [[nodiscard]] std::string myName() const override {return "and";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceAnd;};
};

}