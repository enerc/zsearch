#include "variableIndex.hpp"

#include <utility>
#include "storageModel.hpp"

using namespace std;

namespace indexes {


VariableIndex::VariableIndex(IndexChunkStructBase *_data,shared_ptr<dictionary::Dictionary> _dic,bool isArray) : BaseIndex(_data,mapping::storage_type_enum,isArray) {
    dic = std::move(_dic);
    if (data->length ==0) {
       data->length = 1;
       storageModel = StorageModelFactory::getModel(getYmmData(),getDeletedInfo(),1);
    }
}

void VariableIndex::dump() const {
    cout << endl << "DUMP :: Variable size index" << endl;
    cout << "Length: " << data->length << endl;
    cout << "nb_documents: " << data->nb_documents << endl;
    cout << "nb_row: " << data->row << endl;
    cout << "chunk: " << data->chunk << endl;
    //cout << hex << *(uint64_t*)&data->xmmData << dec << endl;
/*
    for (int i=0; i< data->row; i++) {
        __uint128_t v;
        bool isNull;
        bool endOfDocument;
        variableIndexReader->getNext(v,isNull,endOfDocument);
        cout << (uint64_t)v << " " << isNull << " " << endOfDocument << endl;
    }*/
    cout << endl;
}

void VariableIndex::add(const __uint128_t &v, bool isNull, bool endOfDocument) {
    uint32_t nbBits = 64 - __builtin_clzl((uint64_t)v|1);
    if (nbBits > data->length) {
        resize();
    }
    storageModel->save(data->row, v,isNull, endOfDocument);
    data->row ++;
    data->nb_documents += endOfDocument ? 1 : 0;
    updateDocIndex(endOfDocument);
}

void VariableIndex::resize() {
    auto tmpVal = (uint32_t *)malloc(data->row*sizeof(uint32_t));
    auto tmpIsNull = (bool *)malloc(data->row*sizeof(bool));
    auto tmpEndOfDocument = (bool *)malloc(data->row*sizeof(bool));
    auto tmpIsDeleted = (bool *)malloc(data->row*sizeof(bool));

    for (uint i = 0; i < data->row; i++) {
        __uint128_t a;
        storageModel->get(i, a, tmpIsNull[i], tmpEndOfDocument[i],tmpIsDeleted[i]);
        tmpVal[i] = a;
    }
    data->length++;
    length = data->length;
    storageModel = StorageModelFactory::getModel(getYmmData(),getDeletedInfo(), length);
    memset((char*)getYmmData(),0,StorageModelFactory::getModelSize(length)*(1+data->row/16));

    for (uint i = 0; i < data->row; i++) {
        if (!tmpIsDeleted[i]) {
            storageModel->save(i, tmpVal[i], tmpIsNull[i], tmpEndOfDocument[i]);
        }
    }

    free(tmpVal);
    free(tmpIsNull);
    free(tmpEndOfDocument);
    free(tmpIsDeleted);
}

string_view VariableIndex::getLastStringValue(bool &isNull, bool &endOfDocument,bool &isDeleted)  {
    __uint128_t v;
    if (data->row > 0) {
        storageModel->get(data->row-1,v,isNull,endOfDocument,isDeleted);
    }
    if (isNull) {
        return {};
    } else {
        return dic->getString((uint32_t) v);
    }
}

// used for sorting
void VariableIndex::getValue(uint32_t docId, __uint128_t &v, bool &isNull, bool &endOfDocument, bool &isDeleted)   const {
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

