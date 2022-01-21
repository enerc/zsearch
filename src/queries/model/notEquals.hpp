#pragma once

#include "comparator.hpp"

namespace queries::model {

class NotEqualsModel : public ComparatorModel {
public:
    using ComparatorModel::ComparatorModel;
    void mergeChildrenCandidates() override ;
    uint64_t execShaderThreadOnCpu(uint32_t thread,uint32_t nbThreads) override;
    void execute(queries::builders::AST *) override;
    [[nodiscard]] std::string myName() const override {return "!=";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceNotEquals;};

protected:
};

}
