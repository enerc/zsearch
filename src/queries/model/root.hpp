#pragma once
#include "base.hpp"

namespace queries::model {

typedef struct {
    BaseModel   *t;
	uint32_t 	me;
	uint32_t    nbThreads;
} WorkerInitParam;

typedef enum {
    SearchFirst,
    SearchMiddle,
    SearchEnd,
    SearchFirstEnd,
    SearchExactMatch
} SearchMode;

class RootModel : public BaseModel {
public:
    using BaseModel::BaseModel;
    void mergeChildrenCandidates() override ;
    bool execShaderOnCpu() override;
    bool execShaderThreadOnCpu(uint32_t thread,uint32_t nbThreads) override {return false;} ;
    __uint128_t getResult(bool &isFloat) const override ;
    [[nodiscard]] virtual __uint128_t getFuncResult(const std::vector<__uint128_t>&,__uint128_t count) const;
    [[nodiscard]] std::string myName() const override {return name;}
    void setMyName(const std::string &s) override { name = s; }
    void execute(queries::builders::AST *) override {};
    virtual void executeQuery(queries::builders::AST *);
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return cst;};
    void setClassInstanceType(ClassInstanceType a) override { cst = a;}
    [[nodiscard]] uint64_t getNbResults() const override { return nbResults; }
    virtual void initFunc(mapping::index_types t) {};
    [[nodiscard]] virtual __uint128_t processGlobalFunc(const __uint128_t &a,const __uint128_t &prev) const { return 0;};
    [[nodiscard]] virtual __uint128_t getGlobalFuncStartVal() const { return 0;};
    void selectChunks() override;
protected:
    SearchMode searchMode;
    uint64_t nbResults;
    __int128_t toFilter;
    bool   isaFloat;
    bool   hasFunctionInside=false;
    std::string name="root";
    virtual void postFilter() {};
    virtual void parseSubExpression(queries::builders::AST *ast);

private:
    ClassInstanceType cst = ClassInstanceRoot;


    static void * InternalThreadEntryFunc(void * p) {
    	auto *o = (WorkerInitParam*)p;
    	o->t->execShaderThreadOnCpu(o->me,o->nbThreads);
    	return nullptr;
    }

};

}