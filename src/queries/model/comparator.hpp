#pragma once
#include <map>

#include "root.hpp"
#include "../../devices/cpu/codegen.hpp"

namespace queries::model {

class ComparatorModel : public RootModel {
public:
    using RootModel::RootModel;
protected:
    void parseSubExpression(queries::builders::AST *ast) override;
    devices::Codegen codegen;
private:
    bool   isSigned;
    double maxScaling;
    bool   isInClause;
    bool   isDate;
    mapping::mapping_entry meDate;
    int    len;
    int    bank;
    int    sourceIndex;
    std::map<BaseModel *,int> associatedBank;
    std::map<BaseModel *,bool> doScale;
    std::map<BaseModel *,double> extraScale;
    std::shared_ptr<QueryStatus> queryStatus;


    void parseForType(const std::vector<std::shared_ptr<BaseModel>> &el,queries::builders::AST *ast);
    void gen(const std::vector<std::shared_ptr<BaseModel>> &el,queries::builders::AST *ast);
    void genComparator();
};

}