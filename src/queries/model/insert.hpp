#pragma once
#include <vector>
#include "root.hpp"

namespace queries::model {

class InsertModel : public RootModel {
public:
    using RootModel::RootModel;
    [[nodiscard]] std::string myName() const override {return "insert";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceInsert;};

    std::string tableName;
    std::vector<std::string> columns;
};

}
