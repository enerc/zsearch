#pragma once

#include "root.hpp"

namespace queries::model {

class NoneModel : public RootModel {
public:
    using RootModel::RootModel;
    void mergeChildrenCandidates() override ;
    void execute(queries::builders::AST *) override;
    uint64_t execShaderThreadOnCpu(uint32_t thread,uint32_t nbThreads) override;
    void parseSubExpression(queries::builders::AST *ast) override;
    [[nodiscard]] std::string myName() const override {return "none";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceNone;};
};

}
