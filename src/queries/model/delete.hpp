#pragma once

#include "root.hpp"

namespace queries::model {

class DeleteModel : public RootModel {
public:
    using RootModel::RootModel;
    void execute(queries::builders::AST *) override;
    [[nodiscard]] std::string myName() const override {return "count";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceDelete;};
    mapping::index_types getResultType() override { return mapping::index_type_unsigned; }

    std::string schema;
    std::string tableName;
};

}