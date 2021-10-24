#pragma once

#include "comparator.hpp"

namespace queries::model {

class EqualsModel : public ComparatorModel {
public:
    using ComparatorModel::ComparatorModel;
    void selectChunks() override;
    bool execShaderThreadOnCpu(uint32_t thread,uint32_t nbThreads) override;
    [[nodiscard]] std::string myName() const override {return "equals";}
    void execute(queries::builders::AST *) override;
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceEquals;};
private:
};

}