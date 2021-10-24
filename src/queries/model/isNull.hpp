#pragma once

#include "comparator.hpp"

namespace queries::model {

class IsNullModel : public ComparatorModel {
public:
    using ComparatorModel::ComparatorModel;
    void mergeChildrenCandidates() override;
    bool execShaderThreadOnCpu(uint32_t thread,uint32_t nbThreads) override;
    [[nodiscard]] std::string myName() const override {return "isNull";}
    void execute(queries::builders::AST *) override;
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceIsNull;};
private:
};

}