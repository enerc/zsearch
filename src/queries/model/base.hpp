#pragma once
#include <memory>
#include <utility>
#include <vector>
#include "../../repository/indexManager.hpp"

namespace queries::builders {
    class AST;
}

namespace queries::model {

class BaseModel;

typedef struct {
    uint32_t v[16];
} VarParameters;

typedef struct {
    std::string field;
    std::string alias;
    std::shared_ptr<BaseModel> function;
    __uint128_t globalFuncResult;
} FieldAndFunction;

enum ClassInstanceType {
    ClassInstanceInteger,
    ClassInstanceFloat,
    ClassInstanceString,
    ClassInstanceAnd,
    ClassInstanceBetween,
    ClassInstanceColumnRef,
    ClassInstanceExists,
    ClassInstanceHaving,
    ClassInstanceInsert,
    ClassInstanceLessEq,
    ClassInstanceNone,
    ClassInstanceOffset,
    ClassInstancePlus,
    ClassInstanceSlash,
    ClassInstanceValues,
    ClassInstanceArray,
    ClassInstanceCaret,
    ClassInstanceConcat,
    ClassInstanceExtract,
    ClassInstanceILike,
    ClassInstanceIntersect,
    ClassInstanceLike,
    ClassInstanceNot,
    ClassInstanceOr,
    ClassInstanceXor,
    ClassInstancePoint,
    ClassInstanceSrid,
    ClassInstanceDistanceSphere,
    ClassInstanceStar,
    ClassInstanceWhere,
    ClassInstanceAsterisk,
    ClassInstanceCase,
    ClassInstanceEquals,
    ClassInstanceGreater,
    ClassInstanceImportFile,
    ClassInstanceIsNull,
    ClassInstanceLimit,
    ClassInstanceNotEquals,
    ClassInstanceOrder,
    ClassInstanceRoot,
    ClassInstanceUnaryMinus,
    ClassInstanceBase,
    ClassInstanceCaseListElement,
    ClassInstanceExcept,
    ClassInstanceGreaterEq,
    ClassInstanceIn,
    ClassInstanceLess,
    ClassInstanceMinus,
    ClassInstanceNotLike,
    ClassInstancePercentage,
    ClassInstanceSelect,
    ClassInstanceUnion,
    ClassInstanceCount,
    ClassInstanceMin,
    ClassInstanceMax,
    ClassInstanceSum,
    ClassInstanceAvg,
    ClassInstanceDrop,
    ClassInstanceDelete
};

class BaseModel {
public:
    explicit BaseModel(std::shared_ptr<BaseModel> &p);
    ~BaseModel();

    virtual void mergeChildrenCandidates() = 0;
    virtual bool execShaderOnCpu() =  0;
    virtual bool execShaderThreadOnCpu(uint32_t thread,uint32_t nbThreads) = 0;
    virtual __uint128_t getResult(bool &isFloat) const = 0;
    virtual mapping::index_types getResultType() { return mapping::index_type_none; }
    virtual int adjustScaling(int scaling) { return scaling; }
    virtual void getFuncResult(const __uint128_t&src, __uint128_t &res)  {};
    //void setIndexManager(const std::shared_ptr<indexes::IndexManager> &u) { indexManager = u; };
    //void setType(mapping::storage_types u) { type = u; }
    //[[maybe_unused]] void setValues(const std::vector<std::string>&v) { values = v;  };
    //[[maybe_unused]] void setValue(const __uint128_t&v) { val = v; };
    [[maybe_unused]] void setLength(uint32_t l) { length = l; };
    [[nodiscard]] virtual std::string myName() const = 0;
    virtual void setMyName(const std::string &s) = 0;
    virtual void execute(queries::builders::AST *) = 0;
    [[nodiscard]] virtual ClassInstanceType getClassInstanceType() const = 0;
    [[nodiscard]] virtual const uint64_t* getWorkingSet() const { return workingSet; };
    [[nodiscard]] virtual uint64_t getWorkingSetSize() const { return workingSetSize; };
    [[nodiscard]] virtual uint64_t getNbResults() const { return 0; }
    virtual void setClassInstanceType(ClassInstanceType a) { }
    bool        allocWorkingSet(uint64_t size);
    bool        freeWorkingSet();
    [[nodiscard]] const std::shared_ptr<indexes::IndexManager> &getIndexManager(int i) const { return indexManagers.at(i);}
    [[nodiscard]] const std::vector<std::shared_ptr<BaseModel>> &getExprList() const { return exprList; }
    std::vector<__uint128_t>    vals;
    std::vector<std::vector<std::string>> values;
    uint32_t                    length;
    std::vector<bool>           orderAsc;
    __uint64_t                  center;

protected:
    std::shared_ptr<BaseModel>	parent;
    std::vector<uint32_t>       jobs;
    std::vector<std::shared_ptr<indexes::IndexManager>> indexManagers;
    std::vector<std::string>    fields;
    uint64_t                    *workingSet;
    uint64_t                    workingSetSize;
    std::vector<std::string>    fieldsToOrder;
    std::vector<std::shared_ptr<BaseModel>> exprList;

    [[maybe_unused]] [[nodiscard]] uint64_t getChunckCanditateCount() const { return jobs.size(); }
    friend class queries::builders::AST;
protected:

private:
    virtual void selectChunks() = 0;
};


}