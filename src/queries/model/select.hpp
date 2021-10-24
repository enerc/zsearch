#pragma once

#include "root.hpp"

namespace queries::model {

class WhereModel;
class SelectModel;
class OrderModel;

typedef struct {
    SelectModel *t;
    WhereModel  *where;
    OrderModel *order;
    uint32_t 	me;
    uint64_t    start;
    uint64_t    workingSize;
    uint64_t    toSkip;
    uint64_t    myLimit;
    queries::model::FieldAndFunction fieldToReturn;
} SelectWorkerInitParam;

typedef struct {
    std::vector<indexes::IndexManager*> indexManagerVector;
    std::vector<mapping::mapping_entry> mappingVector;
} CacheAccel;

class SelectModel : public RootModel {
public:
    using RootModel::RootModel;
    [[nodiscard]] std::string myName() const override {return "select";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceSelect;};
    void setFieldsToReturn(const std::vector<queries::model::FieldAndFunction>  &_fieldsToReturn) { fieldsToReturn = _fieldsToReturn; }
    void setIndexDefinition(std::shared_ptr<mapping::IndexDefinition> &e) { columnMapped = e; }
    std::string getResultAsFileThread(std::shared_ptr<WhereModel> &where,std::shared_ptr<QueryStatus> &queryStatus, int &nbFiles,uint64_t &resultSize, uint64_t offset, uint64_t limit);
    void setOrderModel(const std::shared_ptr<OrderModel> &orderModel) {  order = orderModel;   }
private:
    std::vector<queries::model::FieldAndFunction>  fieldsToReturn;
    std::shared_ptr<QueryStatus> queryStatus;
    std::shared_ptr<OrderModel> order;
    std::string tmpFileRootPath;
    static constexpr size_t fileExtendSize = 512L*1024L*1024L;
    CacheAccel ca;
    static constexpr int minLinesPerResultFile = 10000;
    bool hasGlobalFunc;
    bool onlyHasGlobalFunc;
    std::vector<__uint128_t> tmpResults;
    std::vector<__uint64_t> tmpResultsCount;
    std::atomic<std::uint64_t>resultSize;
    std::shared_ptr<mapping::IndexDefinition> columnMapped;
    uint64_t offset;
    uint64_t limit;

    void execGetResultAsFileThreadNoOrder(SelectWorkerInitParam *o);
    void execGetResultAsFileThreadOrdered(const SelectWorkerInitParam *o);
    void execGlobalFuncNoOrder(const WhereModel *where, uint32_t me,  uint64_t start, uint64_t workingSize, const FieldAndFunction &p);
    void execGlobalFuncNoOrdered(uint32_t me, uint64_t start, uint64_t workingSize, const FieldAndFunction &p);
    void  *openPrintFile(uint32_t me,int &tmpFileFd,size_t &tmpFileSize);
    [[nodiscard]] size_t resultToJson(uint64_t loc, char *buffer);
    void processGlobalFunctions(std::shared_ptr<WhereModel> &where);
    void execGetResultAsFileOnlyGlobalFunction();

    static void * InternalThreadEntryFuncNoOrder(void * p) {
        auto *o = (SelectWorkerInitParam*)p;
        o->t->execGetResultAsFileThreadNoOrder(o);
        return nullptr;
    }

    static void * InternalThreadEntryFuncOrdered(void * p) {
        auto *o = (SelectWorkerInitParam*)p;
        o->t->execGetResultAsFileThreadOrdered(o);
        return nullptr;
    }

    static void * InternalThreadEntryGlobalFuncNoOrder(void * p) {
        auto *o = (SelectWorkerInitParam*)p;
        o->t->execGlobalFuncNoOrder(o->where,o->me,o->start,o->workingSize,o->fieldToReturn);
        return nullptr;
    }

    static void * InternalThreadEntryGlobalFuncOrdered(void * p) {
        auto *o = (SelectWorkerInitParam*)p;
        o->t->execGlobalFuncNoOrdered(o->me,o->start,o->workingSize,o->fieldToReturn);
        return nullptr;
    }
};

}