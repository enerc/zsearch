#pragma once
#include <memory>
#include "baseIndex.hpp"
#include "../common/int128.hpp"
#include "../devices/cpu/kernels.hpp"

namespace indexes {

class StorageModel {
public:
    explicit StorageModel(void *_data, void* _isDeleted) { data = _data; isDeletedPtr = (uint64_t*)_isDeleted; d = nullptr;}
    virtual void get(uint32_t row,__uint128_t &v, bool &isNull, bool &endOfDocument,bool &isDeleted) const = 0;
    virtual void save(uint32_t row, const __uint128_t &v, bool isNull, bool endOfDocument) = 0;
    [[maybe_unused]] void setDataPtr(void *_data) { data = _data; }
    void setDeletedPtr(void *_data) { isDeletedPtr = (uint64_t*)_data; }
    void setAlive(uint32_t row) { isDeletedPtr[row>>6] |= 1L << (row&63);}
    [[maybe_unused]] void *getDataPtr() { return data; }
    void setIndexStructArray(void *p) { d = p;};
    [[nodiscard]] void *getIndexStructArray() const { return d; }
protected:
    void *data;
    void *d;
    uint64_t *isDeletedPtr;
    [[nodiscard]] inline bool getIsDeleted(uint32_t row) const { return 1-_bextr_u64(isDeletedPtr[row >> 6], row&63, 1);}
};


typedef struct {
    uint16_t isNull;
    uint16_t isEndOfDocument;
    uint16_t value;
} SM1;

#define getPtrR(row,SM) const auto e = ((SM *)((char*)data+((row)>>4)*sizeof(SM)))
#define getPtrW(row,SM) auto e = ((SM *)((char*)data+((row)>>4)*sizeof(SM)))

class StorageModel1 : public StorageModel {
public:
    explicit StorageModel1(void *_data, void* _isDeleted) : StorageModel(_data,_isDeleted) {};
    void get(uint32_t row,__uint128_t &v, bool &isNull, bool &endOfDocument, bool &isDeleted) const override {
        getPtrR(row,SM1);
        uint32_t i = row&15;
        isNull = _bextr_u32(e->isNull,i,1);
        endOfDocument = _bextr_u32(e->isEndOfDocument,i,1);
        v =  _bextr_u32(e->value,i,1);
        isDeleted = getIsDeleted(row);
    };
    void save(uint32_t row,const __uint128_t &v, bool isNull, bool endOfDocument) override {
        uint32_t i = row&15;
        getPtrW(row,SM1);
        e->isNull |= isNull << i;
        e->isEndOfDocument |= endOfDocument << i;
        e->value |= (uint32_t)v << i;
        setAlive(row);
   }
};

typedef struct {
    uint16_t isNull;
    uint16_t isEndOfDocument;
    uint32_t value;
} SM2;

class StorageModel2 : public StorageModel {
public:
    explicit StorageModel2(void *_data, void* _isDeleted) : StorageModel(_data,_isDeleted) {};
    void get(uint32_t row, __uint128_t &v, bool &isNull, bool &endOfDocument, bool &isDeleted) const override {
        uint32_t i = row&15;
        getPtrR(row,SM2);
        isNull = _bextr_u32(e->isNull,i,1);
        endOfDocument = _bextr_u32(e->isEndOfDocument,i,1);
        v =  _bextr_u32(e->value,i*2,2);
        isDeleted = getIsDeleted(row);
    }
    void save(uint32_t row, const __uint128_t &v, bool isNull, bool endOfDocument) override{
        uint32_t i = row&15;
        getPtrW(row,SM2);
        e->isNull |= isNull << i;
        e->isEndOfDocument |= endOfDocument << i;
        e->value |= (uint32_t)v << (i*2);
        setAlive(row);
    }
};

typedef struct {
    uint16_t isNull;
    uint16_t isEndOfDocument;
    uint8_t value4[8];
} SM3;

class StorageModel3 : public StorageModel {
public:
    explicit StorageModel3(void *_data, void* _isDeleted) : StorageModel(_data,_isDeleted) {};
    void get(uint32_t row, __uint128_t &v, bool &isNull, bool &endOfDocument, bool &isDeleted) const override {
        uint32_t i = row&15;
        getPtrR(row,SM3);
        isNull = _bextr_u32(e->isNull,i,1);
        endOfDocument = _bextr_u32(e->isEndOfDocument,i,1);
        v =  _bextr_u32(e->value4[i>>1],(i&1)*4,4);
        isDeleted = getIsDeleted(row);
    }
    void save(uint32_t row, const __uint128_t &v, bool isNull, bool endOfDocument) override{
        uint32_t i = row&15;
        getPtrW(row,SM3);
        e->isNull |= isNull << i;
        e->isEndOfDocument |= endOfDocument << i;
        e->value4[i>>1] |= v << ((i&1)*4);
        setAlive(row);
    }
};

typedef struct {
    uint16_t isNull;
    uint16_t isEndOfDocument;
    uint8_t value8[16];
} SM5;

class StorageModel5 : public StorageModel {
public:
    explicit StorageModel5(void *_data, void* _isDeleted) : StorageModel(_data,_isDeleted) {};
    void get(uint32_t row, __uint128_t &v, bool &isNull, bool &endOfDocument, bool &isDeleted) const override {
        uint32_t i = row&15;
        getPtrR(row,SM5);
        isNull = _bextr_u32(e->isNull,i,1);
        endOfDocument = _bextr_u32(e->isEndOfDocument,i,1);
        v =  e->value8[i];
        isDeleted = getIsDeleted(row);
    }
    void save(uint32_t row, const __uint128_t &v, bool isNull, bool endOfDocument) override{
        uint32_t i = row&15;
        getPtrW(row,SM5);
        e->isNull |= isNull << i;
        e->isEndOfDocument |= endOfDocument << i;
        e->value8[i] = v;
        setAlive(row);
    }
};

typedef struct {
    uint16_t a0 : 12 __attribute__((packed));
    uint16_t a1 : 12 __attribute__((packed));
    uint16_t a2 : 12 __attribute__((packed));
    uint16_t a3 : 12 __attribute__((packed));
    uint16_t a4 : 12 __attribute__((packed));
    uint16_t a5 : 12 __attribute__((packed));
    uint16_t a6 : 12 __attribute__((packed));
    uint16_t a7 : 12 __attribute__((packed));
    uint16_t a8 : 12 __attribute__((packed));
    uint16_t a9 : 12 __attribute__((packed));
    uint16_t a10 : 12 __attribute__((packed));
    uint16_t a11 : 12 __attribute__((packed));
    uint16_t a12 : 12 __attribute__((packed));
    uint16_t a13 : 12 __attribute__((packed));
    uint16_t a14 : 12 __attribute__((packed));
    uint16_t a15 : 12 __attribute__((packed));
} BitArray12;

typedef struct {
    uint16_t isNull;
    uint16_t isEndOfDocument;
    BitArray12 values;
} SM9;

class StorageModel9 : public StorageModel {
public:
    explicit StorageModel9(void *_data, void* _isDeleted) : StorageModel(_data,_isDeleted) {};
    void get(uint32_t row, __uint128_t &v, bool &isNull, bool &endOfDocument, bool &isDeleted) const override {
        uint32_t i = row&15;
        getPtrR(row,SM9);
        isNull = _bextr_u32(e->isNull,i,1);
        endOfDocument = _bextr_u32(e->isEndOfDocument,i,1);
        switch (i) {
            case 0:
                v = e->values.a0;
                break;
            case 1:
                v = e->values.a1;
                break;
            case 2:
                v = e->values.a2;
                break;
            case 3:
                v = e->values.a3;
                break;
            case 4:
                v = e->values.a4;
                break;
            case 5:
                v = e->values.a5;
                break;
            case 6:
                v = e->values.a6;
                break;
            case 7:
                v = e->values.a7;
                break;
            case 8:
                v = e->values.a8;
                break;
            case 9:
                v = e->values.a9;
                break;
            case 10:
                v = e->values.a10;
                break;
            case 11:
                v = e->values.a11;
                break;
            case 12:
                v = e->values.a12;
                break;
            case 13:
                v = e->values.a13;
                break;
            case 14:
                v = e->values.a14;
                break;
            case 15:
                v = e->values.a15;
                break;
            default:
                break;
        }
        isDeleted = getIsDeleted(row);
    }
    void save(uint32_t row, const __uint128_t &v, bool isNull, bool endOfDocument) override{
        uint32_t i = row&15;
        getPtrW(row,SM9);
        e->isNull |= isNull << i;
        e->isEndOfDocument |= endOfDocument << i;

        switch (i) {
            case 0:
                e->values.a0 = v;
                break;
            case 1:
                e->values.a1 = v;
                break;
            case 2:
                e->values.a2 = v;
                break;
            case 3:
                e->values.a3 = v;
                break;
            case 4:
                e->values.a4 = v;
                break;
            case 5:
                e->values.a5 = v;
                break;
            case 6:
                e->values.a6 = v;
                break;
            case 7:
                e->values.a7 = v;
                break;
            case 8:
                e->values.a8 = v;
                break;
            case 9:
                e->values.a9 = v;
                break;
            case 10:
                e->values.a10 = v;
                break;
            case 11:
                e->values.a11 = v;
                break;
            case 12:
                e->values.a12 = v;
                break;
            case 13:
                e->values.a13 = v;
                break;
            case 14:
                e->values.a14 = v;
                break;
            case 15:
                e->values.a15 = v;
                break;
            default:
                break;
        }
        setAlive(row);
    }
};

typedef struct {
    uint16_t isNull;
    uint16_t isEndOfDocument;
    uint16_t value16[16];
} SM13;

class StorageModel13 : public StorageModel {
public:
    explicit StorageModel13(void *_data, void* _isDeleted) : StorageModel(_data,_isDeleted) {};
    void get(uint32_t row, __uint128_t &v, bool &isNull, bool &endOfDocument, bool &isDeleted) const override {
        uint32_t i = row&15;
        getPtrR(row,SM13);
        isNull = _bextr_u32(e->isNull,i,1);
        endOfDocument = _bextr_u32(e->isEndOfDocument,i,1);
        v =  e->value16[i];
        isDeleted = getIsDeleted(row);
    }
    void save(uint32_t row, const __uint128_t &v, bool isNull, bool endOfDocument) override{
        uint32_t i = row&15;
        getPtrW(row,SM13);
        e->isNull |= isNull << i;
        e->isEndOfDocument |= endOfDocument << i;
        e->value16[i] = v;
        setAlive(row);
    }
};

typedef struct {
    uint32_t a0 : 24 __attribute__((packed));
    uint32_t a1 : 24 __attribute__((packed));
    uint32_t a2 : 24 __attribute__((packed));
    uint32_t a3 : 24 __attribute__((packed));
    uint32_t a4 : 24 __attribute__((packed));
    uint32_t a5 : 24 __attribute__((packed));
    uint32_t a6 : 24 __attribute__((packed));
    uint32_t a7 : 24 __attribute__((packed));
    uint32_t a8 : 24 __attribute__((packed));
    uint32_t a9 : 24 __attribute__((packed));
    uint32_t a10 : 24 __attribute__((packed));
    uint32_t a11 : 24 __attribute__((packed));
    uint32_t a12 : 24 __attribute__((packed));
    uint32_t a13 : 24 __attribute__((packed));
    uint32_t a14 : 24 __attribute__((packed));
    uint32_t a15 : 24 __attribute__((packed));
} BitArray24;

typedef struct {
    uint16_t isNull;
    uint16_t isEndOfDocument;
    BitArray24 values;
} SM17;

class StorageModel17 : public StorageModel {
public:
    explicit StorageModel17(void *_data, void* _isDeleted) : StorageModel(_data,_isDeleted) {};
    void get(uint32_t row, __uint128_t &v, bool &isNull, bool &endOfDocument, bool &isDeleted) const override {
        uint32_t i = row&15;
        getPtrR(row,SM17);
        isNull = _bextr_u32(e->isNull,i,1);
        endOfDocument = _bextr_u32(e->isEndOfDocument,i,1);
        switch (i) {
            case 0:
                v = e->values.a0;
                break;
            case 1:
                v = e->values.a1;
                break;
            case 2:
                v = e->values.a2;
                break;
            case 3:
                v = e->values.a3;
                break;
            case 4:
                v = e->values.a4;
                break;
            case 5:
                v = e->values.a5;
                break;
            case 6:
                v = e->values.a6;
                break;
            case 7:
                v = e->values.a7;
                break;
            case 8:
                v = e->values.a8;
                break;
            case 9:
                v = e->values.a9;
                break;
            case 10:
                v = e->values.a10;
                break;
            case 11:
                v = e->values.a11;
                break;
            case 12:
                v = e->values.a12;
                break;
            case 13:
                v = e->values.a13;
                break;
            case 14:
                v = e->values.a14;
                break;
            case 15:
                v = e->values.a15;
                break;
            default:
                break;
        }
        isDeleted = getIsDeleted(row);
    }
    void save(uint32_t row, const __uint128_t &v, bool isNull, bool endOfDocument) override{
        uint32_t i = row&15;
        getPtrW(row,SM17);
        e->isNull |= isNull << i;
        e->isEndOfDocument |= endOfDocument << i;
        switch (i) {
            case 0:
                e->values.a0 = v;
                break;
            case 1:
                e->values.a1 = v;
                break;
            case 2:
                e->values.a2 = v;
                break;
            case 3:
                e->values.a3 = v;
                break;
            case 4:
                e->values.a4 = v;
                break;
            case 5:
                e->values.a5 = v;
                break;
            case 6:
                e->values.a6 = v;
                break;
            case 7:
                e->values.a7 = v;
                break;
            case 8:
                e->values.a8 = v;
                break;
            case 9:
                e->values.a9 = v;
                break;
            case 10:
                e->values.a10 = v;
                break;
            case 11:
                e->values.a11 = v;
                break;
            case 12:
                e->values.a12 = v;
                break;
            case 13:
                e->values.a13 = v;
                break;
            case 14:
                e->values.a14 = v;
                break;
            case 15:
                e->values.a15 = v;
                break;
            default:
                break;
        }
        setAlive(row);
    }
};

typedef struct {
    uint16_t isNull;
    uint16_t isEndOfDocument;
    uint32_t value32[16];
} SM25;

class StorageModel25 : public StorageModel {
public:
    explicit StorageModel25(void *_data, void* _isDeleted) : StorageModel(_data,_isDeleted) {};
    void get(uint32_t row, __uint128_t &v, bool &isNull, bool &endOfDocument, bool &isDeleted) const override {
        uint32_t i = row&15;
        getPtrR(row,SM25);
        isNull = _bextr_u32(e->isNull,i,1);
        endOfDocument = _bextr_u32(e->isEndOfDocument,i,1);
        v =  e->value32[i];
        isDeleted = getIsDeleted(row);
    }
    void save(uint32_t row, const __uint128_t &v, bool isNull, bool endOfDocument) override{
        uint32_t i = row&15;
        getPtrW(row,SM25);
        e->isNull |= isNull << i;
        e->isEndOfDocument |= endOfDocument << i;
        e->value32[i] = v;
        setAlive(row);
    }
};

typedef struct {
    uint64_t a0 : 40 __attribute__((packed));
    uint64_t a1 : 40 __attribute__((packed));
    uint64_t a2 : 40 __attribute__((packed));
    uint64_t a3 : 40 __attribute__((packed));
    uint64_t a4 : 40 __attribute__((packed));
    uint64_t a5 : 40 __attribute__((packed));
    uint64_t a6 : 40 __attribute__((packed));
    uint64_t a7 : 40 __attribute__((packed));
    uint64_t a8 : 40 __attribute__((packed));
    uint64_t a9 : 40 __attribute__((packed));
    uint64_t a10 : 40 __attribute__((packed));
    uint64_t a11 : 40 __attribute__((packed));
    uint64_t a12 : 40 __attribute__((packed));
    uint64_t a13 : 40 __attribute__((packed));
    uint64_t a14 : 40 __attribute__((packed));
    uint64_t a15 : 40 __attribute__((packed));
} BitArray40;

typedef struct {
    uint16_t isNull;
    uint16_t isEndOfDocument;
    BitArray40 values;
} SM33;

class StorageModel33 : public StorageModel {
public:
    explicit StorageModel33(void *_data, void* _isDeleted) : StorageModel(_data,_isDeleted) {};
    void get(uint32_t row, __uint128_t &v, bool &isNull, bool &endOfDocument, bool &isDeleted) const override {
        uint32_t i = row&15;
        getPtrR(row,SM33);
        isNull = _bextr_u32(e->isNull,i,1);
        endOfDocument = _bextr_u32(e->isEndOfDocument,i,1);
        switch (i) {
            case 0:
                v = e->values.a0;
                break;
            case 1:
                v = e->values.a1;
                break;
            case 2:
                v = e->values.a2;
                break;
            case 3:
                v = e->values.a3;
                break;
            case 4:
                v = e->values.a4;
                break;
            case 5:
                v = e->values.a5;
                break;
            case 6:
                v = e->values.a6;
                break;
            case 7:
                v = e->values.a7;
                break;
            case 8:
                v = e->values.a8;
                break;
            case 9:
                v = e->values.a9;
                break;
            case 10:
                v = e->values.a10;
                break;
            case 11:
                v = e->values.a11;
                break;
            case 12:
                v = e->values.a12;
                break;
            case 13:
                v = e->values.a13;
                break;
            case 14:
                v = e->values.a14;
                break;
            case 15:
                v = e->values.a15;
                break;
            default:
                break;
        }
        isDeleted = getIsDeleted(row);
    }
    void save(uint32_t row, const __uint128_t &v, bool isNull, bool endOfDocument) override{
        uint32_t i = row&15;
        getPtrW(row,SM33);
        e->isNull |= isNull << i;
        e->isEndOfDocument |= endOfDocument << i;
        switch (i) {
            case 0:
                e->values.a0 = v;
                break;
            case 1:
                e->values.a1 = v;
                break;
            case 2:
                e->values.a2 = v;
                break;
            case 3:
                e->values.a3 = v;
                break;
            case 4:
                e->values.a4 = v;
                break;
            case 5:
                e->values.a5 = v;
                break;
            case 6:
                e->values.a6 = v;
                break;
            case 7:
                e->values.a7 = v;
                break;
            case 8:
                e->values.a8 = v;
                break;
            case 9:
                e->values.a9 = v;
                break;
            case 10:
                e->values.a10 = v;
                break;
            case 11:
                e->values.a11 = v;
                break;
            case 12:
                e->values.a12 = v;
                break;
            case 13:
                e->values.a13 = v;
                break;
            case 14:
                e->values.a14 = v;
                break;
            case 15:
                e->values.a15 = v;
                break;
            default:
                break;
        }
        setAlive(row);
    }
};

typedef struct {
    uint64_t a0 : 48 __attribute__((packed));
    uint64_t a1 : 48 __attribute__((packed));
    uint64_t a2 : 48 __attribute__((packed));
    uint64_t a3 : 48 __attribute__((packed));
    uint64_t a4 : 48 __attribute__((packed));
    uint64_t a5 : 48 __attribute__((packed));
    uint64_t a6 : 48 __attribute__((packed));
    uint64_t a7 : 48 __attribute__((packed));
    uint64_t a8 : 48 __attribute__((packed));
    uint64_t a9 : 48 __attribute__((packed));
    uint64_t a10 : 48 __attribute__((packed));
    uint64_t a11 : 48 __attribute__((packed));
    uint64_t a12 : 48 __attribute__((packed));
    uint64_t a13 : 48 __attribute__((packed));
    uint64_t a14 : 48 __attribute__((packed));
    uint64_t a15 : 48 __attribute__((packed));
} BitArray48;

typedef struct {
    uint16_t isNull;
    uint16_t isEndOfDocument;
    BitArray48 values;
} SM41;

class StorageModel41 : public StorageModel {
public:
    explicit StorageModel41(void *_data, void* _isDeleted) : StorageModel(_data,_isDeleted) {};
    void get(uint32_t row, __uint128_t &v, bool &isNull, bool &endOfDocument, bool &isDeleted) const override {
        uint32_t i = row&15;
        getPtrR(row,SM41);
        isNull = _bextr_u32(e->isNull,i,1);
        endOfDocument = _bextr_u32(e->isEndOfDocument,i,1);
        switch (i) {
            case 0:
                v = e->values.a0;
                break;
            case 1:
                v = e->values.a1;
                break;
            case 2:
                v = e->values.a2;
                break;
            case 3:
                v = e->values.a3;
                break;
            case 4:
                v = e->values.a4;
                break;
            case 5:
                v = e->values.a5;
                break;
            case 6:
                v = e->values.a6;
                break;
            case 7:
                v = e->values.a7;
                break;
            case 8:
                v = e->values.a8;
                break;
            case 9:
                v = e->values.a9;
                break;
            case 10:
                v = e->values.a10;
                break;
            case 11:
                v = e->values.a11;
                break;
            case 12:
                v = e->values.a12;
                break;
            case 13:
                v = e->values.a13;
                break;
            case 14:
                v = e->values.a14;
                break;
            case 15:
                v = e->values.a15;
                break;
            default:
                break;
        }
        isDeleted = getIsDeleted(row);
    }
    void save(uint32_t row, const __uint128_t &v, bool isNull, bool endOfDocument) override{
        uint32_t i = row&15;
        getPtrW(row,SM41);
        e->isNull |= isNull << i;
        e->isEndOfDocument |= endOfDocument << i;
        switch (i) {
            case 0:
                e->values.a0 = v;
                break;
            case 1:
                e->values.a1 = v;
                break;
            case 2:
                e->values.a2 = v;
                break;
            case 3:
                e->values.a3 = v;
                break;
            case 4:
                e->values.a4 = v;
                break;
            case 5:
                e->values.a5 = v;
                break;
            case 6:
                e->values.a6 = v;
                break;
            case 7:
                e->values.a7 = v;
                break;
            case 8:
                e->values.a8 = v;
                break;
            case 9:
                e->values.a9 = v;
                break;
            case 10:
                e->values.a10 = v;
                break;
            case 11:
                e->values.a11 = v;
                break;
            case 12:
                e->values.a12 = v;
                break;
            case 13:
                e->values.a13 = v;
                break;
            case 14:
                e->values.a14 = v;
                break;
            case 15:
                e->values.a15 = v;
                break;
            default:
                break;
        }
        setAlive(row);
    }
};

typedef struct {
    uint64_t a0 : 56 __attribute__((packed));
    uint64_t a1 : 56 __attribute__((packed));
    uint64_t a2 : 56 __attribute__((packed));
    uint64_t a3 : 56 __attribute__((packed));
    uint64_t a4 : 56 __attribute__((packed));
    uint64_t a5 : 56 __attribute__((packed));
    uint64_t a6 : 56 __attribute__((packed));
    uint64_t a7 : 56 __attribute__((packed));
    uint64_t a8 : 56 __attribute__((packed));
    uint64_t a9 : 56 __attribute__((packed));
    uint64_t a10 : 56 __attribute__((packed));
    uint64_t a11 : 56 __attribute__((packed));
    uint64_t a12 : 56 __attribute__((packed));
    uint64_t a13 : 56 __attribute__((packed));
    uint64_t a14 : 56 __attribute__((packed));
    uint64_t a15 : 56 __attribute__((packed));
} BitArray56;

typedef struct {
    uint16_t isNull;
    uint16_t isEndOfDocument;
    BitArray56 values;
} SM49;

class StorageModel49 : public StorageModel {
public:
    explicit StorageModel49(void *_data, void* _isDeleted) : StorageModel(_data,_isDeleted) {};
    void get(uint32_t row, __uint128_t &v, bool &isNull, bool &endOfDocument, bool &isDeleted) const override {
        uint32_t i = row&15;
        getPtrR(row,SM49);
        isNull = _bextr_u32(e->isNull,i,1);
        endOfDocument = _bextr_u32(e->isEndOfDocument,i,1);
        switch (i) {
            case 0:
                v = e->values.a0;
                break;
            case 1:
                v = e->values.a1;
                break;
            case 2:
                v = e->values.a2;
                break;
            case 3:
                v = e->values.a3;
                break;
            case 4:
                v = e->values.a4;
                break;
            case 5:
                v = e->values.a5;
                break;
            case 6:
                v = e->values.a6;
                break;
            case 7:
                v = e->values.a7;
                break;
            case 8:
                v = e->values.a8;
                break;
            case 9:
                v = e->values.a9;
                break;
            case 10:
                v = e->values.a10;
                break;
            case 11:
                v = e->values.a11;
                break;
            case 12:
                v = e->values.a12;
                break;
            case 13:
                v = e->values.a13;
                break;
            case 14:
                v = e->values.a14;
                break;
            case 15:
                v = e->values.a15;
                break;
            default:
                break;
        }
        isDeleted = getIsDeleted(row);
    }
    void save(uint32_t row, const __uint128_t &v, bool isNull, bool endOfDocument) override{
        uint32_t i = row&15;
        getPtrW(row,SM49);
        e->isNull |= isNull << i;
        e->isEndOfDocument |= endOfDocument << i;
        switch (i) {
            case 0:
                e->values.a0 = v;
                break;
            case 1:
                e->values.a1 = v;
                break;
            case 2:
                e->values.a2 = v;
                break;
            case 3:
                e->values.a3 = v;
                break;
            case 4:
                e->values.a4 = v;
                break;
            case 5:
                e->values.a5 = v;
                break;
            case 6:
                e->values.a6 = v;
                break;
            case 7:
                e->values.a7 = v;
                break;
            case 8:
                e->values.a8 = v;
                break;
            case 9:
                e->values.a9 = v;
                break;
            case 10:
                e->values.a10 = v;
                break;
            case 11:
                e->values.a11 = v;
                break;
            case 12:
                e->values.a12 = v;
                break;
            case 13:
                e->values.a13 = v;
                break;
            case 14:
                e->values.a14 = v;
                break;
            case 15:
                e->values.a15 = v;
                break;
            default:
                break;
        }
        setAlive(row);
    }
};

typedef struct {
    uint16_t isNull __attribute__((packed));
    uint16_t isEndOfDocument __attribute__((packed));
    uint64_t values[16] __attribute__((packed));
} SM57;

class StorageModel57 : public StorageModel {
public:
    explicit StorageModel57(void *_data, void* _isDeleted) : StorageModel(_data,_isDeleted) {};
    void get(uint32_t row, __uint128_t &v, bool &isNull, bool &endOfDocument, bool &isDeleted) const override {
        uint32_t i = row&15;
        getPtrR(row,SM57);
        isNull = _bextr_u32(e->isNull,i,1);
        endOfDocument = _bextr_u32(e->isEndOfDocument,i,1);
        v = e->values[i];
        isDeleted = getIsDeleted(row);
    }
    void save(uint32_t row, const __uint128_t &v, bool isNull, bool endOfDocument) override{
        uint32_t i = row&15;
        getPtrW(row,SM57);
        e->isNull |= isNull << i;
        e->isEndOfDocument |= endOfDocument << i;
        e->values[i] = v;
        setAlive(row);
    }
};

typedef struct {
    __uint128_t a0 : 72 __attribute__((packed));
    __uint128_t a1 : 72 __attribute__((packed));
    __uint128_t a2 : 72 __attribute__((packed));
    __uint128_t a3 : 72 __attribute__((packed));
    __uint128_t a4 : 72 __attribute__((packed));
    __uint128_t a5 : 72 __attribute__((packed));
    __uint128_t a6 : 72 __attribute__((packed));
    __uint128_t a7 : 72 __attribute__((packed));
    __uint128_t a8 : 72 __attribute__((packed));
    __uint128_t a9 : 72 __attribute__((packed));
    __uint128_t a10 : 72 __attribute__((packed));
    __uint128_t a11 : 72 __attribute__((packed));
    __uint128_t a12 : 72 __attribute__((packed));
    __uint128_t a13 : 72 __attribute__((packed));
    __uint128_t a14 : 72 __attribute__((packed));
    __uint128_t a15 : 72 __attribute__((packed));
} BitArray65;


typedef struct {
    uint16_t isNull;
    uint16_t isEndOfDocument;
    BitArray65 values;
} SM65;

class StorageModel65 : public StorageModel {
public:
    explicit StorageModel65(void *_data, void* _isDeleted) : StorageModel(_data,_isDeleted) {};
    void get(uint32_t row, __uint128_t &v, bool &isNull, bool &endOfDocument, bool &isDeleted) const override {
        uint32_t i = row&15;
        getPtrR(row,SM65);
        isNull = _bextr_u32(e->isNull,i,1);
        endOfDocument = _bextr_u32(e->isEndOfDocument,i,1);
        switch (i) {
            case 0:
                v = e->values.a0;
                break;
            case 1:
                v = e->values.a1;
                break;
            case 2:
                v = e->values.a2;
                break;
            case 3:
                v = e->values.a3;
                break;
            case 4:
                v = e->values.a4;
                break;
            case 5:
                v = e->values.a5;
                break;
            case 6:
                v = e->values.a6;
                break;
            case 7:
                v = e->values.a7;
                break;
            case 8:
                v = e->values.a8;
                break;
            case 9:
                v = e->values.a9;
                break;
            case 10:
                v = e->values.a10;
                break;
            case 11:
                v = e->values.a11;
                break;
            case 12:
                v = e->values.a12;
                break;
            case 13:
                v = e->values.a13;
                break;
            case 14:
                v = e->values.a14;
                break;
            case 15:
                v = e->values.a15;
                break;
            default:
                break;
        }
        isDeleted = getIsDeleted(row);
    }
    void save(uint32_t row, const __uint128_t &v, bool isNull, bool endOfDocument) override{
        uint32_t i = row&15;
        getPtrW(row,SM65);
        e->isNull |= isNull << i;
        e->isEndOfDocument |= endOfDocument << i;
        switch (i) {
            case 0:
                e->values.a0 = v;
                break;
            case 1:
                e->values.a1 = v;
                break;
            case 2:
                e->values.a2 = v;
                break;
            case 3:
                e->values.a3 = v;
                break;
            case 4:
                e->values.a4 = v;
                break;
            case 5:
                e->values.a5 = v;
                break;
            case 6:
                e->values.a6 = v;
                break;
            case 7:
                e->values.a7 = v;
                break;
            case 8:
                e->values.a8 = v;
                break;
            case 9:
                e->values.a9 = v;
                break;
            case 10:
                e->values.a10 = v;
                break;
            case 11:
                e->values.a11 = v;
                break;
            case 12:
                e->values.a12 = v;
                break;
            case 13:
                e->values.a13 = v;
                break;
            case 14:
                e->values.a14 = v;
                break;
            case 15:
                e->values.a15 = v;
                break;
            default:
                break;
        }
        setAlive(row);
    }
};

typedef struct {
    __uint128_t a0 : 80 __attribute__((packed));
    __uint128_t a1 : 80 __attribute__((packed));
    __uint128_t a2 : 80 __attribute__((packed));
    __uint128_t a3 : 80 __attribute__((packed));
    __uint128_t a4 : 80 __attribute__((packed));
    __uint128_t a5 : 80 __attribute__((packed));
    __uint128_t a6 : 80 __attribute__((packed));
    __uint128_t a7 : 80 __attribute__((packed));
    __uint128_t a8 : 80 __attribute__((packed));
    __uint128_t a9 : 80 __attribute__((packed));
    __uint128_t a10 : 80 __attribute__((packed));
    __uint128_t a11 : 80 __attribute__((packed));
    __uint128_t a12 : 80 __attribute__((packed));
    __uint128_t a13 : 80 __attribute__((packed));
    __uint128_t a14 : 80 __attribute__((packed));
    __uint128_t a15 : 80 __attribute__((packed));
} BitArray73;

typedef struct {
    uint16_t isNull;
    uint16_t isEndOfDocument;
    BitArray73 values;
} SM73;

class StorageModel73 : public StorageModel {
public:
    explicit StorageModel73(void *_data, void* _isDeleted) : StorageModel(_data,_isDeleted) {};
    void get(uint32_t row, __uint128_t &v, bool &isNull, bool &endOfDocument, bool &isDeleted) const override {
        uint32_t i = row&15;
        getPtrR(row,SM73);
        isNull = _bextr_u32(e->isNull,i,1);
        endOfDocument = _bextr_u32(e->isEndOfDocument,i,1);
        switch (i) {
            case 0:
                v = e->values.a0;
                break;
            case 1:
                v = e->values.a1;
                break;
            case 2:
                v = e->values.a2;
                break;
            case 3:
                v = e->values.a3;
                break;
            case 4:
                v = e->values.a4;
                break;
            case 5:
                v = e->values.a5;
                break;
            case 6:
                v = e->values.a6;
                break;
            case 7:
                v = e->values.a7;
                break;
            case 8:
                v = e->values.a8;
                break;
            case 9:
                v = e->values.a9;
                break;
            case 10:
                v = e->values.a10;
                break;
            case 11:
                v = e->values.a11;
                break;
            case 12:
                v = e->values.a12;
                break;
            case 13:
                v = e->values.a13;
                break;
            case 14:
                v = e->values.a14;
                break;
            case 15:
                v = e->values.a15;
                break;
            default:
                break;
        }
        isDeleted = getIsDeleted(row);
    }
    void save(uint32_t row, const __uint128_t &v, bool isNull, bool endOfDocument) override{
        uint32_t i = row&15;
        getPtrW(row,SM73);
        e->isNull |= isNull << i;
        e->isEndOfDocument |= endOfDocument << i;
        switch (i) {
            case 0:
                e->values.a0 = v;
                break;
            case 1:
                e->values.a1 = v;
                break;
            case 2:
                e->values.a2 = v;
                break;
            case 3:
                e->values.a3 = v;
                break;
            case 4:
                e->values.a4 = v;
                break;
            case 5:
                e->values.a5 = v;
                break;
            case 6:
                e->values.a6 = v;
                break;
            case 7:
                e->values.a7 = v;
                break;
            case 8:
                e->values.a8 = v;
                break;
            case 9:
                e->values.a9 = v;
                break;
            case 10:
                e->values.a10 = v;
                break;
            case 11:
                e->values.a11 = v;
                break;
            case 12:
                e->values.a12 = v;
                break;
            case 13:
                e->values.a13 = v;
                break;
            case 14:
                e->values.a14 = v;
                break;
            case 15:
                e->values.a15 = v;
                break;
            default:
                break;
        }
        setAlive(row);
    }
};

typedef struct {
    __uint128_t a0 : 88 __attribute__((packed));
    __uint128_t a1 : 88 __attribute__((packed));
    __uint128_t a2 : 88 __attribute__((packed));
    __uint128_t a3 : 88 __attribute__((packed));
    __uint128_t a4 : 88 __attribute__((packed));
    __uint128_t a5 : 88 __attribute__((packed));
    __uint128_t a6 : 88 __attribute__((packed));
    __uint128_t a7 : 88 __attribute__((packed));
    __uint128_t a8 : 88 __attribute__((packed));
    __uint128_t a9 : 88 __attribute__((packed));
    __uint128_t a10 : 88 __attribute__((packed));
    __uint128_t a11 : 88 __attribute__((packed));
    __uint128_t a12 : 88 __attribute__((packed));
    __uint128_t a13 : 88 __attribute__((packed));
    __uint128_t a14 : 88 __attribute__((packed));
    __uint128_t a15 : 88 __attribute__((packed));
} BitArray81;

typedef struct {
    uint16_t isNull;
    uint16_t isEndOfDocument;
    BitArray81 values;
} SM81;

class StorageModel81 : public StorageModel {
public:
    explicit StorageModel81(void *_data, void* _isDeleted) : StorageModel(_data,_isDeleted) {};
    void get(uint32_t row, __uint128_t &v, bool &isNull, bool &endOfDocument, bool &isDeleted) const override {
        uint32_t i = row&15;
        getPtrR(row,SM81);
        isNull = _bextr_u32(e->isNull,i,1);
        endOfDocument = _bextr_u32(e->isEndOfDocument,i,1);
        switch (i) {
            case 0:
                v = e->values.a0;
                break;
            case 1:
                v = e->values.a1;
                break;
            case 2:
                v = e->values.a2;
                break;
            case 3:
                v = e->values.a3;
                break;
            case 4:
                v = e->values.a4;
                break;
            case 5:
                v = e->values.a5;
                break;
            case 6:
                v = e->values.a6;
                break;
            case 7:
                v = e->values.a7;
                break;
            case 8:
                v = e->values.a8;
                break;
            case 9:
                v = e->values.a9;
                break;
            case 10:
                v = e->values.a10;
                break;
            case 11:
                v = e->values.a11;
                break;
            case 12:
                v = e->values.a12;
                break;
            case 13:
                v = e->values.a13;
                break;
            case 14:
                v = e->values.a14;
                break;
            case 15:
                v = e->values.a15;
                break;
            default:
                break;
        }
        isDeleted = getIsDeleted(row);
    }
    void save(uint32_t row, const __uint128_t &v, bool isNull, bool endOfDocument) override{
        uint32_t i = row&15;
        getPtrW(row,SM81);
        e->isNull |= isNull << i;
        e->isEndOfDocument |= endOfDocument << i;
        switch (i) {
            case 0:
                e->values.a0 = v;
                break;
            case 1:
                e->values.a1 = v;
                break;
            case 2:
                e->values.a2 = v;
                break;
            case 3:
                e->values.a3 = v;
                break;
            case 4:
                e->values.a4 = v;
                break;
            case 5:
                e->values.a5 = v;
                break;
            case 6:
                e->values.a6 = v;
                break;
            case 7:
                e->values.a7 = v;
                break;
            case 8:
                e->values.a8 = v;
                break;
            case 9:
                e->values.a9 = v;
                break;
            case 10:
                e->values.a10 = v;
                break;
            case 11:
                e->values.a11 = v;
                break;
            case 12:
                e->values.a12 = v;
                break;
            case 13:
                e->values.a13 = v;
                break;
            case 14:
                e->values.a14 = v;
                break;
            case 15:
                e->values.a15 = v;
                break;
            default:
                break;
        }
        setAlive(row);
    }
};

typedef struct {
    __uint128_t a0 : 96 __attribute__((packed));
    __uint128_t a1 : 96 __attribute__((packed));
    __uint128_t a2 : 96 __attribute__((packed));
    __uint128_t a3 : 96 __attribute__((packed));
    __uint128_t a4 : 96 __attribute__((packed));
    __uint128_t a5 : 96 __attribute__((packed));
    __uint128_t a6 : 96 __attribute__((packed));
    __uint128_t a7 : 96 __attribute__((packed));
    __uint128_t a8 : 96 __attribute__((packed));
    __uint128_t a9 : 96 __attribute__((packed));
    __uint128_t a10 : 96 __attribute__((packed));
    __uint128_t a11 : 96 __attribute__((packed));
    __uint128_t a12 : 96 __attribute__((packed));
    __uint128_t a13 : 96 __attribute__((packed));
    __uint128_t a14 : 96 __attribute__((packed));
    __uint128_t a15 : 96 __attribute__((packed));
} BitArray89;

typedef struct {
    uint16_t isNull;
    uint16_t isEndOfDocument;
    BitArray89 values;
} SM89;

class StorageModel89 : public StorageModel {
public:
    explicit StorageModel89(void *_data, void* _isDeleted) : StorageModel(_data,_isDeleted) {};
    void get(uint32_t row, __uint128_t &v, bool &isNull, bool &endOfDocument, bool &isDeleted) const override {
        uint32_t i = row&15;
        getPtrR(row,SM89);
        isNull = _bextr_u32(e->isNull,i,1);
        endOfDocument = _bextr_u32(e->isEndOfDocument,i,1);
        switch (i) {
            case 0:
                v = e->values.a0;
                break;
            case 1:
                v = e->values.a1;
                break;
            case 2:
                v = e->values.a2;
                break;
            case 3:
                v = e->values.a3;
                break;
            case 4:
                v = e->values.a4;
                break;
            case 5:
                v = e->values.a5;
                break;
            case 6:
                v = e->values.a6;
                break;
            case 7:
                v = e->values.a7;
                break;
            case 8:
                v = e->values.a8;
                break;
            case 9:
                v = e->values.a9;
                break;
            case 10:
                v = e->values.a10;
                break;
            case 11:
                v = e->values.a11;
                break;
            case 12:
                v = e->values.a12;
                break;
            case 13:
                v = e->values.a13;
                break;
            case 14:
                v = e->values.a14;
                break;
            case 15:
                v = e->values.a15;
                break;
            default:
                break;
        }
        isDeleted = getIsDeleted(row);
    }
    void save(uint32_t row, const __uint128_t &v, bool isNull, bool endOfDocument) override{
        uint32_t i = row&15;
        getPtrW(row,SM89);
        e->isNull |= isNull << i;
        e->isEndOfDocument |= endOfDocument << i;
        switch (i) {
            case 0:
                e->values.a0 = v;
                break;
            case 1:
                e->values.a1 = v;
                break;
            case 2:
                e->values.a2 = v;
                break;
            case 3:
                e->values.a3 = v;
                break;
            case 4:
                e->values.a4 = v;
                break;
            case 5:
                e->values.a5 = v;
                break;
            case 6:
                e->values.a6 = v;
                break;
            case 7:
                e->values.a7 = v;
                break;
            case 8:
                e->values.a8 = v;
                break;
            case 9:
                e->values.a9 = v;
                break;
            case 10:
                e->values.a10 = v;
                break;
            case 11:
                e->values.a11 = v;
                break;
            case 12:
                e->values.a12 = v;
                break;
            case 13:
                e->values.a13 = v;
                break;
            case 14:
                e->values.a14 = v;
                break;
            case 15:
                e->values.a15 = v;
                break;
            default:
                break;
        }
        setAlive(row);
    }
};

typedef struct {
    __uint128_t a0 : 104 __attribute__((packed));
    __uint128_t a1 : 104 __attribute__((packed));
    __uint128_t a2 : 104 __attribute__((packed));
    __uint128_t a3 : 104 __attribute__((packed));
    __uint128_t a4 : 104 __attribute__((packed));
    __uint128_t a5 : 104 __attribute__((packed));
    __uint128_t a6 : 104 __attribute__((packed));
    __uint128_t a7 : 104 __attribute__((packed));
    __uint128_t a8 : 104 __attribute__((packed));
    __uint128_t a9 : 104 __attribute__((packed));
    __uint128_t a10 : 104 __attribute__((packed));
    __uint128_t a11 : 104 __attribute__((packed));
    __uint128_t a12 : 104 __attribute__((packed));
    __uint128_t a13 : 104 __attribute__((packed));
    __uint128_t a14 : 104 __attribute__((packed));
    __uint128_t a15 : 104 __attribute__((packed));
} BitArray97;

typedef struct {
    uint16_t isNull;
    uint16_t isEndOfDocument;
    BitArray97 values;
} SM97;

class StorageModel97 : public StorageModel {
public:
    explicit StorageModel97(void *_data, void* _isDeleted) : StorageModel(_data,_isDeleted) {};
    void get(uint32_t row, __uint128_t &v, bool &isNull, bool &endOfDocument, bool &isDeleted) const override {
        uint32_t i = row&15;
        getPtrR(row,SM97);
        isNull = _bextr_u32(e->isNull,i,1);
        endOfDocument = _bextr_u32(e->isEndOfDocument,i,1);
        switch (i) {
            case 0:
                v = e->values.a0;
                break;
            case 1:
                v = e->values.a1;
                break;
            case 2:
                v = e->values.a2;
                break;
            case 3:
                v = e->values.a3;
                break;
            case 4:
                v = e->values.a4;
                break;
            case 5:
                v = e->values.a5;
                break;
            case 6:
                v = e->values.a6;
                break;
            case 7:
                v = e->values.a7;
                break;
            case 8:
                v = e->values.a8;
                break;
            case 9:
                v = e->values.a9;
                break;
            case 10:
                v = e->values.a10;
                break;
            case 11:
                v = e->values.a11;
                break;
            case 12:
                v = e->values.a12;
                break;
            case 13:
                v = e->values.a13;
                break;
            case 14:
                v = e->values.a14;
                break;
            case 15:
                v = e->values.a15;
                break;
            default:
                break;
        }
        isDeleted = getIsDeleted(row);
    }
    void save(uint32_t row, const __uint128_t &v, bool isNull, bool endOfDocument) override{
        uint32_t i = row&15;
        getPtrW(row,SM97);
        e->isNull |= isNull << i;
        e->isEndOfDocument |= endOfDocument << i;
        switch (i) {
            case 0:
                e->values.a0 = v;
                break;
            case 1:
                e->values.a1 = v;
                break;
            case 2:
                e->values.a2 = v;
                break;
            case 3:
                e->values.a3 = v;
                break;
            case 4:
                e->values.a4 = v;
                break;
            case 5:
                e->values.a5 = v;
                break;
            case 6:
                e->values.a6 = v;
                break;
            case 7:
                e->values.a7 = v;
                break;
            case 8:
                e->values.a8 = v;
                break;
            case 9:
                e->values.a9 = v;
                break;
            case 10:
                e->values.a10 = v;
                break;
            case 11:
                e->values.a11 = v;
                break;
            case 12:
                e->values.a12 = v;
                break;
            case 13:
                e->values.a13 = v;
                break;
            case 14:
                e->values.a14 = v;
                break;
            case 15:
                e->values.a15 = v;
                break;
            default:
                break;
        }
        setAlive(row);
    }
};

typedef struct {
    __uint128_t a0 : 112 __attribute__((packed));
    __uint128_t a1 : 112 __attribute__((packed));
    __uint128_t a2 : 112 __attribute__((packed));
    __uint128_t a3 : 112 __attribute__((packed));
    __uint128_t a4 : 112 __attribute__((packed));
    __uint128_t a5 : 112 __attribute__((packed));
    __uint128_t a6 : 112 __attribute__((packed));
    __uint128_t a7 : 112 __attribute__((packed));
    __uint128_t a8 : 112 __attribute__((packed));
    __uint128_t a9 : 112 __attribute__((packed));
    __uint128_t a10 : 112 __attribute__((packed));
    __uint128_t a11 : 112 __attribute__((packed));
    __uint128_t a12 : 112 __attribute__((packed));
    __uint128_t a13 : 112 __attribute__((packed));
    __uint128_t a14 : 112 __attribute__((packed));
    __uint128_t a15 : 112 __attribute__((packed));
} BitArray105;

typedef struct {
    uint16_t isNull;
    uint16_t isEndOfDocument;
    BitArray105 values;
} SM105;

class StorageModel105 : public StorageModel {
public:
    explicit StorageModel105(void *_data, void* _isDeleted) : StorageModel(_data,_isDeleted) {};
    void get(uint32_t row, __uint128_t &v, bool &isNull, bool &endOfDocument, bool &isDeleted) const override {
        uint32_t i = row&15;
        getPtrR(row,SM105);
        isNull = _bextr_u32(e->isNull,i,1);
        endOfDocument = _bextr_u32(e->isEndOfDocument,i,1);
        switch (i) {
            case 0:
                v = e->values.a0;
                break;
            case 1:
                v = e->values.a1;
                break;
            case 2:
                v = e->values.a2;
                break;
            case 3:
                v = e->values.a3;
                break;
            case 4:
                v = e->values.a4;
                break;
            case 5:
                v = e->values.a5;
                break;
            case 6:
                v = e->values.a6;
                break;
            case 7:
                v = e->values.a7;
                break;
            case 8:
                v = e->values.a8;
                break;
            case 9:
                v = e->values.a9;
                break;
            case 10:
                v = e->values.a10;
                break;
            case 11:
                v = e->values.a11;
                break;
            case 12:
                v = e->values.a12;
                break;
            case 13:
                v = e->values.a13;
                break;
            case 14:
                v = e->values.a14;
                break;
            case 15:
                v = e->values.a15;
                break;
            default:
                break;
        }
        isDeleted = getIsDeleted(row);
    }
    void save(uint32_t row, const __uint128_t &v, bool isNull, bool endOfDocument) override{
        uint32_t i = row&15;
        getPtrW(row,SM105);
        e->isNull |= isNull << i;
        e->isEndOfDocument |= endOfDocument << i;
        switch (i) {
            case 0:
                e->values.a0 = v;
                break;
            case 1:
                e->values.a1 = v;
                break;
            case 2:
                e->values.a2 = v;
                break;
            case 3:
                e->values.a3 = v;
                break;
            case 4:
                e->values.a4 = v;
                break;
            case 5:
                e->values.a5 = v;
                break;
            case 6:
                e->values.a6 = v;
                break;
            case 7:
                e->values.a7 = v;
                break;
            case 8:
                e->values.a8 = v;
                break;
            case 9:
                e->values.a9 = v;
                break;
            case 10:
                e->values.a10 = v;
                break;
            case 11:
                e->values.a11 = v;
                break;
            case 12:
                e->values.a12 = v;
                break;
            case 13:
                e->values.a13 = v;
                break;
            case 14:
                e->values.a14 = v;
                break;
            case 15:
                e->values.a15 = v;
                break;
            default:
                break;
        }
        setAlive(row);
    }
};

typedef struct {
    __uint128_t a0 : 120 __attribute__((packed));
    __uint128_t a1 : 120 __attribute__((packed));
    __uint128_t a2 : 120 __attribute__((packed));
    __uint128_t a3 : 120 __attribute__((packed));
    __uint128_t a4 : 120 __attribute__((packed));
    __uint128_t a5 : 120 __attribute__((packed));
    __uint128_t a6 : 120 __attribute__((packed));
    __uint128_t a7 : 120 __attribute__((packed));
    __uint128_t a8 : 120 __attribute__((packed));
    __uint128_t a9 : 120 __attribute__((packed));
    __uint128_t a10 : 120 __attribute__((packed));
    __uint128_t a11 : 120 __attribute__((packed));
    __uint128_t a12 : 120 __attribute__((packed));
    __uint128_t a13 : 120 __attribute__((packed));
    __uint128_t a14 : 120 __attribute__((packed));
    __uint128_t a15 : 120 __attribute__((packed));
} BitArray113;

typedef struct {
    uint16_t isNull;
    uint16_t isEndOfDocument;
    BitArray113 values;
} SM113;

class StorageModel113 : public StorageModel {
public:
    explicit StorageModel113(void *_data, void* _isDeleted) : StorageModel(_data,_isDeleted) {};
    void get(uint32_t row, __uint128_t &v, bool &isNull, bool &endOfDocument, bool &isDeleted) const override {
        uint32_t i = row&15;
        getPtrR(row,SM113);
        isNull = _bextr_u32(e->isNull,i,1);
        endOfDocument = _bextr_u32(e->isEndOfDocument,i,1);
        switch (i) {
            case 0:
                v = e->values.a0;
                break;
            case 1:
                v = e->values.a1;
                break;
            case 2:
                v = e->values.a2;
                break;
            case 3:
                v = e->values.a3;
                break;
            case 4:
                v = e->values.a4;
                break;
            case 5:
                v = e->values.a5;
                break;
            case 6:
                v = e->values.a6;
                break;
            case 7:
                v = e->values.a7;
                break;
            case 8:
                v = e->values.a8;
                break;
            case 9:
                v = e->values.a9;
                break;
            case 10:
                v = e->values.a10;
                break;
            case 11:
                v = e->values.a11;
                break;
            case 12:
                v = e->values.a12;
                break;
            case 13:
                v = e->values.a13;
                break;
            case 14:
                v = e->values.a14;
                break;
            case 15:
                v = e->values.a15;
                break;
            default:
                break;
        }
        isDeleted = getIsDeleted(row);
    }
    void save(uint32_t row, const __uint128_t &v, bool isNull, bool endOfDocument) override{
        uint32_t i = row&15;
        getPtrW(row,SM113);
        e->isNull |= isNull << i;
        e->isEndOfDocument |= endOfDocument << i;
        switch (i) {
            case 0:
                e->values.a0 = v;
                break;
            case 1:
                e->values.a1 = v;
                break;
            case 2:
                e->values.a2 = v;
                break;
            case 3:
                e->values.a3 = v;
                break;
            case 4:
                e->values.a4 = v;
                break;
            case 5:
                e->values.a5 = v;
                break;
            case 6:
                e->values.a6 = v;
                break;
            case 7:
                e->values.a7 = v;
                break;
            case 8:
                e->values.a8 = v;
                break;
            case 9:
                e->values.a9 = v;
                break;
            case 10:
                e->values.a10 = v;
                break;
            case 11:
                e->values.a11 = v;
                break;
            case 12:
                e->values.a12 = v;
                break;
            case 13:
                e->values.a13 = v;
                break;
            case 14:
                e->values.a14 = v;
                break;
            case 15:
                e->values.a15 = v;
                break;
            default:
                break;
        }
        setAlive(row);
    }
};

typedef struct {
    __uint128_t a0 : 128 __attribute__((packed));
    __uint128_t a1 : 128 __attribute__((packed));
    __uint128_t a2 : 128 __attribute__((packed));
    __uint128_t a3 : 128 __attribute__((packed));
    __uint128_t a4 : 128 __attribute__((packed));
    __uint128_t a5 : 128 __attribute__((packed));
    __uint128_t a6 : 128 __attribute__((packed));
    __uint128_t a7 : 128 __attribute__((packed));
    __uint128_t a8 : 128 __attribute__((packed));
    __uint128_t a9 : 128 __attribute__((packed));
    __uint128_t a10 : 128 __attribute__((packed));
    __uint128_t a11 : 128 __attribute__((packed));
    __uint128_t a12 : 128 __attribute__((packed));
    __uint128_t a13 : 128 __attribute__((packed));
    __uint128_t a14 : 128 __attribute__((packed));
    __uint128_t a15 : 128 __attribute__((packed));
} BitArray121;

typedef struct {
    uint16_t isNull;
    uint16_t isEndOfDocument;
    BitArray121 values;
} SM121;

class StorageModel121 : public StorageModel {
public:
    explicit StorageModel121(void *_data, void* _isDeleted) : StorageModel(_data,_isDeleted) {};
    void get(uint32_t row, __uint128_t &v, bool &isNull, bool &endOfDocument, bool &isDeleted) const override {
        uint32_t i = row&15;
        getPtrR(row,SM121);
        isNull = _bextr_u32(e->isNull,i,1);
        endOfDocument = _bextr_u32(e->isEndOfDocument,i,1);
        switch (i) {
            case 0:
                v = e->values.a0;
                break;
            case 1:
                v = e->values.a1;
                break;
            case 2:
                v = e->values.a2;
                break;
            case 3:
                v = e->values.a3;
                break;
            case 4:
                v = e->values.a4;
                break;
            case 5:
                v = e->values.a5;
                break;
            case 6:
                v = e->values.a6;
                break;
            case 7:
                v = e->values.a7;
                break;
            case 8:
                v = e->values.a8;
                break;
            case 9:
                v = e->values.a9;
                break;
            case 10:
                v = e->values.a10;
                break;
            case 11:
                v = e->values.a11;
                break;
            case 12:
                v = e->values.a12;
                break;
            case 13:
                v = e->values.a13;
                break;
            case 14:
                v = e->values.a14;
                break;
            case 15:
                v = e->values.a15;
                break;
            default:
                break;
        }
        isDeleted = getIsDeleted(row);
    }
    void save(uint32_t row, const __uint128_t &v, bool isNull, bool endOfDocument) override{
        uint32_t i = row&15;
        getPtrW(row,SM121);
        e->isNull |= isNull << i;
        e->isEndOfDocument |= endOfDocument << i;
        switch (i) {
            case 0:
                e->values.a0 = v;
                break;
            case 1:
                e->values.a1 = v;
                break;
            case 2:
                e->values.a2 = v;
                break;
            case 3:
                e->values.a3 = v;
                break;
            case 4:
                e->values.a4 = v;
                break;
            case 5:
                e->values.a5 = v;
                break;
            case 6:
                e->values.a6 = v;
                break;
            case 7:
                e->values.a7 = v;
                break;
            case 8:
                e->values.a8 = v;
                break;
            case 9:
                e->values.a9 = v;
                break;
            case 10:
                e->values.a10 = v;
                break;
            case 11:
                e->values.a11 = v;
                break;
            case 12:
                e->values.a12 = v;
                break;
            case 13:
                e->values.a13 = v;
                break;
            case 14:
                e->values.a14 = v;
                break;
            case 15:
                e->values.a15 = v;
                break;
            default:
                break;
        }
        setAlive(row);
    }
};

class StorageModelFactory {
public:
    static std::shared_ptr<StorageModel> getModel(void *data,void *isDeleted, uint32_t len) {
        switch (len) {
            case 1:
                return std::make_shared<StorageModel1>(data,isDeleted);
            case 2:
                return std::make_shared<StorageModel2>(data,isDeleted);
            case 3:
            case 4:
                return std::make_shared<StorageModel3>(data,isDeleted);
            case 5:
            case 6:
            case 7:
            case 8:
                return std::make_shared<StorageModel5>(data,isDeleted);
            case 9:
            case 10:
            case 11:
            case 12:
                return std::make_shared<StorageModel9>(data,isDeleted);
            case 13:
            case 14:
            case 15:
            case 16:
                return std::make_shared<StorageModel13>(data,isDeleted);
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
                return std::make_shared<StorageModel17>(data,isDeleted);
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
            case 30:
            case 31:
            case 32:
                return std::make_shared<StorageModel25>(data,isDeleted);
            case 33:
            case 34:
            case 35:
            case 36:
            case 37:
            case 38:
            case 39:
            case 40:
                return std::make_shared<StorageModel33>(data,isDeleted);
            case 41:
            case 42:
            case 43:
            case 44:
            case 45:
            case 46:
            case 47:
            case 48:
                return std::make_shared<StorageModel41>(data,isDeleted);
            case 49:
            case 50:
            case 51:
            case 52:
            case 53:
            case 54:
            case 55:
            case 56:
                return std::make_shared<StorageModel49>(data,isDeleted);
            case 57:
            case 58:
            case 59:
            case 60:
            case 61:
            case 62:
            case 63:
            case 64:
                return std::make_shared<StorageModel57>(data,isDeleted);
            case 65:
            case 66:
            case 67:
            case 68:
            case 69:
            case 70:
            case 71:
            case 72:
                return std::make_shared<StorageModel65>(data,isDeleted);
            case 73:
            case 74:
            case 75:
            case 76:
            case 77:
            case 78:
            case 79:
            case 80:
                return std::make_shared<StorageModel73>(data,isDeleted);
            case 81:
            case 82:
            case 83:
            case 84:
            case 85:
            case 86:
            case 87:
            case 88:
                return std::make_shared<StorageModel81>(data,isDeleted);
            case 89:
            case 90:
            case 91:
            case 92:
            case 93:
            case 94:
            case 95:
            case 96:
                return std::make_shared<StorageModel89>(data,isDeleted);
            case 97:
            case 98:
            case 99:
            case 100:
            case 101:
            case 102:
            case 103:
            case 104:
                return std::make_shared<StorageModel97>(data,isDeleted);
            case 105:
            case 106:
            case 107:
            case 108:
            case 109:
            case 110:
            case 111:
            case 112:
                return std::make_shared<StorageModel105>(data,isDeleted);
            case 113:
            case 114:
            case 115:
            case 116:
            case 117:
            case 118:
            case 119:
            case 120:
                return std::make_shared<StorageModel113>(data,isDeleted);
            case 121:
            case 122:
            case 123:
            case 124:
            case 125:
            case 126:
            case 127:
            case 128:
                return std::make_shared<StorageModel121>(data,isDeleted);
            default:
                return nullptr; // should never go here....
        }
    }

