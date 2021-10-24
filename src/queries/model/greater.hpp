#pragma once

#include "comparator.hpp"

namespace queries::model {

class GreaterModel : public ComparatorModel {
public:
    using ComparatorModel::ComparatorModel;
    void selectChunks() override;
    void execute(queries::builders::AST *) override;
    bool execShaderThreadOnCpu(uint32_t thread,uint32_t nbThreads) override;
    [[nodiscard]] std::string myName() const override {return ">";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceGreater;};
private:
};

}