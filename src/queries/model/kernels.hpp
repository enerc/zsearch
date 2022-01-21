#pragma once
#include "base.hpp"
#include "../../devices/cpu/kernels.hpp"
#include "../../devices/cpu/codegen.hpp"

namespace queries::model {

class Kernels {
public:
    template<MergeOper merge> uint64_t execKernelNumbers(const std::vector<std::shared_ptr<indexes::IndexManager>> &indexManagers,uint64_t *workingSet,const std::vector<uint32_t>& jobs,uint32_t start, uint32_t JobLen,const devices::Codegen &code);
    template<OperatorType oper,MergeOper merge> uint64_t execKernelVar(const std::shared_ptr<indexes::IndexManager> &indexManager, uint64_t *workingSet,const std::vector<uint32_t>& jobs, uint32_t start, uint32_t JobLen, const std::vector<std::string>  &toSearch);
    static uint64_t execKernelNone(const std::shared_ptr<indexes::IndexManager> &indexManager, uint64_t *workingSet,const std::vector<uint32_t>& jobs, uint32_t start, uint32_t JobLen);
    static uint64_t execKernelNull(const std::shared_ptr<indexes::IndexManager> &indexManager, uint64_t *workingSet,const std::vector<uint32_t>& jobs,uint32_t start, uint32_t JobLen);
    static uint64_t execKernelNotNull(const std::shared_ptr<indexes::IndexManager> &indexManager, uint64_t *workingSet,uint32_t start, uint32_t JobLen);
private:
    template<MergeOper merge> uint64_t execKernelNumbersNoArray(const std::vector<std::shared_ptr<indexes::IndexManager>> &indexManagers,uint64_t *workingSet,const std::vector<uint32_t>& jobs,uint32_t start, uint32_t JobLen,const devices::Codegen &code);
};

}
