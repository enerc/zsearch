#pragma once
#include "baseIndex.hpp"
#include "../common/int128.hpp"

namespace indexes {

// encoding:
// length is constant 1..128
// value (size length)
// last : 1 - 1 -> next is new documentId

class FixedIndex : public BaseIndex {
public:
	FixedIndex(IndexChunkStructBase *,mapping::storage_types,uint32_t length,bool isArray);
    void add(const __uint128_t &v, bool isNull, bool endOFDocument)  override;
    void getLastValue(__uint128_t &v, bool &isNull, bool &endOfDocument,bool &isDeleted)  override;
    void getValue(uint32_t loc,__uint128_t &v, bool &isNull, bool &endOfDocument,bool &isDeleted) const override;
    std::string_view getLastStringValue(bool &isNull, bool &endOfDocument,bool &isDeleted) override { return {} ; }
    [[nodiscard]] bool isFixed() const override { return true; };
	void dump() const override;
private:
};

}