    static uint16_t getModelSize(uint32_t len) {
        switch (len) {
            case 1:
                return sizeof(SM1);
            case 2:
                return sizeof(SM2);
            case 3:
            case 4:
                return sizeof(SM3);
            case 5:
            case 6:
            case 7:
            case 8:
                return sizeof(SM5);
            case 9:
            case 10:
            case 11:
            case 12:
                return sizeof(SM9);
            case 13:
            case 14:
            case 15:
            case 16:
                return sizeof(SM13);
            case 17:
            case 18:
            case 19:
            case 20:
            case 21:
            case 22:
            case 23:
            case 24:
                return sizeof(SM17);
            case 25:
            case 26:
            case 27:
            case 28:
            case 29:
            case 30:
            case 31:
            case 32:
                return sizeof(SM25);
            case 33:
            case 34:
            case 35:
            case 36:
            case 37:
            case 38:
            case 39:
            case 40:
                return sizeof(SM33);
            case 41:
            case 42:
            case 43:
            case 44:
            case 45:
            case 46:
            case 47:
            case 48:
                return sizeof(SM41);
            case 49:
            case 50:
            case 51:
            case 52:
            case 53:
            case 54:
            case 55:
            case 56:
                return sizeof(SM49);
            case 57:
            case 58:
            case 59:
            case 60:
            case 61:
            case 62:
            case 63:
            case 64:
                return sizeof(SM57);
            case 65:
            case 66:
            case 67:
            case 68:
            case 69:
            case 70:
            case 71:
            case 72:
                return sizeof(SM65);
            case 73:
            case 74:
            case 75:
            case 76:
            case 77:
            case 78:
            case 79:
            case 80:
                return sizeof(SM73);
            case 81:
            case 82:
            case 83:
            case 84:
            case 85:
            case 86:
            case 87:
            case 88:
                return sizeof(SM81);
            case 89:
            case 90:
            case 91:
            case 92:
            case 93:
            case 94:
            case 95:
            case 96:
                return sizeof(SM89);
            case 97:
            case 98:
            case 99:
            case 100:
            case 101:
            case 102:
            case 103:
            case 104:
                return sizeof(SM97);
            case 105:
            case 106:
            case 107:
            case 108:
            case 109:
            case 110:
            case 111:
            case 112:
                return sizeof(SM105);
            case 113:
            case 114:
            case 115:
            case 116:
            case 117:
            case 118:
            case 119:
            case 120:
                return sizeof(SM113);
            case 121:
            case 122:
            case 123:
            case 124:
            case 125:
            case 126:
            case 127:
            case 128:
                return sizeof(SM121);
            default:
                return 0;
        }
    }
    template<TestOper oper>OperatorType convertToOperatorType() {
        if (oper == OperEqual) return OperEqualType;
        if (oper == OperEqualF16) return OperEqualType;
        if (oper == OperEqualF32) return OperEqualType;
        if (oper == OperEqualF64) return OperEqualType;
        if (oper == OperNotEqual) return OperNotEqualType;
        if (oper == OperNotEqualF16) return OperNotEqualType;
        if (oper == OperNotEqualF32) return OperNotEqualType;
        if (oper == OperNotEqualF64) return OperNotEqualType;
        if (oper == OperLTS) return OperLTType;
        if (oper == OperLTU) return OperLTType;
        if (oper == OperGTS) return OperGTType;
        if (oper == OperGTU) return OperGTType;
        if (oper == OperLES) return OperLEType;
        if (oper == OperLEU) return OperLEType;
        if (oper == OperGES) return OperGEType;
        if (oper == OperGEU) return OperGEType;
        if (oper == OperLTF16) return OperLTType;
        if (oper == OperLTF32) return OperLTType;
        if (oper == OperLTF64) return OperLTType;
        if (oper == OperGTF16) return OperGTType;
        if (oper == OperGTF32) return OperGTType;
        if (oper == OperGTF64) return OperGTType;
        if (oper == OperLEF16) return OperLEType;
        if (oper == OperLEF32) return OperLEType;
        if (oper == OperLEF64) return OperLEType;
        if (oper == OperGEF16) return OperGEType;
        if (oper == OperGEF32) return OperGEType;
        if (oper == OperGEF64) return OperGEType;
    }

#if 0
    static constexpr UnpackFunc kernelProgUnpackUnsigned_L[] = {
            unpack_unsigned_1_32_L,  unpack_unsigned_2_32_L,  unpack_unsigned_3_32_L,  unpack_unsigned_4_32_L,  unpack_unsigned_5_32_L,  unpack_unsigned_6_32_L,  unpack_unsigned_7_32_L,  unpack_unsigned_8_32_L,
            unpack_unsigned_9_32_L,  unpack_unsigned_10_32_L, unpack_unsigned_11_32_L, unpack_unsigned_12_32_L, unpack_unsigned_13_32_L, unpack_unsigned_14_32_L, unpack_unsigned_15_32_L, unpack_unsigned_16_32_L,
            unpack_unsigned_17_32_L, unpack_unsigned_18_32_L, unpack_unsigned_19_32_L, unpack_unsigned_20_32_L, unpack_unsigned_21_32_L, unpack_unsigned_22_32_L, unpack_unsigned_23_32_L, unpack_unsigned_24_32_L,
            unpack_unsigned_25_32_L, unpack_unsigned_26_32_L, unpack_unsigned_27_32_L, unpack_unsigned_28_32_L, unpack_unsigned_29_32_L, unpack_unsigned_30_32_L, unpack_unsigned_31_32_L, unpack_unsigned_32_32_L,
            unpack_unsigned_33_64_L, unpack_unsigned_34_64_L, unpack_unsigned_35_64_L, unpack_unsigned_36_64_L, unpack_unsigned_37_64_L, unpack_unsigned_38_64_L, unpack_unsigned_39_64_L, unpack_unsigned_40_64_L,
            unpack_unsigned_41_64_L, unpack_unsigned_42_64_L, unpack_unsigned_43_64_L, unpack_unsigned_44_64_L, unpack_unsigned_45_64_L, unpack_unsigned_46_64_L, unpack_unsigned_47_64_L, unpack_unsigned_48_64_L,
            unpack_unsigned_49_64_L, unpack_unsigned_50_64_L, unpack_unsigned_51_64_L, unpack_unsigned_52_64_L, unpack_unsigned_53_64_L, unpack_unsigned_54_64_L, unpack_unsigned_55_64_L, unpack_unsigned_56_64_L,
            unpack_unsigned_57_64_L, unpack_unsigned_58_64_L, unpack_unsigned_59_64_L, unpack_unsigned_60_64_L, unpack_unsigned_61_64_L, unpack_unsigned_62_64_L, unpack_unsigned_63_64_L, unpack_unsigned_64_64_L,
            unpack_unsigned_65_128_L, unpack_unsigned_66_128_L, unpack_unsigned_67_128_L, unpack_unsigned_68_128_L, unpack_unsigned_69_128_L, unpack_unsigned_70_128_L, unpack_unsigned_71_128_L, unpack_unsigned_72_128_L,
            unpack_unsigned_73_128_L, unpack_unsigned_74_128_L, unpack_unsigned_75_128_L, unpack_unsigned_76_128_L, unpack_unsigned_77_128_L, unpack_unsigned_78_128_L, unpack_unsigned_79_128_L, unpack_unsigned_80_128_L,
            unpack_unsigned_81_128_L, unpack_unsigned_82_128_L, unpack_unsigned_83_128_L, unpack_unsigned_84_128_L, unpack_unsigned_85_128_L, unpack_unsigned_86_128_L, unpack_unsigned_87_128_L, unpack_unsigned_88_128_L,
            unpack_unsigned_89_128_L, unpack_unsigned_90_128_L, unpack_unsigned_91_128_L, unpack_unsigned_92_128_L, unpack_unsigned_93_128_L, unpack_unsigned_94_128_L, unpack_unsigned_95_128_L, unpack_unsigned_96_128_L,
            unpack_unsigned_97_128_L, unpack_unsigned_98_128_L, unpack_unsigned_99_128_L, unpack_unsigned_100_128_L, unpack_unsigned_101_128_L, unpack_unsigned_102_128_L, unpack_unsigned_103_128_L, unpack_unsigned_104_128_L,
            unpack_unsigned_105_128_L, unpack_unsigned_106_128_L, unpack_unsigned_107_128_L, unpack_unsigned_108_128_L, unpack_unsigned_109_128_L, unpack_unsigned_110_128_L, unpack_unsigned_111_128_L, unpack_unsigned_112_128_L,
            unpack_unsigned_113_128_L, unpack_unsigned_114_128_L, unpack_unsigned_115_128_L, unpack_unsigned_116_128_L, unpack_unsigned_117_128_L, unpack_unsigned_118_128_L, unpack_unsigned_119_128_L, unpack_unsigned_120_128_L,
            unpack_unsigned_121_128_L, unpack_unsigned_122_128_L, unpack_unsigned_123_128_L, unpack_unsigned_124_128_L, unpack_unsigned_125_128_L, unpack_unsigned_126_128_L, unpack_unsigned_127_128_L, unpack_unsigned_128_128_L

    };
    static constexpr UnpackFunc kernelProgUnpackUnsigned_R[] = {
            unpack_unsigned_1_32_R,  unpack_unsigned_2_32_R,  unpack_unsigned_3_32_R,  unpack_unsigned_4_32_R,  unpack_unsigned_5_32_R,  unpack_unsigned_6_32_R,  unpack_unsigned_7_32_R,  unpack_unsigned_8_32_R,
            unpack_unsigned_9_32_R,  unpack_unsigned_10_32_R, unpack_unsigned_11_32_R, unpack_unsigned_12_32_R, unpack_unsigned_13_32_R, unpack_unsigned_14_32_R, unpack_unsigned_15_32_R, unpack_unsigned_16_32_R,
            unpack_unsigned_17_32_R, unpack_unsigned_18_32_R, unpack_unsigned_19_32_R, unpack_unsigned_20_32_R, unpack_unsigned_21_32_R, unpack_unsigned_22_32_R, unpack_unsigned_23_32_R, unpack_unsigned_24_32_R,
            unpack_unsigned_25_32_R, unpack_unsigned_26_32_R, unpack_unsigned_27_32_R, unpack_unsigned_28_32_R, unpack_unsigned_29_32_R, unpack_unsigned_30_32_R, unpack_unsigned_31_32_R, unpack_unsigned_32_32_R,
            unpack_unsigned_33_64_R, unpack_unsigned_34_64_R, unpack_unsigned_35_64_R, unpack_unsigned_36_64_R, unpack_unsigned_37_64_R, unpack_unsigned_38_64_R, unpack_unsigned_39_64_R, unpack_unsigned_40_64_R,
            unpack_unsigned_41_64_R, unpack_unsigned_42_64_R, unpack_unsigned_43_64_R, unpack_unsigned_44_64_R, unpack_unsigned_45_64_R, unpack_unsigned_46_64_R, unpack_unsigned_47_64_R, unpack_unsigned_48_64_R,
            unpack_unsigned_49_64_R, unpack_unsigned_50_64_R, unpack_unsigned_51_64_R, unpack_unsigned_52_64_R, unpack_unsigned_53_64_R, unpack_unsigned_54_64_R, unpack_unsigned_55_64_R, unpack_unsigned_56_64_R,
            unpack_unsigned_57_64_R, unpack_unsigned_58_64_R, unpack_unsigned_59_64_R, unpack_unsigned_60_64_R, unpack_unsigned_61_64_R, unpack_unsigned_62_64_R, unpack_unsigned_63_64_R, unpack_unsigned_64_64_R,
            unpack_unsigned_65_128_R, unpack_unsigned_66_128_R, unpack_unsigned_67_128_R, unpack_unsigned_68_128_R, unpack_unsigned_69_128_R, unpack_unsigned_70_128_R, unpack_unsigned_71_128_R, unpack_unsigned_72_128_R,
            unpack_unsigned_73_128_R, unpack_unsigned_74_128_R, unpack_unsigned_75_128_R, unpack_unsigned_76_128_R, unpack_unsigned_77_128_R, unpack_unsigned_78_128_R, unpack_unsigned_79_128_R, unpack_unsigned_80_128_R,
            unpack_unsigned_81_128_R, unpack_unsigned_82_128_R, unpack_unsigned_83_128_R, unpack_unsigned_84_128_R, unpack_unsigned_85_128_R, unpack_unsigned_86_128_R, unpack_unsigned_87_128_R, unpack_unsigned_88_128_R,
            unpack_unsigned_89_128_R, unpack_unsigned_90_128_R, unpack_unsigned_91_128_R, unpack_unsigned_92_128_R, unpack_unsigned_93_128_R, unpack_unsigned_94_128_R, unpack_unsigned_95_128_R, unpack_unsigned_96_128_R,
            unpack_unsigned_97_128_R, unpack_unsigned_98_128_R, unpack_unsigned_99_128_R, unpack_unsigned_100_128_R, unpack_unsigned_101_128_R, unpack_unsigned_102_128_R, unpack_unsigned_103_128_R, unpack_unsigned_104_128_R,
            unpack_unsigned_105_128_R, unpack_unsigned_106_128_R, unpack_unsigned_107_128_R, unpack_unsigned_108_128_R, unpack_unsigned_109_128_R, unpack_unsigned_110_128_R, unpack_unsigned_111_128_R, unpack_unsigned_112_128_R,
            unpack_unsigned_113_128_R, unpack_unsigned_114_128_R, unpack_unsigned_115_128_R, unpack_unsigned_116_128_R, unpack_unsigned_117_128_R, unpack_unsigned_118_128_R, unpack_unsigned_119_128_R, unpack_unsigned_120_128_R,
            unpack_unsigned_121_128_R, unpack_unsigned_122_128_R, unpack_unsigned_123_128_R, unpack_unsigned_124_128_R, unpack_unsigned_125_128_R, unpack_unsigned_126_128_R, unpack_unsigned_127_128_R, unpack_unsigned_128_128_R
    };

