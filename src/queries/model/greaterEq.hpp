#pragma once

#include "comparator.hpp"

namespace queries::model {

class GreaterEqModel : public ComparatorModel {
public:
    using ComparatorModel::ComparatorModel;
    void selectChunks() override;
    void execute(queries::builders::AST *) override;
    bool execShaderThreadOnCpu(uint32_t thread,uint32_t nbThreads) override;
    [[nodiscard]] std::string myName()  const override {return ">=";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceGreaterEq;};
private:
};

}