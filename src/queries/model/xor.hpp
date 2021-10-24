#pragma once

#include "root.hpp"

namespace queries::model {

class XorModel : public RootModel {
    public:
        using RootModel::RootModel;
        void execute(queries::builders::AST *) override;
        [[nodiscard]] std::string myName() const override {return "xor";}
        [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceXor;};
    };
}