    static constexpr UnpackFunc kernelProgUnpackSigned_L[] = {
            unpack_signed_1_32_L,  unpack_signed_2_32_L,  unpack_signed_3_32_L,  unpack_signed_4_32_L,  unpack_signed_5_32_L,  unpack_signed_6_32_L,  unpack_signed_7_32_L,  unpack_signed_8_32_L,
            unpack_signed_9_32_L,  unpack_signed_10_32_L, unpack_signed_11_32_L, unpack_signed_12_32_L, unpack_signed_13_32_L, unpack_signed_14_32_L, unpack_signed_15_32_L, unpack_signed_16_32_L,
            unpack_signed_17_32_L, unpack_signed_18_32_L, unpack_signed_19_32_L, unpack_signed_20_32_L, unpack_signed_21_32_L, unpack_signed_22_32_L, unpack_signed_23_32_L, unpack_signed_24_32_L,
            unpack_signed_25_32_L, unpack_signed_26_32_L, unpack_signed_27_32_L, unpack_signed_28_32_L, unpack_signed_29_32_L, unpack_signed_30_32_L, unpack_signed_31_32_L, unpack_signed_32_32_L,
            unpack_signed_33_64_L, unpack_signed_34_64_L, unpack_signed_35_64_L, unpack_signed_36_64_L, unpack_signed_37_64_L, unpack_signed_38_64_L, unpack_signed_39_64_L, unpack_signed_40_64_L,
            unpack_signed_41_64_L, unpack_signed_42_64_L, unpack_signed_43_64_L, unpack_signed_44_64_L, unpack_signed_45_64_L, unpack_signed_46_64_L, unpack_signed_47_64_L, unpack_signed_48_64_L,
            unpack_signed_49_64_L, unpack_signed_50_64_L, unpack_signed_51_64_L, unpack_signed_52_64_L, unpack_signed_53_64_L, unpack_signed_54_64_L, unpack_signed_55_64_L, unpack_signed_56_64_L,
            unpack_signed_57_64_L, unpack_signed_58_64_L, unpack_signed_59_64_L, unpack_signed_60_64_L, unpack_signed_61_64_L, unpack_signed_62_64_L, unpack_signed_63_64_L, unpack_signed_64_64_L,
            unpack_signed_65_128_L, unpack_signed_66_128_L, unpack_signed_67_128_L, unpack_signed_68_128_L, unpack_signed_69_128_L, unpack_signed_70_128_L, unpack_signed_71_128_L, unpack_signed_72_128_L,
            unpack_signed_73_128_L, unpack_signed_74_128_L, unpack_signed_75_128_L, unpack_signed_76_128_L, unpack_signed_77_128_L, unpack_signed_78_128_L, unpack_signed_79_128_L, unpack_signed_80_128_L,
            unpack_signed_81_128_L, unpack_signed_82_128_L, unpack_signed_83_128_L, unpack_signed_84_128_L, unpack_signed_85_128_L, unpack_signed_86_128_L, unpack_signed_87_128_L, unpack_signed_88_128_L,
            unpack_signed_89_128_L, unpack_signed_90_128_L, unpack_signed_91_128_L, unpack_signed_92_128_L, unpack_signed_93_128_L, unpack_signed_94_128_L, unpack_signed_95_128_L, unpack_signed_96_128_L,
            unpack_signed_97_128_L, unpack_signed_98_128_L, unpack_signed_99_128_L, unpack_signed_100_128_L, unpack_signed_101_128_L, unpack_signed_102_128_L, unpack_signed_103_128_L, unpack_signed_104_128_L,
            unpack_signed_105_128_L, unpack_signed_106_128_L, unpack_signed_107_128_L, unpack_signed_108_128_L, unpack_signed_109_128_L, unpack_signed_110_128_L, unpack_signed_111_128_L, unpack_signed_112_128_L,
            unpack_signed_113_128_L, unpack_signed_114_128_L, unpack_signed_115_128_L, unpack_signed_116_128_L, unpack_signed_117_128_L, unpack_signed_118_128_L, unpack_signed_119_128_L, unpack_signed_120_128_L,
            unpack_signed_121_128_L, unpack_signed_122_128_L, unpack_signed_123_128_L, unpack_signed_124_128_L, unpack_signed_125_128_L, unpack_signed_126_128_L, unpack_signed_127_128_L, unpack_signed_128_128_L
    };
    static constexpr UnpackFunc kernelProgUnpackSigned_R[] = {
            unpack_signed_1_32_R,  unpack_signed_2_32_R,  unpack_signed_3_32_R,  unpack_signed_4_32_R,  unpack_signed_5_32_R,  unpack_signed_6_32_R,  unpack_signed_7_32_R,  unpack_signed_8_32_R,
            unpack_signed_9_32_R,  unpack_signed_10_32_R, unpack_signed_11_32_R, unpack_signed_12_32_R, unpack_signed_13_32_R, unpack_signed_14_32_R, unpack_signed_15_32_R, unpack_signed_16_32_R,
            unpack_signed_17_32_R, unpack_signed_18_32_R, unpack_signed_19_32_R, unpack_signed_20_32_R, unpack_signed_21_32_R, unpack_signed_22_32_R, unpack_signed_23_32_R, unpack_signed_24_32_R,
            unpack_signed_25_32_R, unpack_signed_26_32_R, unpack_signed_27_32_R, unpack_signed_28_32_R, unpack_signed_29_32_R, unpack_signed_30_32_R, unpack_signed_31_32_R, unpack_signed_32_32_R,
            unpack_signed_33_64_R, unpack_signed_34_64_R, unpack_signed_35_64_R, unpack_signed_36_64_R, unpack_signed_37_64_R, unpack_signed_38_64_R, unpack_signed_39_64_R, unpack_signed_40_64_R,
            unpack_signed_41_64_R, unpack_signed_42_64_R, unpack_signed_43_64_R, unpack_signed_44_64_R, unpack_signed_45_64_R, unpack_signed_46_64_R, unpack_signed_47_64_R, unpack_signed_48_64_R,
            unpack_signed_49_64_R, unpack_signed_50_64_R, unpack_signed_51_64_R, unpack_signed_52_64_R, unpack_signed_53_64_R, unpack_signed_54_64_R, unpack_signed_55_64_R, unpack_signed_56_64_R,
            unpack_signed_57_64_R, unpack_signed_58_64_R, unpack_signed_59_64_R, unpack_signed_60_64_R, unpack_signed_61_64_R, unpack_signed_62_64_R, unpack_signed_63_64_R, unpack_signed_64_64_R,
            unpack_signed_65_128_R, unpack_signed_66_128_R, unpack_signed_67_128_R, unpack_signed_68_128_R, unpack_signed_69_128_R, unpack_signed_70_128_R, unpack_signed_71_128_R, unpack_signed_72_128_R,
            unpack_signed_73_128_R, unpack_signed_74_128_R, unpack_signed_75_128_R, unpack_signed_76_128_R, unpack_signed_77_128_R, unpack_signed_78_128_R, unpack_signed_79_128_R, unpack_signed_80_128_R,
            unpack_signed_81_128_R, unpack_signed_82_128_R, unpack_signed_83_128_R, unpack_signed_84_128_R, unpack_signed_85_128_R, unpack_signed_86_128_R, unpack_signed_87_128_R, unpack_signed_88_128_R,
            unpack_signed_89_128_R, unpack_signed_90_128_R, unpack_signed_91_128_R, unpack_signed_92_128_R, unpack_signed_93_128_R, unpack_signed_94_128_R, unpack_signed_95_128_R, unpack_signed_96_128_R,
            unpack_signed_97_128_R, unpack_signed_98_128_R, unpack_signed_99_128_R, unpack_signed_100_128_R, unpack_signed_101_128_R, unpack_signed_102_128_R, unpack_signed_103_128_R, unpack_signed_104_128_R,
            unpack_signed_105_128_R, unpack_signed_106_128_R, unpack_signed_107_128_R, unpack_signed_108_128_R, unpack_signed_109_128_R, unpack_signed_110_128_R, unpack_signed_111_128_R, unpack_signed_112_128_R,
            unpack_signed_113_128_R, unpack_signed_114_128_R, unpack_signed_115_128_R, unpack_signed_116_128_R, unpack_signed_117_128_R, unpack_signed_118_128_R, unpack_signed_119_128_R, unpack_signed_120_128_R,
            unpack_signed_121_128_R, unpack_signed_122_128_R, unpack_signed_123_128_R, unpack_signed_124_128_R, unpack_signed_125_128_R, unpack_signed_126_128_R, unpack_signed_127_128_R, unpack_signed_128_128_R
    };

