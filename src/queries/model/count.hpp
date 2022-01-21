#pragma once

#include "root.hpp"

namespace queries::model {

    class CountModel : public RootModel {
    public:
        using RootModel::RootModel;
        void getFuncResult(const __uint128_t&src, __uint128_t &res)  override;
        [[nodiscard]] std::string myName() const override {return "count";}
        [[nodiscard]] ClassInstanceType getClassInstanceType()  const override {return ClassInstanceCount;};
        void initCount(const std::shared_ptr<RootModel>& where);
        mapping::index_types getResultType() override { return mapping::index_type_unsigned; }
        int adjustScaling(int scaling) override { return 0; }
        [[nodiscard]] uint64_t getCount() const { return count;  }
    private:
        uint64_t count;
    };

}
