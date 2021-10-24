#pragma once
#include "base.hpp"
#include "../../devices/cpu/kernels.hpp"
#include "../../devices/cpu/codegen.hpp"

namespace queries::model {

class Kernels {
public:
    template<MergeOper merge> bool execKernelNumbers(const std::vector<std::shared_ptr<indexes::IndexManager>> &indexManagers,uint64_t *workingSet,const std::vector<uint32_t>& jobs,uint32_t start, uint32_t JobLen,const devices::Codegen &code);
    template<OperatorType oper,MergeOper merge> bool execKernelVar(const std::shared_ptr<indexes::IndexManager> &indexManager, uint64_t *workingSet,const std::vector<uint32_t>& jobs, uint32_t start, uint32_t JobLen, const std::vector<std::string>  &toSearch);
    static bool execKernelNone(const std::shared_ptr<indexes::IndexManager> &indexManager, uint64_t *workingSet,const std::vector<uint32_t>& jobs, uint32_t start, uint32_t JobLen);
    static bool execKernelNull(const std::shared_ptr<indexes::IndexManager> &indexManager, uint64_t *workingSet,const std::vector<uint32_t>& jobs,uint32_t start, uint32_t JobLen);
    static bool execKernelNotNull(const std::shared_ptr<indexes::IndexManager> &indexManager, uint64_t *workingSet,uint32_t start, uint32_t JobLen);
private:
    template<MergeOper merge> bool execKernelNumbersNoArray(const std::vector<std::shared_ptr<indexes::IndexManager>> &indexManagers,uint64_t *workingSet,const std::vector<uint32_t>& jobs,uint32_t start, uint32_t JobLen,const devices::Codegen &code);
};

}
