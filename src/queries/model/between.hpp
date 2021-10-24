#pragma once

#include "comparator.hpp"

namespace queries::model {

class BetweenModel : public ComparatorModel {
public:
    using ComparatorModel::ComparatorModel;
    [[nodiscard]] std::string myName() const override {return "between";}
    bool execShaderThreadOnCpu(uint32_t thread,uint32_t nbThreads) override;
    void execute(queries::builders::AST *) override;
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceBetween;};
};

}