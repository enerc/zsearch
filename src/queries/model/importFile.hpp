#pragma once

#include "root.hpp"
#include "../../repository/importFile.hpp"

namespace queries::model {

class ImportFileModel : public RootModel {
public:
    using RootModel::RootModel;
    uint64_t execShaderOnCpu() override ;
    void execute(queries::builders::AST *) override;
    [[maybe_unused]] void setQueryStatus(const std::shared_ptr<queries::QueryStatus> &queryStatus);
    [[nodiscard]] std::string myName() const override {return "importFile";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceImportFile;};

    std::string path;
    std::string indexName;
    repository::FileType    type;
    std::shared_ptr<queries::QueryStatus> queryStatus;

};

}
