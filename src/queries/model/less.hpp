#pragma once

#include "comparator.hpp"

namespace queries::model {

class LessModel : public ComparatorModel {
public:
    using ComparatorModel::ComparatorModel;
    void selectChunks() override;
    bool execShaderThreadOnCpu(uint32_t thread,uint32_t nbThreads) override;
    void execute(queries::builders::AST *) override;
    [[nodiscard]] std::string myName() const override {return "<";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceLess;};
private:
};

}