#pragma once

#include "comparator.hpp"

namespace queries::model {

class InModel : public ComparatorModel {
public:
    using ComparatorModel::ComparatorModel;
    void mergeChildrenCandidates() override {};
    uint64_t execShaderThreadOnCpu(uint32_t thread,uint32_t nbThreads) override;
    [[nodiscard]] std::string myName() const override {return "in";}
    void execute(queries::builders::AST *) override;
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceIn;};
};

}
