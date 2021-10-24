#pragma once

#include "root.hpp"

namespace queries::model {

class CaseListElementModel : public RootModel {
public:
    using RootModel::RootModel;
    [[nodiscard]] std::string myName() const override {return "caseList";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceCaseListElement;};
};

}