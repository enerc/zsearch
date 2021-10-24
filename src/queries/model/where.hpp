#pragma once

#include "root.hpp"

namespace queries::model {

class WhereModel : public RootModel {
public:
    using RootModel::RootModel;
    [[nodiscard]] std::string myName() const override {return "where";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceWhere;};
    [[nodiscard]] const uint64_t* getWorkingSet() const override;
    [[nodiscard]] uint64_t getWorkingSetSize() const override;
    [[nodiscard]] uint64_t getNbResults() const override;
    void setNoWhere(bool _noWhere) {WhereModel::noWhere = _noWhere;  }
private:
    bool        noWhere;
};

}