    static UnpackFunc getKernelUnpackASM(uint32_t len,mapping::storage_types t, bool left) {
        switch (t) {
            case mapping::storage_type_unknown:
            case mapping::storage_type_null:
            case mapping::storage_type_enum:
                return nullptr;
                break;
            case mapping::storage_type_signed:
            case mapping::storage_type_signed_fixed_point:
                return left ? kernelProgUnpackSigned_L[len - 1] : kernelProgUnpackSigned_R[len - 1];
            case mapping::storage_type_unsigned:
            case mapping::storage_type_unsigned_fixed_point:
                return left ? kernelProgUnpackUnsigned_L[len - 1] : kernelProgUnpackUnsigned_R[len - 1];
            case mapping::storage_type_float16:
                return left ? unpack_float16_L : unpack_float16_R;
            case mapping::storage_type_float:
                return left ? unpack_float_L : unpack_float_R;
            case mapping::storage_type_double:
                return left ? unpack_double_L : unpack_double_R;
            case mapping::storage_type_geopoint:
                // FIXME
                return nullptr;
        }
        return nullptr;
    }

    static AsmKernelDef getKernelProgASM(OperatorType type,uint32_t lenLeft,uint32_t lenRight,bool leftIsConst,bool rightIsConst,mapping::storage_types dt1, mapping::storage_types dt2) {
        AsmKernelDef ret;
        ret.rightIsConst = rightIsConst;
        bool is_double = isDouble(dt1) || isDouble(dt2);
        bool is_float = !is_double && (isFloat(dt1) || isFloat(dt2) || isFloat16(dt1) || isFloat16(dt2));
        if (is_float && (lenLeft > 32 || lenRight > 32)) {
            is_float = false;
            is_double = true;
        }
        bool is_b128 = (lenLeft >= 64 || lenRight >= 64) & !is_double;
        bool is_b64 = (lenLeft >= 32 || lenRight >= 32) && !is_b128 && !is_double && !is_float;

        int i = 0;
        if (!leftIsConst) {
            ret.chain[i++] = getKernelUnpackASM(lenLeft, dt1, true);
        }
        if (!rightIsConst) {
            ret.chain[i++] = getKernelUnpackASM(lenRight, dt2, false);
        }
        if (is_double) {
            std::cout << "is_double " << std::endl;
            // LEFT
            if (isInt(dt1)) {
                if (lenLeft < 32) {
                    ret.chain[i++] = cvt_signed_32_double_L;
                } else if (lenLeft == 32) {
                    if (isSignedInt(dt1)) {
                        ret.chain[i++] = cvt_signed_32_double_L;
                    } else {
                        ret.chain[i++] = cvt_unsigned_32_64_L;
                        ret.chain[i++] = cvt_signed_64_double_L;
                    }
                } else if (lenLeft <= 64) {
                    if (isSignedInt(dt1)) {
                        ret.chain[i++] = cvt_signed_64_double_L;
                    } else {
                        if (lenLeft < 64) {
                            ret.chain[i++] = cvt_signed_64_double_L;
                        } else {
                            Log::info("During 64 bit unsigned int to double conversion, if upper bit is set, wrong value will be used. Make sure numbers are below 2^63");
                            ret.chain[i++] = cvt_signed_64_double_L;
                        }
                    }
                } else {
                    Log::error("Conversion from numbers with more than 64 bits to double not supported yet");
                }
            } else if (isFloat(dt1)) {
                ret.chain[i++] = cvt_float_double_L;
            }
            // Right
            if (!rightIsConst) {
                if (isInt(dt2)) {
                    if (lenRight < 32) {
                        ret.chain[i++] = cvt_signed_32_double_R;
                    } else if (lenRight == 32) {
                        if (isSignedInt(dt2)) {
                            ret.chain[i++] = cvt_signed_32_double_R;
                        } else {
                            ret.chain[i++] = cvt_unsigned_32_64_R;
                            ret.chain[i++] = cvt_signed_64_double_R;
                        }
                    } else if (lenRight <= 64) {
                        if (isSignedInt(dt2)) {
                            ret.chain[i++] = cvt_signed_64_double_R;
                        } else {
                            if (lenRight < 64) {
                                ret.chain[i++] = cvt_signed_64_double_R;
                            } else {
                                Log::info("During 64 bit unsigned int to double conversion, if upper bit is set, wrong value will be used. Make sure numbers are below 2^63");
                                ret.chain[i++] = cvt_signed_64_double_R;
                            }
                        }
                    } else {
                        Log::error("Conversion from numbers with more than 64 bits to double not supported yet");
                    }
                } else if (isFloat(dt2)) {
                    ret.chain[i++] = cvt_float_double_R;
                }
            }
            ret.chain[i++] = reinterpret_cast<UnpackFunc>(sub_double);
        } else if (is_float) {
            if (isInt(dt1)) {
                if (lenLeft < 32) {
                    ret.chain[i++] = cvt_signed_32_float_L;
                } else {
                    std::cout << "Error should not go here ....." << std::endl;
                }
            } else if (isDouble(dt1)) {
                std::cout << "Error should not go here ....." << std::endl;
            }
            // Right
            if (!rightIsConst) {
                if (isInt(dt1)) {
                    if (lenRight < 32) {
                        ret.chain[i++] = cvt_signed_32_float_R;
                    } else {
                        std::cout << "Error should not go here ....." << std::endl;
                    }
                } else if (isDouble(dt2)) {
                    std::cout << "Error should not go here ....." << std::endl;
                }
            }
            ret.chain[i++] = reinterpret_cast<UnpackFunc>(sub_float);
        } else if (is_b128) {
            if (isInt(dt1)) {
                if (lenLeft <= 32) {
                    if (isSignedInt(dt1)) {
                        ret.chain[i++] = cvt_signed_32_64_L;
                        ret.chain[i++] = cvt_signed_64_128_L;
                    } else {
                        ret.chain[i++] = cvt_unsigned_32_64_L;
                        ret.chain[i++] = cvt_unsigned_64_128_L;
                    }
                } else if (lenLeft <= 64){
                    if (isSignedInt(dt1)) {
                        ret.chain[i++] = cvt_signed_64_128_L;
                    } else {
                        ret.chain[i++] = cvt_unsigned_64_128_L;
                    }
                }
            }
            // Right
            if (!rightIsConst) {
                if (isInt(dt2)) {
                    if (lenRight <= 32) {
                        if (isSignedInt(dt2)) {
                            ret.chain[i++] = cvt_signed_32_64_R;
                            ret.chain[i++] = cvt_signed_64_128_R;
                        } else {
                            ret.chain[i++] = cvt_unsigned_32_64_R;
                            ret.chain[i++] = cvt_unsigned_64_128_R;
                        }
                    } else if (lenRight <= 64){
                        if (isSignedInt(dt2)) {
                            ret.chain[i++] = cvt_signed_64_128_R;
                        } else {
                            ret.chain[i++] = cvt_unsigned_64_128_R;
                        }
                    }
                }
            }
            if (type == OperEqualType || type == OperNotEqualType) {
                ret.chain[i++] = reinterpret_cast<UnpackFunc>(sub_128bq);
            } else {;
                ret.chain[i++] = reinterpret_cast<UnpackFunc>(sub_128b);
            }
        } else if (is_b64) {
            std::cout << "b64 " << std::endl;
            if (isInt(dt1)) {
                if (lenLeft <= 32) {
                    if (isSignedInt(dt1)) {
                        std::cout << "cvt_signed_32_64_L" << std::endl;
                        ret.chain[i++] = cvt_signed_32_64_L;
                    } else {
                        ret.chain[i++] = cvt_unsigned_32_64_L;
                    }
                }
            }
            // Right
            if (!rightIsConst) {
                if (isInt(dt2)) {
                    if (lenRight <= 32) {
                        if (isSignedInt(dt2)) {
                            ret.chain[i++] = cvt_signed_32_64_R;
                        } else {
                            ret.chain[i++] = cvt_unsigned_32_64_R;
                        }
                    }
                }
            }
            ret.chain[i++] = reinterpret_cast<UnpackFunc>(sub_64b);
        } else {
            // already as int/uint 32
            ret.chain[i++] = reinterpret_cast<UnpackFunc>(sub_32b);
        }

        switch (type) {
            case OperEqualType: {
                if (is_double) ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmpz_double);
                else if (is_float) ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmpz_float);
                else if (is_b128) ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmpz_128b);
                else if (is_b64) ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmpz_64b);
                else ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmpz_32b);
            }
                break;
            case OperNotEqualType:
                if (is_double) ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmpnz_double);
                else if (is_float) ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmpnz_float);
                else if (is_b128) ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmpnz_128b);
                else if (is_b64) ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmpnz_64b);
                else ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmpnz_32b);
                break;
            case OperLTType:
                if (is_double) ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmplt_double);
                else if (is_float) ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmplt_float);
                else if (is_b128) ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmplt_128b);
                else if (is_b64) ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmplt_64b);
                else ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmplt_32b);
                break;
            case OperGTType:
                if (is_double) ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmpgt_double);
                else if (is_float) ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmpgt_float);
                else if (is_b128) ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmpgt_128b);
                else if (is_b64) ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmpgt_64b);
                else ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmpgt_32b);
                break;
            case OperLEType:
                if (is_double) ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmple_double);
                else if (is_float) ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmple_float);
                else if (is_b128) ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmple_128b);
                else if (is_b64) ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmple_64b);
                else  ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmple_32b);
                break;
            case OperGEType:
                if (is_double) ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmpge_double);
                else if (is_float) ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmpge_float);
                else if (is_b128) ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmpge_128b);
                else if (is_b64) ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmpge_64b);
                else ret.chain[i++] = reinterpret_cast<UnpackFunc>(cmpge_32b);
                break;
        }
        return ret;
    }


    static void storeConstant_R(mapping::storage_types dt, uint32_t len , const __uint128_t &v) {
        if (dt == mapping::storage_type_float16)  unpack_constant_float16_R(nullptr,&v, nullptr,nullptr);
        else if (dt == mapping::storage_type_float)  unpack_constant_float_R(nullptr,&v, nullptr,nullptr);
        else if (dt == mapping::storage_type_double)  unpack_constant_double_R(nullptr,&v, nullptr,nullptr);
        else if (len < 32) unpack_constant32_R(nullptr,&v, nullptr,nullptr);
        else if (len < 64) unpack_constant64_R(nullptr,&v, nullptr,nullptr);
        else unpack_constant128_R(nullptr,&v, nullptr,nullptr);
    }

    static void storeConstant_L(mapping::storage_types dt, uint32_t len , const __uint128_t &v) {
        if (dt == mapping::storage_type_float16)  unpack_constant_float16_L(&v,nullptr, nullptr,nullptr);
        else if (dt == mapping::storage_type_float)  unpack_constant_float_L(&v,nullptr, nullptr,nullptr);
        else if (dt == mapping::storage_type_double)  unpack_constant_double_L(&v,nullptr, nullptr,nullptr);
        else if (len < 32) unpack_constant32_L(&v,nullptr, nullptr,nullptr);
        else if (len < 64) unpack_constant64_L(&v,nullptr, nullptr,nullptr);
        else unpack_constant128_L(&v,nullptr,nullptr,nullptr);
    }

    static UnpackFunc getStoreConstant_L(mapping::storage_types dt, uint32_t len) {
        if (dt == mapping::storage_type_float16)  return unpack_constant_float16_L;
        else if (dt == mapping::storage_type_float)  return unpack_constant_float_L;
        else if (dt == mapping::storage_type_double)  return unpack_constant_double_L;
        else if (len < 32) return unpack_constant32_L;
        else if (len < 64) return unpack_constant64_L;
        else return unpack_constant128_L;
    }
#endif
};

}