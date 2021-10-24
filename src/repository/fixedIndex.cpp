#include "baseIndex.hpp"
#include "fixedIndex.hpp"
#include "storageModel.hpp"

#include <chrono>

using namespace std;
using namespace mapping;

namespace indexes {


FixedIndex::FixedIndex(IndexChunkStructBase *_data, storage_types _type,uint32_t _length,bool isArray) : BaseIndex(_data,_type,isArray){
	if (data->length == 0) {
		data->length = _length;
        length = _length;
        storageModel = StorageModelFactory::getModel(getYmmData(),getDeletedInfo(),_length);
	}
 }

void FixedIndex::dump() const {
	cout << endl << "DUMP :: Fixed size index" << endl;
	cout << "Length: " << data->length << endl;
	cout << "nb_documents: " << data->nb_documents << endl;
    cout << "nb_rows: " << data->row << endl;
	cout << "chunk: " << data->chunk << endl;
	cout << " * " << hex << data64[0] << " " << data64[1] << " " << data64[2] << " " << (&data64 + 65536/8) << " " << dec << endl;
	//printf("%lp %lp \n",data64,data64+65536/8);
#if 0
	while (e) {
		e = decodeNext(documentId,v,endOfDocument,isNull);
		if (e) {
			cout << "Id:" << dec << documentId << " value:";
			printValue(v);
			cout << " end:" << (endOfDocument ? "true" : "false") << endl;
		}
	}
	cout << endl;
#endif
}

void FixedIndex::add(const __uint128_t &v, bool isNull, bool endOfDocument)  {
    storageModel->save(data->row,v,isNull,endOfDocument);
    data->row ++;
    data->nb_documents += endOfDocument ? 1 : 0;
    updateDocIndex(endOfDocument);
}

void FixedIndex::getLastValue(__uint128_t &v, bool &isNull, bool &endOfDocument, bool &isDeleted) {
    if (data->row > 0) {
        storageModel->get(data->row-1,v,isNull,endOfDocument,isDeleted);
    }
}

void FixedIndex::getValue(uint32_t docId, __uint128_t &v, bool &isNull, bool &endOfDocument, bool &isDeleted) const {
    uint32_t r;
    if (isArray) {
        auto *b = (IndexChunkStructArray*)data;
        r = b->blocOffsetH[docId>>4] << 16;
        r |= b->blocOffsetL[docId];
    } else {
        r = docId;
    }
    storageModel->get(r,v,isNull,endOfDocument,isDeleted);
}

}
