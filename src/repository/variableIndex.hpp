#pragma once

#include "baseIndex.hpp"
#include "../dictionary/dictionary.hpp"
#include "indexManager.hpp"

namespace indexes {

// Encoding:
// length = 1
// indexIncreased : 1. 1 -> index size doubled (length = lenght << 1)
// value (size length)
// last : 1 - 1 -> next is new documentId

class VariableIndex : public BaseIndex {
public:
	VariableIndex(IndexChunkStructBase *,std::shared_ptr<dictionary::Dictionary>,bool isArray);
    void add(const __uint128_t &v, bool isNull, bool endOfDocument)  override;
    void getLastValue(__uint128_t &v, bool &isNull, bool &endOfDocument,bool &isDeleted)  override {  }
    void getValue(uint32_t loc, __uint128_t &v, bool &isNull, bool &endOfDocument,bool &isDeleted)  const override;
    std::string_view getLastStringValue(bool &isNull, bool &endOfDocument,bool &isDeleted) override;
	void dump() const override;
	[[nodiscard]] bool isFixed() const override { return false; };

private:
	std::shared_ptr<dictionary::Dictionary> dic;

    void   resize();
};

}
