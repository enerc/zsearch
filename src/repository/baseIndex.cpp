#include "baseIndex.hpp"
#include "storageModel.hpp"

using namespace std;

namespace indexes {

BaseIndex::BaseIndex(IndexChunkStructBase *_data,mapping::storage_types _type, bool _isArray) : data(_data), type(_type), isArray(_isArray), readRow(0){
	data64 = (uint64_t*)getYmmData();
	length = data->length;
    storageModel = StorageModelFactory::getModel(data64,getDeletedInfo(),length);
}

void BaseIndex::setLength(uint32_t _length) {
	if (data->length == 0) {
		data->length = _length;
		length = _length;
	}
}

void BaseIndex::setArrayStart(uint32_t docId) {
    if (isArray) {
        auto *b = (IndexChunkStructArray*)data;
        readRow = b->blocOffsetH[docId>>4] << 16;
        readRow |= b->blocOffsetL[docId];
    } else {
        readRow = docId;
    }
}

void BaseIndex::reset() {
    auto blocSize = StorageModelFactory::getModelSize(data->length);
    memset(getYmmData(),0,((data->row+15)/16)*blocSize);
    data->row = 0;
    data->nb_documents = 0;
}

}
