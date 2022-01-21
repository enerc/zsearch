#pragma once

#include "comparator.hpp"

namespace queries::model {

class LikeModel : public ComparatorModel {
public:
    using ComparatorModel::ComparatorModel;
    uint64_t execShaderThreadOnCpu(uint32_t thread,uint32_t nbThreads) override;
    void execute(queries::builders::AST *) override;
    [[nodiscard]] std::string myName() const override {return "like";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceLike;};
private:
    bool prepareJob();
};

}
