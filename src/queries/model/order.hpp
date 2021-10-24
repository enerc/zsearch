#pragma once

#include "root.hpp"

namespace queries::model {

class WhereModel;

template <typename T>
class PreAllocator {
private:
    T* memory_ptr;
    std::size_t memory_size;

public:
    typedef std::size_t     size_type;
    typedef T*              pointer;
    typedef T               value_type;

    PreAllocator(T* memory_ptr, std::size_t memory_size) : memory_ptr(memory_ptr), memory_size(memory_size) {}

    PreAllocator(const PreAllocator& other) throw() : memory_ptr(other.memory_ptr), memory_size(other.memory_size) {};

    template<typename U>
    PreAllocator(const PreAllocator<U>& other) throw() : memory_ptr(other.memory_ptr), memory_size(other.memory_size) {};

    template<typename U>
    PreAllocator& operator = (const PreAllocator<U>& other) { return *this; }
    PreAllocator<T>& operator = (const PreAllocator& other) { return *this; }
    ~PreAllocator() {}


    pointer allocate(size_type n, const void* hint = 0) {return memory_ptr;}
    void deallocate(T* ptr, size_type n) {}

    [[nodiscard]] size_type max_size() const {return memory_size;}
};

class OrderModel : public RootModel {
public:
    virtual ~OrderModel() { free(sortDataRaw); }
    using RootModel::RootModel;
    bool execShaderOnCpu() override {return false;};
    [[nodiscard]] std::string myName() const override {return "order";}
    [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceOrder;};
    void execute(queries::builders::AST *) override;
    void setFieldsToOrder(const std::vector<std::string>   &_fieldsToOrder) { fieldsToOrder = _fieldsToOrder;}
    void setOrder(std::vector<bool> _orderAsc) { orderAsc = _orderAsc; }
    void setColumnMapping(const std::shared_ptr<mapping::IndexDefinition> &_columnMapping) {
        columnMapping = _columnMapping;
    }
    void setWhere(const std::shared_ptr<queries::model::WhereModel> &_where) {
        OrderModel::where = _where;
    }
    void setFromTables(const std::vector<std::string> &_fromTables) {
        fromTables = _fromTables;
    }
    [[nodiscard]] const void * getSortDataRaw() const { return sortDataRaw ; }
private:
    //std::vector<std::string>   fieldsToOrder;
    std::vector<std::string>   fromTables;
    std::shared_ptr<mapping::IndexDefinition> columnMapping;
    std::shared_ptr<queries::model::WhereModel> where;
    queries::builders::AST *ast;
    void *sortDataRaw = nullptr;
    std::shared_ptr<mapping::IndexDefinition> columnMapped;

    class SortDataType { };
    class Type_Float16 : SortDataType {};
    class Type_Enum  : SortDataType {};
    class Type_Distance : SortDataType {};
    class Type_Numeric : SortDataType {};


    void orderOnOne();
    void orderOnTwo();
    template<typename T,typename dt> void orderOne();
    template<typename T,typename U, typename dt1,typename dt2> void orderTwo();
    bool prepareOrderTwo(int sizeObject);
    void pack(uint32_t size,int blocSize);
};

}