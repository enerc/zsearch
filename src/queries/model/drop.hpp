#pragma once

#include "root.hpp"

namespace queries::model {

class DropModel : public RootModel {
public:
    using RootModel::RootModel;
    void execute(queries::builders::AST *) override;
    [[nodiscard]] std::string myName() const override {return "drop";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceDrop;};

    std::string tableName;
    bool        ifExists;
};

}
