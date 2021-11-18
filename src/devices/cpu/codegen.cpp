#include "codegen.hpp"
#include <sys/mman.h>
#include <cstring>
#include "../../common/log.hpp"
#include "../../common/tools.hpp"

using namespace std;

typedef uint16_t (*Func)(const void *data,void *banks);

namespace devices {

Codegen::Codegen() {
    mem = (uint8_t *) mmap(nullptr, MAX_CODE_SIZE + MAX_DATA_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (mem == MAP_FAILED) {
        Log::panic("Cannot allocate memory for kernels.");
    }
    cursor = 0;
    sourceIndex = 0;
    constantIndex = 0;
    nbInstructions = 0;

    constexpr int INSTR_ENDBR64[] = {0xf3, 0x0f, 0x1e, 0xfa};
    add(INSTR_ENDBR64, size(INSTR_ENDBR64));
}

Codegen::~Codegen() {
    munmap(mem,MAX_CODE_SIZE + MAX_DATA_SIZE);
}


void Codegen::flush() {
    ret();
    if (mprotect(mem, MAX_CODE_SIZE, PROT_READ | PROT_EXEC) == -1) {
        Log::panic("Fail to turn generated kernel code to executable");
    }
}

void Codegen::add(const int *arr, int size) {
    if (cursor+16 < MAX_CODE_SIZE ) {
        for (int i = 0; i < size; i++) {
            mem[cursor++] = (uint8_t) arr[i];
        }
        nbInstructions++;
    } else {
        Log::error("Codegen instruction buffer full!");
    }
}

void Codegen::ret() {
    int INSTR[] = {0xc3};
    add(INSTR, size(INSTR));
    add(INSTR, size(INSTR));
}

uint16_t Codegen::exec(void *data, void *banks) const{
    Func f = (Func) mem;
    return f(data, banks);
}

void Codegen::illegalInstruction() {
    int INSTR[] = {0x62, 0xf1, 0x7d, 0x48, 0xef, 0xc0};
    add(INSTR, size(INSTR));
}

void Codegen::loadPointer() {
    int INSTR0[] = {0x48, 0x8b, 0x4f, 8 * sourceIndex};    // mov    Xx8(%rdi),%rcx
    add(INSTR0, size(INSTR0));
}


void Codegen::addMemoryOffset(uint64_t offset) {
    mem[cursor++] = (uint8_t) offset;
    mem[cursor++] = (uint8_t) (offset >> 8);
    mem[cursor++] = (uint8_t) (offset >> 16);
    mem[cursor++] = (uint8_t) (offset >> 24);
}

void Codegen::addMemoryOffsetL(uint64_t offset) {
    mem[cursor++] = (uint8_t) offset;
    mem[cursor++] = (uint8_t) (offset >> 8);
    mem[cursor++] = (uint8_t) (offset >> 16);
    mem[cursor++] = (uint8_t) (offset >> 24);
    mem[cursor++] = (uint8_t) (offset >> 32);
    mem[cursor++] = (uint8_t) (offset >> 40);
    mem[cursor++] = (uint8_t) (offset >> 48);
    mem[cursor++] = (uint8_t) (offset >> 56);
}

void Codegen::addConstant(const __m256i &cst, bool doCopy) {
    if (doCopy) {
        if (constantIndex*32 < MAX_DATA_SIZE) {
            constantIndex++;
            memcpy(mem + MAX_CODE_SIZE + constantIndex * 32, &cst, 32);
        } else {
            Log::error("Codegen constant max size reached.");
        }
    }
    addMemoryOffset(MAX_CODE_SIZE + constantIndex * 32 - (cursor + 4));
}

void Codegen::addSignedShift32(int s) {
    int INSTR5[] = {0xc5, 0xfd, 0x72, 0xf0, s,      //      	vpslld $s,%ymm0,%ymm0
                    0xc5, 0xf5, 0x72, 0xf1, s,      //      	vpslld $S,%ymm1,%ymm1
                    0xc5, 0xfd, 0x72, 0xe0, s,      //      	vpsrad $s,%ymm0,%ymm0
                    0xc5, 0xf5, 0x72, 0xe1, s};     //      	vpsrad s,%ymm1,%ymm1
    add(INSTR5, size(INSTR5));
}

void Codegen::addSignedShift64(int s) {
    static __m256i signed33 = _mm256_set_epi32(31, 0, 31, 0, 31, 0, 31, 0);
    static __m256i signed34 = _mm256_set_epi32(30, 0, 30, 0, 30, 0, 30, 0);
    static __m256i signed35 = _mm256_set_epi32(29, 0, 29, 0, 29, 0, 29, 0);
    static __m256i signed36 = _mm256_set_epi32(28, 0, 28, 0, 28, 0, 28, 0);
    static __m256i signed37 = _mm256_set_epi32(27, 0, 27, 0, 27, 0, 27, 0);
    static __m256i signed38 = _mm256_set_epi32(26, 0, 26, 0, 26, 0, 26, 0);
    static __m256i signed39 = _mm256_set_epi32(25, 0, 25, 0, 25, 0, 25, 0);
    static __m256i signed40 = _mm256_set_epi32(24, 0, 24, 0, 24, 0, 24, 0);
    static __m256i signed41 = _mm256_set_epi32(23, 0, 23, 0, 23, 0, 23, 0);
    static __m256i signed42 = _mm256_set_epi32(22, 0, 22, 0, 22, 0, 22, 0);
    static __m256i signed43 = _mm256_set_epi32(21, 0, 21, 0, 21, 0, 21, 0);
    static __m256i signed44 = _mm256_set_epi32(20, 0, 20, 0, 20, 0, 20, 0);
    static __m256i signed45 = _mm256_set_epi32(19, 0, 19, 0, 19, 0, 19, 0);
    static __m256i signed46 = _mm256_set_epi32(18, 0, 18, 0, 18, 0, 18, 0);
    static __m256i signed47 = _mm256_set_epi32(17, 0, 17, 0, 17, 0, 17, 0);
    static __m256i signed48 = _mm256_set_epi32(16, 0, 16, 0, 16, 0, 16, 0);
    static __m256i signed49 = _mm256_set_epi32(15, 0, 15, 0, 15, 0, 15, 0);
    static __m256i signed50 = _mm256_set_epi32(14, 0, 14, 0, 14, 0, 14, 0);
    static __m256i signed51 = _mm256_set_epi32(13, 0, 13, 0, 13, 0, 13, 0);
    static __m256i signed52 = _mm256_set_epi32(12, 0, 12, 0, 12, 0, 12, 0);
    static __m256i signed53 = _mm256_set_epi32(11, 0, 11, 0, 11, 0, 11, 0);
    static __m256i signed54 = _mm256_set_epi32(10, 0, 10, 0, 10, 0, 10, 0);
    static __m256i signed55 = _mm256_set_epi32(9, 0, 9, 0, 9, 0, 9, 0);
    static __m256i signed56 = _mm256_set_epi32(8, 0, 8, 0, 8, 0, 8, 0);
    static __m256i signed57 = _mm256_set_epi32(7, 0, 7, 0, 7, 0, 7, 0);
    static __m256i signed58 = _mm256_set_epi32(6, 0, 6, 0, 6, 0, 6, 0);
    static __m256i signed59 = _mm256_set_epi32(5, 0, 5, 0, 5, 0, 5, 0);
    static __m256i signed60 = _mm256_set_epi32(4, 0, 4, 0, 4, 0, 4, 0);
    static __m256i signed61 = _mm256_set_epi32(3, 0, 3, 0, 3, 0, 3, 0);
    static __m256i signed62 = _mm256_set_epi32(2, 0, 2, 0, 2, 0, 2, 0);
    static __m256i signed63 = _mm256_set_epi32(1, 0, 1, 0, 1, 0, 1, 0);

    __m256i a;
    switch (s) {
        case 33:
            a = signed33;
            break;
        case 34:
            a = signed34;
            break;
        case 35:
            a = signed35;
            break;
        case 36:
            a = signed36;
            break;
        case 37:
            a = signed37;
            break;
        case 38:
            a = signed38;
            break;
        case 39:
            a = signed39;
            break;
        case 40:
            a = signed40;
            break;
        case 41:
            a = signed41;
            break;
        case 42:
            a = signed42;
            break;
        case 43:
            a = signed43;
            break;
        case 44:
            a = signed44;
            break;
        case 45:
            a = signed45;
            break;
        case 46:
            a = signed46;
            break;
        case 47:
            a = signed47;
            break;
        case 48:
            a = signed48;
            break;
        case 49:
            a = signed49;
            break;
        case 50:
            a = signed50;
            break;
        case 51:
            a = signed51;
            break;
        case 52:
            a = signed52;
            break;
        case 53:
            a = signed53;
            break;
        case 54:
            a = signed54;
            break;
        case 55:
            a = signed55;
            break;
        case 56:
            a = signed56;
            break;
        case 57:
            a = signed57;
            break;
        case 58:
            a = signed58;
            break;
        case 59:
            a = signed59;
            break;
        case 60:
            a = signed60;
            break;
        case 61:
            a = signed61;
            break;
        case 62:
            a = signed62;
            break;
        case 63:
            a = signed63;
            break;
        default:
            a = _mm256_set1_epi32(0);
            break;
    }
    int INSTR0[] = {0xc5, 0xfe, 0x6f, 0x25};
    add(INSTR0, size(INSTR0));
    addConstant(a, true);
    int INSTR1[] = {0xc4, 0xe2, 0x7d, 0x47, 0xc4};//       	vpsllvd %ymm4,%ymm0,%ymm0
    int INSTR2[] = {0xc4, 0xe2, 0x75, 0x47, 0xcc};//       	vpsllvd %ymm4,%ymm1,%ymm1
    int INSTR3[] = {0xc4, 0xe2, 0x6d, 0x47, 0xd4};//       	vpsllvd %ymm4,%ymm2,%ymm2
    int INSTR4[] = {0xc4, 0xe2, 0x65, 0x47, 0xdc};//       	vpsllvd %ymm4,%ymm3,%ymm3
    int INSTR5[] = {0xc4, 0xe2, 0x7d, 0x46, 0xc4};//       	vpsravd %ymm4,%ymm0,%ymm0
    int INSTR6[] = {0xc4, 0xe2, 0x75, 0x46, 0xcc};//       	vpsravd %ymm4,%ymm1,%ymm1
    int INSTR7[] = {0xc4, 0xe2, 0x6d, 0x46, 0xd4};//       	vpsravd %ymm4,%ymm2,%ymm2
    int INSTR8[] = {0xc4, 0xe2, 0x65, 0x46, 0xdc};//       	vpsravd %ymm4,%ymm3,%ymm3
    add(INSTR1, size(INSTR1));
    add(INSTR2, size(INSTR2));
    add(INSTR3, size(INSTR3));
    add(INSTR4, size(INSTR4));
    add(INSTR5, size(INSTR5));
    add(INSTR6, size(INSTR6));
    add(INSTR7, size(INSTR7));
    add(INSTR8, size(INSTR8));
}

void Codegen::unpack1(bool isSigned) {
    // typedef struct
    //    uint16_t isNull;
    //    uint16_t isendOfDocument;
    //    uint16_t value;
    // SM1;
    static __m256i shift1a = _mm256_set_epi32(7, 6, 5, 4, 3, 2, 1, 0);
    static __m256i shift1b = _mm256_set_epi32(15, 14, 13, 12, 11, 10, 9, 8);
    static __m256i pand1 = _mm256_set1_epi32(1);
    loadPointer();
    int INSTR0[] = {0xc4, 0x62, 0x7d, 0x58, 0x51, 0x04};  // vpbroadcastd 0x4(%rcx),%ymm10
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc4, 0xe2, 0x2d, 0x45, 0x05};
    add(INSTR1, size(INSTR1));
    addConstant(shift1a, true);
    int INSTR2[] = {0xc4, 0xe2, 0x2d, 0x45, 0x0d};
    add(INSTR2, size(INSTR2));
    addConstant(shift1b, true);
    int INSTR3[] = {0xc5, 0xfd, 0xdb, 0x05};
    add(INSTR3, size(INSTR3));
    addConstant(pand1, true);
    int INSTR4[] = {0xc5, 0xf5, 0xdb, 0x0d};
    add(INSTR4, size(INSTR4));
    addConstant(pand1, false);
    if (isSigned) {
        addSignedShift32(31);
    }
}

void Codegen::unpack2(bool isSigned) {
    //typedef struct
    //    uint16_t isNull;
    //    uint16_t isendOfDocument;
    //    uint32_t value;
    // SM2;
    static __m256i shift2a = _mm256_set_epi32(14, 12, 10, 8, 6, 4, 2, 0);
    static __m256i shift2b = _mm256_set_epi32(30, 28, 26, 24, 22, 20, 18, 16);
    static __m256i pand2 = _mm256_set1_epi32(3);
    loadPointer();
    int INSTR0[] = {0xc4, 0x62, 0x7d, 0x58, 0x51, 0x04};  // vpbroadcastd 0x4(%rcx),%ymm10
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc4, 0xe2, 0x2d, 0x45, 0x05};
    add(INSTR1, size(INSTR1));
    addConstant(shift2a, true);
    int INSTR2[] = {0xc4, 0xe2, 0x2d, 0x45, 0x0d};
    add(INSTR2, size(INSTR2));
    addConstant(shift2b, true);
    int INSTR3[] = {0xc5, 0xfd, 0xdb, 0x05};
    add(INSTR3, size(INSTR3));
    addConstant(pand2, true);
    int INSTR4[] = {0xc5, 0xf5, 0xdb, 0x0d};
    add(INSTR4, size(INSTR4));
    addConstant(pand2, false);
    if (isSigned) {
        addSignedShift32(30);
    }
}

void Codegen::unpack3(bool isSigned) {
    //typedef struct
    //    uint16_t isNull;
    //    uint16_t isendOfDocument;
    //    uint8_t value4[8];
    // SM3;
    static __m256i shift4a = _mm256_set_epi32(7 * 4, 6 * 4, 5 * 4, 4 * 4, 3 * 4, 2 * 4, 1 * 4, 0 * 4);
    static __m256i pand3 = _mm256_set1_epi32(7);
    loadPointer();
    int INSTR0[] = {0xc4, 0x62, 0x7d, 0x58, 0x51, 0x04};  // vpbroadcastd 0x4(%rcx),%ymm10
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc4, 0x62, 0x7d, 0x58, 0x59, 0x08};  // vpbroadcastd 0x4(%rcx),%ymm11
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc4, 0xe2, 0x2d, 0x45, 0x05}; // vpsrlvd  shift4a(%rip),Out0,Out0
    add(INSTR2, size(INSTR2));
    addConstant(shift4a, true);
    int INSTR3[] = {0xc4, 0xe2, 0x25, 0x45, 0x0d};
    add(INSTR3, size(INSTR3));
    addConstant(shift4a, false);
    int INSTR4[] = {0xc5, 0xfd, 0xdb, 0x05};
    add(INSTR4, size(INSTR4));
    addConstant(pand3, true);
    int INSTR5[] = {0xc5, 0xf5, 0xdb, 0x0d};
    add(INSTR5, size(INSTR5));
    addConstant(pand3, false);
    if (isSigned) {
        addSignedShift32(29);
    }
}

void Codegen::unpack4(bool isSigned) {
    //typedef struct
    //    uint16_t isNull;
    //    uint16_t isendOfDocument;
    //    uint8_t value4[8];
    // SM3;
    static __m256i shift4a = _mm256_set_epi32(7 * 4, 6 * 4, 5 * 4, 4 * 4, 3 * 4, 2 * 4, 1 * 4, 0 * 4);
    static __m256i pand4 = _mm256_set1_epi32(0xf);
    loadPointer();
    int INSTR0[] = {0xc4, 0x62, 0x7d, 0x58, 0x51, 0x04};  // vpbroadcastd 0x4(%rcx),%ymm10
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc4, 0x62, 0x7d, 0x58, 0x59, 0x08};  // vpbroadcastd 0x4(%rcx),%ymm11
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc4, 0xe2, 0x2d, 0x45, 0x05}; // vpsrlvd  shift4a(%rip),Out0,Out0
    add(INSTR2, size(INSTR2));
    addConstant(shift4a, true);
    int INSTR3[] = {0xc4, 0xe2, 0x25, 0x45, 0x0d};
    add(INSTR3, size(INSTR3));
    addConstant(shift4a, false);
    int INSTR4[] = {0xc5, 0xfd, 0xdb, 0x05};
    add(INSTR4, size(INSTR4));
    addConstant(pand4, true);
    int INSTR5[] = {0xc5, 0xf5, 0xdb, 0x0d};
    add(INSTR5, size(INSTR5));
    addConstant(pand4, false);
    if (isSigned) {
        addSignedShift32(28);
    }
}

void Codegen::unpackGen5(bool isSigned, int p, const __m256i &mask) {
    loadPointer();
    int INSTR0[] = {0xc4, 0x62, 0x7d, 0x5a, 0x51, 4};  // vbroadcasti128 0x4(%rcx),%ymm10
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc4, 0x62, 0x7d, 0x5a, 0x59, 12};  // vbroadcasti128 12(%rcx),%ymm11
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc4, 0xc2, 0x7d, 0x31, 0xc2};  // vpmovzxbd %xmm10,%ymm8
    add(INSTR2, size(INSTR2));
    int INSTR3[] = {0xc4, 0xc2, 0x7d, 0x31, 0xcb};  // vpmovzxbd %xmm11,%ymm9
    add(INSTR3, size(INSTR3));
    if (isSigned) {
        addSignedShift32(32 - p);
    } else {
        int INSTR4[] = {0xc5, 0xfd, 0xdb, 0x05};
        add(INSTR4, size(INSTR4));
        addConstant(mask, true);
        int INSTR5[] = {0xc5, 0xf5, 0xdb, 0x0d};
        add(INSTR5, size(INSTR5));
        addConstant(mask, false);
    }
}

void Codegen::unpack5(bool isSigned) {
    static __m256i pand5 = _mm256_set1_epi32(0x1f);
    unpackGen5(isSigned, 5, pand5);
}

void Codegen::unpack6(bool isSigned) {
    static __m256i pand6 = _mm256_set1_epi32(0x3f);
    unpackGen5(isSigned, 6, pand6);
}

void Codegen::unpack7(bool isSigned) {
    static __m256i pand7 = _mm256_set1_epi32(0x7f);
    unpackGen5(isSigned, 7, pand7);
}

void Codegen::unpack8(bool isSigned) {
    static __m256i pand8 = _mm256_set1_epi32(0xff);
    unpackGen5(isSigned, 8, pand8);
}

void Codegen::unpackGen9(bool isSigned, int p) {
    // typedef struct
    //    uint16_t isNull;
    //    uint16_t isEndOfDocument;
    //    uint8_t value4[8];
    //    uint8_t value8[16];
    // SM9;
    static __m256i grouping4 = _mm256_setr_epi8(4, 5, 5, 6, 7, 8, 8, 9, 10, 11, 11, 12, 13, 14, 14, 15, 0, 1, 1, 2, 3, 4, 4, 5, 6, 7, 7, 8, 9, 10, 10, 11);
    static __m256i pand9 = _mm256_set1_epi32(0xfff);
    loadPointer();
    int INSTR0[] = {0xc5, 0xfe, 0x6f, 0x01};  // vmovdqu (%rcx),%ymm0
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc5, 0xfe, 0x6f, 0x51, 12};  // vmovdqu 12(%rcx),%ymm2
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc4, 0xe2, 0x7d, 0x00, 0x05};  // vpshufb  grouping4(%rip),Out0,Out0
    add(INSTR2, size(INSTR2));
    addConstant(grouping4, true);
    int INSTR3[] = {0xc4, 0xe2, 0x6d, 0x00, 0x15};  // vpshufb  grouping4(%rip),Out2,Out2
    add(INSTR3, size(INSTR3));
    addConstant(grouping4, false);
    int INSTR4[] = {0xc5, 0xf5, 0x71, 0xd0, 0x04};  // vpsrlw            $4,Out0,Out1
    add(INSTR4, size(INSTR4));
    int INSTR5[] = {0xc5, 0xe5, 0x71, 0xd2, 0x04};  // vpsrlw            $4,Out0,Out1
    add(INSTR5, size(INSTR5));
    int INSTR6[] = {0xc5, 0xfd, 0xdb, 0x05};   // vpand             pand9(%rip),Out0,Out0
    add(INSTR6, size(INSTR6));
    addConstant(pand9, true);
    int INSTR7[] = {0xc5, 0xed, 0xdb, 0x15};
    add(INSTR7, size(INSTR7));
    addConstant(pand9, false);
    int INSTR8[] = {0xc4, 0xe3, 0x7d, 0x0e, 0xc1, 0xaa};   // vpblendw          $0b10101010,Out1,Out0,Out0
    add(INSTR8, size(INSTR8));
    int INSTR9[] = {0xc4, 0xe3, 0x6d, 0x0e, 0xd3, 0xaa};   // vpblendw          $0b10101010,Out1,Out0,Out0
    add(INSTR9, size(INSTR9));
    int INSTR10[] = {0xc4, 0xe2, 0x7d, 0x33, 0xc0}; // vpmovzxwd         Outm0,Out0
    add(INSTR10, size(INSTR10));
    int INSTR11[] = {0xc4, 0xe2, 0x7d, 0x33, 0xca}; // vpmovzxwd         Outm2,Out1
    add(INSTR11, size(INSTR11));
    if (isSigned) {
        addSignedShift32(32 - p);
    }
}

void Codegen::unpack9(bool isSigned) {
    unpackGen9(isSigned, 9);
}

void Codegen::unpack10(bool isSigned) {
    unpackGen9(isSigned, 10);
}

void Codegen::unpack11(bool isSigned) {
    unpackGen9(isSigned, 11);
}

void Codegen::unpack12(bool isSigned) {
    unpackGen9(isSigned, 12);
}

void Codegen::unpackGen13(bool isSigned, int p) {
    //typedef struct
    //    uint16_t isNull;
    //    uint16_t isEndOfDocument;
    //    uint16_t value16[16];
    // SM13;
    loadPointer();
    int INSTR0[] = {0xc5, 0xfe, 0x6f, 0x41, 0x04};   // vmovdqu           4(PTR_DATA),Out0
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc5, 0xfe, 0x6f, 0x49, 0x14};   // vmovdqu           20(PTR_DATA),Out2
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc4, 0xe2, 0x7d, 0x33, 0xc0}; // vpmovzxwd         Outm0,Out0
    add(INSTR2, size(INSTR2));
    int INSTR3[] = {0xc4, 0xe2, 0x7d, 0x33, 0xc9}; // vpmovzxwd         Outm1,Out1
    add(INSTR3, size(INSTR3));
    if (isSigned) {
        addSignedShift32(32 - p);
    }
}

void Codegen::unpack13(bool isSigned) {
    unpackGen13(isSigned, 13);
}

void Codegen::unpack14(bool isSigned) {
    unpackGen13(isSigned, 14);
}

void Codegen::unpack15(bool isSigned) {
    unpackGen13(isSigned, 15);
}

void Codegen::unpack16(bool isSigned) {
    unpackGen13(isSigned, 16);
}

void Codegen::unpackGen17(bool isSigned, int p) {
    //typedef struct
    //    uint16_t isNull;
    //    uint16_t isEndOfDocument;
    //    BitArray24 values;
    // SM17;
    static __m256i grouping8 = _mm256_set_epi8((char)0x80, 11, 10, 9, (char)0x80, 8, 7, 6, (char)0x80, 5, 4, 3, (char)0x80, 2, 1, 0, (char)0x80, 15, 14, 13, (char)0x80, 12, 11, 10, (char)0x80, 9, 8, 7, (char)0x80, 6, 5, 4);

    loadPointer();
    int INSTR0[] = {0xc5, 0xfe, 0x6f, 0x41, 0};   // vmovdqu           0(PTR_DATA),Out0
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc5, 0xfe, 0x6f, 0x49, 24};   // vmovdqu           24(PTR_DATA),Out2
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc4, 0xe2, 0x7d, 0x00, 0x05};  // vpshufb  grouping4(%rip),Out0,Out0
    add(INSTR2, size(INSTR2));
    addConstant(grouping8, true);
    int INSTR3[] = {0xc4, 0xe2, 0x75, 0x00, 0x0d};  // vpshufb  grouping4(%rip),Out1,Out1
    add(INSTR3, size(INSTR3));
    addConstant(grouping8, false);
    if (isSigned && p < 32) {
        addSignedShift32(32 - p);
    }
}

void Codegen::unpack17(bool isSigned) {
    unpackGen17(isSigned, 17);
}

void Codegen::unpack18(bool isSigned) {
    unpackGen17(isSigned, 18);
}

void Codegen::unpack19(bool isSigned) {
    unpackGen17(isSigned, 19);
}

void Codegen::unpack20(bool isSigned) {
    unpackGen17(isSigned, 20);
}

void Codegen::unpack21(bool isSigned) {
    unpackGen17(isSigned, 21);
}

void Codegen::unpack22(bool isSigned) {
    unpackGen17(isSigned, 22);
}

void Codegen::unpack23(bool isSigned) {
    unpackGen17(isSigned, 23);
}

void Codegen::unpack24(bool isSigned) {
    unpackGen17(isSigned, 24);
}

void Codegen::unpackGen25(bool isSigned, int p) {
    //typedef struct
    //    uint16_t isNull;
    //    uint16_t isEndOfDocument;
    //    uint32_t value32[16];
    // SM25;
    loadPointer();
    int INSTR0[] = {0xc5, 0xfe, 0x6f, 0x41, 4};   // vmovdqu           4(PTR_DATA),Out0
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc5, 0xfe, 0x6f, 0x49, 36};   // vmovdqu           36(PTR_DATA),Out2
    add(INSTR1, size(INSTR1));
    if (isSigned && p < 32) {
        addSignedShift32(32 - p);
    }
}

void Codegen::unpack25(bool isSigned) {
    unpackGen25(isSigned, 25);
}

void Codegen::unpack26(bool isSigned) {
    unpackGen25(isSigned, 26);
}

void Codegen::unpack27(bool isSigned) {
    unpackGen25(isSigned, 27);
}

void Codegen::unpack28(bool isSigned) {
    unpackGen25(isSigned, 28);
}

void Codegen::unpack29(bool isSigned) {
    unpackGen25(isSigned, 29);
}

void Codegen::unpack30(bool isSigned) {
    unpackGen25(isSigned, 30);
}

void Codegen::unpack31(bool isSigned) {
    unpackGen25(isSigned, 31);
}

void Codegen::unpack32(bool isSigned) {
    unpackGen25(isSigned, 32);
}

void Codegen::unpackGen33(bool isSigned, int p) {
    //typedef struct
    //    uint16_t isNull;
    //    uint16_t isEndOfDocument;
    //    BitArray40 values;
    // SM33;
    static __m256i grouping40 = _mm256_set_epi8((char)0x80, (char)0x80, (char)0x80, 9, 8, 7, 6, 5, (char)0x80, (char)0x80, (char)0x80, 4, 3, 2, 1, 0, (char)0x80, (char)0x80, (char)0x80, 15, 14, 13, 12, 11, (char)0x80, (char)0x80, (char)0x80, 10, 9,
                                                8, 7, 6);
    loadPointer();
    int INSTR0[] = {0xc5, 0xfe, 0x6f, 0x41, -2};   // vmovdqu           -2(PTR_DATA),Out0
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc5, 0xfe, 0x6f, 0x49, 18};   // vmovdqu           18(PTR_DATA),Out1
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc5, 0xfe, 0x6f, 0x51, 38};   // vmovdqu            38(PTR_DATA),Out2
    add(INSTR2, size(INSTR2));
    int INSTR3[] = {0xc5, 0xfe, 0x6f, 0x59, 58};   // vmovdqu           58(PTR_DATA),Out3
    add(INSTR3, size(INSTR3));
    int INSTR4[] = {0xc4, 0xe2, 0x7d, 0x00, 0x05};  // vpshufb  grouping40(%rip),Out0,Out0
    add(INSTR4, size(INSTR4));
    addConstant(grouping40, true);
    int INSTR5[] = {0xc4, 0xe2, 0x75, 0x00, 0x0d};  // vpshufb  grouping40(%rip),Out1,Out1
    add(INSTR5, size(INSTR5));
    addConstant(grouping40, false);
    int INSTR6[] = {0xc4, 0xe2, 0x6d, 0x00, 0x15};  // vpshufb  grouping40(%rip),Out2,Out2
    add(INSTR6, size(INSTR6));
    addConstant(grouping40, false);
    int INSTR7[] = {0xc4, 0xe2, 0x65, 0x00, 0x1d};  // vpshufb  grouping40(%rip),Out3,Out3
    add(INSTR7, size(INSTR7));
    addConstant(grouping40, false);
    if (isSigned && p < 64) {
        addSignedShift64(p);
    }

}

void Codegen::unpack33(bool isSigned) {
    unpackGen33(isSigned, 33);
}

void Codegen::unpack34(bool isSigned) {
    unpackGen33(isSigned, 34);
}

void Codegen::unpack35(bool isSigned) {
    unpackGen33(isSigned, 35);
}

void Codegen::unpack36(bool isSigned) {
    unpackGen33(isSigned, 36);
}

void Codegen::unpack37(bool isSigned) {
    unpackGen33(isSigned, 37);
}

void Codegen::unpack38(bool isSigned) {
    unpackGen33(isSigned, 38);
}

void Codegen::unpack39(bool isSigned) {
    unpackGen33(isSigned, 39);
}

void Codegen::unpack40(bool isSigned) {
    unpackGen33(isSigned, 40);
}

void Codegen::unpackGen41(bool isSigned, int p) {
    //typedef struct
    //    uint16_t isNull;
    //    uint16_t isEndOfDocument;
    //    BitArray48 values;
    // SM41;
    static __m256i grouping48 = _mm256_set_epi8((char)0x80, (char)0x80, 11, 10, 9, 8, 7, 6, (char)0x80, (char)0x80, 5, 4, 3, 2, 1, 0, (char)0x80, (char)0x80, 15, 14, 13, 12, 11, 10, (char)0x80, (char)0x80, 9, 8, 7, 6, 5, 4);
    loadPointer();
    int INSTR0[] = {0xc5, 0xfe, 0x6f, 0x41, 0};   // vmovdqu           0(PTR_DATA),Out0
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc5, 0xfe, 0x6f, 0x49, 24};   // vmovdqu           24(PTR_DATA),Out1
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc5, 0xfe, 0x6f, 0x51, 48};   // vmovdqu            48(PTR_DATA),Out2
    add(INSTR2, size(INSTR2));
    int INSTR3[] = {0xc5, 0xfe, 0x6f, 0x59, 72};   // vmovdqu           72(PTR_DATA),Out3
    add(INSTR3, size(INSTR3));
    int INSTR4[] = {0xc4, 0xe2, 0x7d, 0x00, 0x05};  // vpshufb  grouping48(%rip),Out0,Out0
    add(INSTR4, size(INSTR4));
    addConstant(grouping48, true);
    int INSTR5[] = {0xc4, 0xe2, 0x75, 0x00, 0x0d};  // vpshufb  grouping40(%rip),Out1,Out1
    add(INSTR5, size(INSTR5));
    addConstant(grouping48, false);
    int INSTR6[] = {0xc4, 0xe2, 0x6d, 0x00, 0x15};  // vpshufb  grouping48(%rip),Out2,Out2
    add(INSTR6, size(INSTR6));
    addConstant(grouping48, false);
    int INSTR7[] = {0xc4, 0xe2, 0x65, 0x00, 0x1d};  // vpshufb  grouping48(%rip),Out3,Out3
    add(INSTR7, size(INSTR7));
    addConstant(grouping48, false);
    if (isSigned && p < 64) {
        addSignedShift64(p);
    }
}

void Codegen::unpack41(bool isSigned) {
    unpackGen41(isSigned, 41);
}

void Codegen::unpack42(bool isSigned) {
    unpackGen41(isSigned, 42);
}

void Codegen::unpack43(bool isSigned) {
    unpackGen41(isSigned, 43);
}

void Codegen::unpack44(bool isSigned) {
    unpackGen41(isSigned, 44);
}

void Codegen::unpack45(bool isSigned) {
    unpackGen41(isSigned, 45);
}

void Codegen::unpack46(bool isSigned) {
    unpackGen41(isSigned, 46);
}

void Codegen::unpack47(bool isSigned) {
    unpackGen41(isSigned, 47);
}

void Codegen::unpack48(bool isSigned) {
    unpackGen41(isSigned, 48);
}

void Codegen::unpackGen49(bool isSigned, int p) {
    //typedef struct
    //    uint16_t isNull;
    //    uint16_t isEndOfDocument;
    //    BitArray56 values;
    // SM49;
    static __m256i grouping56 = _mm256_set_epi8((char)0x80, 13, 12, 11, 10, 9, 8, 7, (char)0x80, 6, 5, 4, 3, 2, 1, 0, (char)0x80, 15, 14, 13, 12, 11, 10, 9, (char)0x80, 8, 7, 6, 5, 4, 3, 2);
    loadPointer();
    int INSTR0[] = {0xc5, 0xfe , 0x6f, 0x41, 2};   // vmovdqu           2(PTR_DATA),Out0
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc5, 0xfe , 0x6f, 0x49, 30};   // vmovdqu           30(PTR_DATA),Out1
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc5, 0xfe , 0x6f, 0x51, 58};   // vmovdqu            58(PTR_DATA),Out2
    add(INSTR2, size(INSTR2));
    int INSTR3[] = {0xc5, 0xfe , 0x6f, 0x59, 86};   // vmovdqu           86(PTR_DATA),Out3
    add(INSTR3, size(INSTR3));
    int INSTR4[] = {0xc4, 0xe2 , 0x7d , 0x00, 0x05};  // vpshufb  grouping56(%rip),Out0,Out0
    add(INSTR4, size(INSTR4));
    addConstant(grouping56, true);
    int INSTR5[] = {0xc4, 0xe2 , 0x75, 0x00, 0x0d};  // vpshufb  grouping56(%rip),Out1,Out1
    add(INSTR5, size(INSTR5));
    addConstant(grouping56, false);
    int INSTR6[] = {0xc4, 0xe2 , 0x6d, 0x00, 0x15};  // vpshufb  grouping56(%rip),Out2,Out2
    add(INSTR6, size(INSTR6));
    addConstant(grouping56, false);
    int INSTR7[] = {0xc4, 0xe2 , 0x65, 0x00, 0x1d};  // vpshufb  grouping56(%rip),Out3,Out3
    add(INSTR7, size(INSTR7));
    addConstant(grouping56, false);
    if (isSigned && p < 64) {
        addSignedShift64(p);
    }
}

void Codegen::unpack49(bool isSigned) {
    unpackGen49(isSigned, 49);
}

void Codegen::unpack50(bool isSigned) {
    unpackGen49(isSigned, 50);
}

void Codegen::unpack51(bool isSigned) {
    unpackGen49(isSigned, 51);
}

void Codegen::unpack52(bool isSigned) {
    unpackGen49(isSigned, 52);
}

void Codegen::unpack53(bool isSigned) {
    unpackGen49(isSigned, 53);
}

void Codegen::unpack54(bool isSigned) {
    unpackGen49(isSigned, 54);
}

void Codegen::unpack55(bool isSigned) {
    unpackGen49(isSigned, 55);
}

void Codegen::unpack56(bool isSigned) {
    unpackGen49(isSigned, 56);
}

void Codegen::unpackGen57(bool isSigned, int p) {
    //typedef struct
    //    uint16_t isNull;
    //    uint16_t isEndOfDocument;
    //    uint64_t values[16];
    // SM57;
    loadPointer();
    int INSTR0[] = {0xc5, 0xfe , 0x6f, 0x41, 4};   // vmovdqu           4(PTR_DATA),Out0
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc5, 0xfe , 0x6f, 0x49, 36};   // vmovdqu           36(PTR_DATA),Out1
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc5, 0xfe , 0x6f, 0x51, 68};   // vmovdqu            68(PTR_DATA),Out2
    add(INSTR2, size(INSTR2));
    int INSTR3[] = {0xc5, 0xfe , 0x6f, 0x59, 100};   // vmovdqu           100(PTR_DATA),Out3
    add(INSTR3, size(INSTR3));
    if (isSigned && p < 64) {
        addSignedShift64(p);
    }
}

void Codegen::unpack57(bool isSigned) {
    unpackGen57(isSigned, 57);
}

void Codegen::unpack58(bool isSigned) {
    unpackGen57(isSigned, 58);
}

void Codegen::unpack59(bool isSigned) {
    unpackGen57(isSigned, 59);
}

void Codegen::unpack60(bool isSigned) {
    unpackGen57(isSigned, 60);
}

void Codegen::unpack61(bool isSigned) {
    unpackGen57(isSigned, 61);
}

void Codegen::unpack62(bool isSigned) {
    unpackGen57(isSigned, 62);
}

void Codegen::unpack63(bool isSigned) {
    unpackGen57(isSigned, 63);
}

void Codegen::unpack64(bool isSigned) {
    unpackGen57(isSigned, 64);
}

void Codegen::unpackGen65(bool isSigned, int p) {
    //typedef struct {
    //    uint16_t isNull;
    //    uint16_t isEndOfDocument;
    //    BitArray65 values;
    // SM65;
    static __m256i shufle65 = _mm256_set_epi8((char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, 8, 7, 6, 5, 4, 3, 2, 1, 0,
                                              (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, 15, 14, 13, 12, 11, 10, 9, 8, 7);
    static __m256i shufle65s = _mm256_set_epi8(8, 8, 8, 8, 8, 8, 8, 8, 7, 6, 5, 4, 3, 2, 1, 0,
                                               15, 15, 15, 15, 15, 15, 15, 15, 14, 13, 12, 11, 10, 9, 8, 7);
    static __m256i signed65 = _mm256_set_epi32(31, 31, 0, 0, 31, 31, 0, 0);
    static __m256i signed65b = _mm256_set_epi32(32, 31, 0, 0, 32, 31, 0, 0);
    static __m256i signed66 = _mm256_set_epi32(30, 30, 0, 0, 30, 30, 0, 0);
    static __m256i signed66b = _mm256_set_epi32(32, 30, 0, 0, 32, 30, 0, 0);
    static __m256i signed67 = _mm256_set_epi32(29, 29, 0, 0, 29, 29, 0, 0);
    static __m256i signed67b = _mm256_set_epi32(32, 29, 0, 0, 32, 29, 0, 0);
    static __m256i signed68 = _mm256_set_epi32(28, 28, 0, 0, 28, 28, 0, 0);
    static __m256i signed68b = _mm256_set_epi32(32, 28, 0, 0, 32, 28, 0, 0);
    static __m256i signed69 = _mm256_set_epi32(27, 27, 0, 0, 27, 27, 0, 0);
    static __m256i signed69b = _mm256_set_epi32(32, 27, 0, 0, 32, 27, 0, 0);
    static __m256i signed70 = _mm256_set_epi32(26, 26, 0, 0, 26, 26, 0, 0);
    static __m256i signed70b = _mm256_set_epi32(32, 26, 0, 0, 32, 26, 0, 0);
    static __m256i signed71 = _mm256_set_epi32(25, 25, 0, 0, 25, 25, 0, 0);
    static __m256i signed71b = _mm256_set_epi32(32, 25, 0, 0, 32, 25, 0, 0);
    static __m256i signed72 = _mm256_set_epi32(24, 24, 0, 0, 24, 24, 0, 0);
    static __m256i signed72b = _mm256_set_epi32(32, 24, 0, 0, 32, 24, 0, 0);

    __m256i s, sb;
    switch (p) {
        case 65:
            s = signed65;
            sb = signed65b;
            break;
        case 66:
            s = signed66;
            sb = signed66b;
            break;
        case 67:
            s = signed67;
            sb = signed67b;
            break;
        case 68:
            s = signed68;
            sb = signed68b;
            break;
        case 69:
            s = signed69;
            sb = signed69b;
            break;
        case 70:
            s = signed70;
            sb = signed70b;
            break;
        case 71:
            s = signed71;
            sb = signed71b;
            break;
        case 72:
            s = signed72;
            sb = signed72b;
            break;
        default:
            break;
    }
    loadPointer();

    int INSTR0[] = {0xc5, 0xff, 0xf0, 0x41, -3};//      	vlddqu -0x3(%rcx),%ymm0
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc5, 0xff, 0xf0, 0x49, 15};//       	vlddqu 0xf(%rcx),%ymm1
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc5, 0xff, 0xf0, 0x51, 33};//       	vlddqu 0x21(%rcx),%ymm2
    add(INSTR2, size(INSTR2));
    int INSTR3[] = {0xc5, 0xff, 0xf0, 0x59, 51};//       	vlddqu 0x33(%rcx),%ymm3
    add(INSTR3, size(INSTR3));
    int INSTR4[] = {0xc5, 0xff, 0xf0, 0x61, 69};//       	vlddqu 0x45(%rcx),%ymm4
    add(INSTR4, size(INSTR0));
    int INSTR5[] = {0xc5, 0xff, 0xf0, 0x69, 87};//       	vlddqu 0x57(%rcx),%ymm5
    add(INSTR5, size(INSTR5));
    int INSTR6[] = {0xc5, 0xff, 0xf0, 0x71, 105};//       	vlddqu 0x69(%rcx),%ymm6
    add(INSTR6, size(INSTR6));
    int INSTR7[] = {0xc5, 0xff, 0xf0, 0x79, 123};//       	vlddqu 0x7b(%rcx),%ymm7
    add(INSTR7, size(INSTR7));
    if (isSigned) {
        int INSTR8[] = {0xc4, 0xe2, 0x7d, 0x00, 0x05};// 	vpshufb 0x0(%rip),%ymm0,%ymm0        # 35 <FUNC+0x35>	31: R_X86_64_PC32	shufle65s-0x4
        add(INSTR8, size(INSTR8));
        addConstant(shufle65s, true);
        int INSTR9[] = {0xc4, 0xe2, 0x75, 0x00, 0x0d};// 	vpshufb 0x0(%rip),%ymm1,%ymm1        # 3e <FUNC+0x3e>	3a: R_X86_64_PC32	shufle65s-0x4
        add(INSTR9, size(INSTR9));
        addConstant(shufle65s, false);
        int INSTR10[] = {0xc4, 0xe2, 0x6d, 0x00, 0x15};// 	vpshufb 0x0(%rip),%ymm2,%ymm2        # 47 <FUNC+0x47>	43: R_X86_64_PC32	shufle65s-0x4
        add(INSTR10, size(INSTR10));
        addConstant(shufle65s, false);
        int INSTR11[] = {0xc4, 0xe2, 0x65, 0x00, 0x1d};// 	vpshufb 0x0(%rip),%ymm3,%ymm3        # 50 <FUNC+0x50>	4c: R_X86_64_PC32	shufle65s-0x4
        add(INSTR11, size(INSTR11));
        addConstant(shufle65s, false);
        int INSTR12[] = {0xc4, 0xe2, 0x5d, 0x00, 0x25};// 	vpshufb 0x0(%rip),%ymm4,%ymm4        # 59 <FUNC+0x59>	55: R_X86_64_PC32	shufle65s-0x4
        add(INSTR12, size(INSTR12));
        addConstant(shufle65s, false);
        int INSTR13[] = {0xc4, 0xe2, 0x55, 0x00, 0x2d};// 	vpshufb 0x0(%rip),%ymm5,%ymm5        # 62 <FUNC+0x62>	5e: R_X86_64_PC32	shufle65s-0x4
        add(INSTR13, size(INSTR13));
        addConstant(shufle65s, false);
        int INSTR14[] = {0xc4, 0xe2, 0x4d, 0x00, 0x35};// 	vpshufb 0x0(%rip),%ymm6,%ymm6        # 6b <FUNC+0x6b>	67: R_X86_64_PC32	shufle65s-0x4
        add(INSTR14, size(INSTR14));
        addConstant(shufle65s, false);
        int INSTR15[] = {0xc4, 0xe2, 0x45, 0x00, 0x3d};// 	vpshufb 0x0(%rip),%ymm7,%ymm7        # 74 <FUNC+0x74>	70: R_X86_64_PC32	shufle65s-0x4
        add(INSTR15, size(INSTR15));
        addConstant(shufle65s, true);
        int INSTR16[] = {0xc4, 0xe2, 0x7d, 0x47, 0x05};// 	vpsllvd 0x0(%rip),%ymm0,%ymm0        # 7d <FUNC+0x7d>	79: R_X86_64_PC32	signed65-0x4
        add(INSTR16, size(INSTR16));
        addConstant(s, true);
        int INSTR17[] = {0xc4, 0xe2, 0x75, 0x47, 0x0d};// 	vpsllvd 0x0(%rip),%ymm1,%ymm1        # 86 <FUNC+0x86>	82: R_X86_64_PC32	signed65-0x4
        add(INSTR17, size(INSTR17));
        addConstant(s, false);
        int INSTR18[] = {0xc4, 0xe2, 0x6d, 0x47, 0x15};// 	vpsllvd 0x0(%rip),%ymm2,%ymm2        # 8f <FUNC+0x8f>	8b: R_X86_64_PC32	signed65-0x4
        add(INSTR18, size(INSTR18));
        addConstant(s, false);
        int INSTR19[] = {0xc4, 0xe2, 0x65, 0x47, 0x1d};// 	vpsllvd 0x0(%rip),%ymm3,%ymm3        # 98 <FUNC+0x98>	94: R_X86_64_PC32	signed65-0x4
        add(INSTR19, size(INSTR19));
        addConstant(s, false);
        int INSTR20[] = {0xc4, 0xe2, 0x5d, 0x47, 0x25};// 	vpsllvd 0x0(%rip),%ymm4,%ymm4        # a1 <FUNC+0xa1>	9d: R_X86_64_PC32	signed65-0x4
        add(INSTR20, size(INSTR20));
        addConstant(s, false);
        int INSTR21[] = {0xc4, 0xe2, 0x55, 0x47, 0x2d};// 	vpsllvd 0x0(%rip),%ymm5,%ymm5        # aa <FUNC+0xaa>	a6: R_X86_64_PC32	signed65-0x4
        add(INSTR21, size(INSTR21));
        addConstant(s, false);
        int INSTR22[] = {0xc4, 0xe2, 0x4d, 0x47, 0x35};// 	vpsllvd 0x0(%rip),%ymm6,%ymm6        # b3 <FUNC+0xb3>	af: R_X86_64_PC32	signed65-0x4
        add(INSTR22, size(INSTR22));
        addConstant(s, false);
        int INSTR23[] = {0xc4, 0xe2, 0x45, 0x47, 0x3d};// 	vpsllvd 0x0(%rip),%ymm7,%ymm7        # bc <FUNC+0xbc>	b8: R_X86_64_PC32	signed65-0x4
        add(INSTR23, size(INSTR23));
        addConstant(s, false);
        int INSTR24[] = {0xc4, 0xe2, 0x7d, 0x46, 0x05};// 	vpsravd 0x0(%rip),%ymm0,%ymm0        # 0xc5 ,0x<FUNC+0xc5>	c1: R_X86_64_PC32	signed65b-0x4
        add(INSTR24, size(INSTR24));
        addConstant(sb, true);
        int INSTR25[] = {0xc4, 0xe2, 0x75, 0x46, 0x0d};// 	vpsravd 0x0(%rip),%ymm1,%ymm1        # ce <FUNC+0xce>	ca: R_X86_64_PC32	signed65b-0x4
        add(INSTR25, size(INSTR25));
        addConstant(sb, false);
        int INSTR26[] = {0xc4, 0xe2, 0x6d, 0x46, 0x15};// 	vpsravd 0x0(%rip),%ymm2,%ymm2        # d7 <FUNC+0xd7>	d3: R_X86_64_PC32	signed65b-0x4
        add(INSTR26, size(INSTR26));
        addConstant(sb, false);
        int INSTR27[] = {0xc4, 0xe2, 0x65, 0x46, 0x1d};// 	vpsravd 0x0(%rip),%ymm3,%ymm3        # e0 <FUNC+0xe0>	dc: R_X86_64_PC32	signed65b-0x4
        add(INSTR27, size(INSTR27));
        addConstant(sb, false);
        int INSTR28[] = {0xc4, 0xe2, 0x5d, 0x46, 0x25};// 	vpsravd 0x0(%rip),%ymm4,%ymm4        # e9 <FUNC+0xe9>	e5: R_X86_64_PC32	signed65b-0x4
        add(INSTR28, size(INSTR28));
        addConstant(sb, false);
        int INSTR29[] = {0xc4, 0xe2, 0x55, 0x46, 0x2d};// 	vpsravd 0x0(%rip),%ymm5,%ymm5        # f2 <FUNC+0xf2>	ee: R_X86_64_PC32	signed65b-0x4
        add(INSTR29, size(INSTR29));
        addConstant(sb, false);
        int INSTR30[] = {0xc4, 0xe2, 0x4d, 0x46, 0x35};// 	vpsravd 0x0(%rip),%ymm6,%ymm6        # fb <FUNC+0xfb>	f7: R_X86_64_PC32	signed65b-0x4
        add(INSTR30, size(INSTR30));
        addConstant(sb, false);
        int INSTR31[] = {0xc4, 0xe2, 0x45, 0x46, 0x3d};// 	vpsravd 0x0(%rip),%ymm7,%ymm7        # 104 <FUNC+0x104>	100: R_X86_64_PC32	signed65b-0x4
        add(INSTR31, size(INSTR31));
        addConstant(sb, false);
    } else {
        int INSTR8[] = {0xc4, 0xe2, 0x7d, 0x00, 0x05};// 	vpshufb 0x0(%rip),%ymm0,%ymm0        # 35 <FUNC+0x35>	31: R_X86_64_PC32	shufle65-0x4
        add(INSTR8, size(INSTR8));
        addConstant(shufle65, true);
        int INSTR9[] = {0xc4, 0xe2, 0x75, 0x00, 0x0d};// 	vpshufb 0x0(%rip),%ymm1,%ymm1        # 3e <FUNC+0x3e>	3a: R_X86_64_PC32	shufle65-0x4
        add(INSTR9, size(INSTR9));
        addConstant(shufle65, false);
        int INSTR10[] = {0xc4, 0xe2, 0x6d, 0x00, 0x15};// 	vpshufb 0x0(%rip),%ymm2,%ymm2        # 47 <FUNC+0x47>	43: R_X86_64_PC32	shufle65-0x4
        add(INSTR10, size(INSTR10));
        addConstant(shufle65, false);
        int INSTR11[] = {0xc4, 0xe2, 0x65, 0x00, 0x1d};// 	vpshufb 0x0(%rip),%ymm3,%ymm3        # 50 <FUNC+0x50>	4c: R_X86_64_PC32	shufle65-0x4
        add(INSTR11, size(INSTR11));
        addConstant(shufle65, false);
        int INSTR12[] = {0xc4, 0xe2, 0x5d, 0x00, 0x25};// 	vpshufb 0x0(%rip),%ymm4,%ymm4        # 59 <FUNC+0x59>	55: R_X86_64_PC32	shufle65-0x4
        add(INSTR12, size(INSTR12));
        addConstant(shufle65, false);
        int INSTR13[] = {0xc4, 0xe2, 0x55, 0x00, 0x2d};// 	vpshufb 0x0(%rip),%ymm5,%ymm5        # 62 <FUNC+0x62>	5e: R_X86_64_PC32	shufle65-0x4
        add(INSTR13, size(INSTR13));
        addConstant(shufle65, false);
        int INSTR14[] = {0xc4, 0xe2, 0x4d, 0x00, 0x35};// 	vpshufb 0x0(%rip),%ymm6,%ymm6        # 6b <FUNC+0x6b>	67: R_X86_64_PC32	shufle65-0x4
        add(INSTR14, size(INSTR14));
        addConstant(shufle65, false);
        int INSTR15[] = {0xc4, 0xe2, 0x45, 0x00, 0x3d};// 	vpshufb 0x0(%rip),%ymm7,%ymm7        # 74 <FUNC+0x74>	70: R_X86_64_PC32	shufle65-0x4
        add(INSTR15, size(INSTR15));
        addConstant(shufle65, false);
    }
}

void Codegen::unpack65(bool isSigned) {
    unpackGen65(isSigned, 65);
}

void Codegen::unpack66(bool isSigned) {
    unpackGen65(isSigned, 66);
}

void Codegen::unpack67(bool isSigned) {
    unpackGen65(isSigned, 67);
}

void Codegen::unpack68(bool isSigned) {
    unpackGen65(isSigned, 68);
}

void Codegen::unpack69(bool isSigned) {
    unpackGen65(isSigned, 69);
}

void Codegen::unpack70(bool isSigned) {
    unpackGen65(isSigned, 70);
}

void Codegen::unpack71(bool isSigned) {
    unpackGen65(isSigned, 71);
}

void Codegen::unpack72(bool isSigned) {
    unpackGen65(isSigned, 72);
}

void Codegen::unpackGen73(bool isSigned, int p) {
    //typedef struct
    //    uint16_t isNull;
    //    uint16_t isEndOfDocument;
    //    BitArray73 values;
    // SM73;
    static __m256i shufle73 = _mm256_set_epi8((char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
                                              (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6);
    static __m256i shufle73s = _mm256_set_epi8(9, 9, 9, 9, 9, 9, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
                                               15, 15, 15, 15, 15, 15, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6);
    static __m256i signed73 = _mm256_set_epi32(23, 23, 0, 0, 23, 23, 0, 0);
    static __m256i signed73b = _mm256_set_epi32(32, 23, 0, 0, 32, 23, 0, 0);
    static __m256i signed74 = _mm256_set_epi32(22, 22, 0, 0, 22, 22, 0, 0);
    static __m256i signed74b = _mm256_set_epi32(32, 22, 0, 0, 32, 22, 0, 0);
    static __m256i signed75 = _mm256_set_epi32(21, 21, 0, 0, 21, 21, 0, 0);
    static __m256i signed75b = _mm256_set_epi32(32, 21, 0, 0, 32, 21, 0, 0);
    static __m256i signed76 = _mm256_set_epi32(20, 20, 0, 0, 20, 20, 0, 0);
    static __m256i signed76b = _mm256_set_epi32(32, 20, 0, 0, 32, 20, 0, 0);
    static __m256i signed77 = _mm256_set_epi32(19, 19, 0, 0, 19, 19, 0, 0);
    static __m256i signed77b = _mm256_set_epi32(32, 19, 0, 0, 32, 19, 0, 0);
    static __m256i signed78 = _mm256_set_epi32(18, 18, 0, 0, 18, 18, 0, 0);
    static __m256i signed78b = _mm256_set_epi32(32, 18, 0, 0, 32, 18, 0, 0);
    static __m256i signed79 = _mm256_set_epi32(17, 17, 0, 0, 17, 17, 0, 0);
    static __m256i signed79b = _mm256_set_epi32(32, 17, 0, 0, 32, 17, 0, 0);
    static __m256i signed80 = _mm256_set_epi32(16, 16, 0, 0, 16, 16, 0, 0);
    static __m256i signed80b = _mm256_set_epi32(32, 16, 0, 0, 32, 16, 0, 0);

    __m256i s, sb;
    switch (p) {
        case 73:
            s = signed73;
            sb = signed73b;
            break;
        case 74:
            s = signed74;
            sb = signed74b;
            break;
        case 75:
            s = signed75;
            sb = signed75b;
            break;
        case 76:
            s = signed76;
            sb = signed76b;
            break;
        case 77:
            s = signed77;
            sb = signed77b;
            break;
        case 78:
            s = signed78;
            sb = signed78b;
            break;
        case 79:
            s = signed79;
            sb = signed79b;
            break;
        case 80:
            s = signed80;
            sb = signed80b;
            break;
        default:
            break;
    }
    loadPointer();

    int INSTR0[] = {0xc5, 0xff, 0xf0, 0x41, -2};//      	vlddqu -2(%rcx),%ymm0
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc5, 0xff, 0xf0, 0x49, 18};//       	vlddqu 18(%rcx),%ymm1
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc5, 0xff, 0xf0, 0x51, 38};//       	vlddqu 38(%rcx),%ymm2
    add(INSTR2, size(INSTR2));
    int INSTR3[] = {0xc5, 0xff, 0xf0, 0x59, 58};//       	vlddqu 58(%rcx),%ymm3
    add(INSTR3, size(INSTR3));
    int INSTR4[] = {0xc5, 0xff, 0xf0, 0x61, 78};//       	vlddqu 78(%rcx),%ymm4
    add(INSTR4, size(INSTR0));
    int INSTR5[] = {0xc5, 0xff, 0xf0, 0x69, 98};//       	vlddqu 98(%rcx),%ymm5
    add(INSTR5, size(INSTR5));
    int INSTR6[] = {0xc5, 0xff, 0xf0, 0x71, 118};//       	vlddqu 118(%rcx),%ymm6
    add(INSTR6, size(INSTR6));
    int INSTR7[] = {0xc5, 0xff, 0xf0, 0xb9, 138, 0,0,0};// 	vlddqu 138(%rcx),%ymm7
    add(INSTR7, size(INSTR7));
    if (isSigned) {
        int INSTR8[] = {0xc4, 0xe2, 0x7d, 0x00, 0x05};// 	vpshufb 0x0(%rip),%ymm0,%ymm0        # 35 <FUNC+0x35>	31: R_X86_64_PC32	shufle65s-0x4
        add(INSTR8, size(INSTR8));
        addConstant(shufle73s, true);
        int INSTR9[] = {0xc4, 0xe2, 0x75, 0x00, 0x0d};// 	vpshufb 0x0(%rip),%ymm1,%ymm1        # 3e <FUNC+0x3e>	3a: R_X86_64_PC32	shufle65s-0x4
        add(INSTR9, size(INSTR9));
        addConstant(shufle73s, false);
        int INSTR10[] = {0xc4, 0xe2, 0x6d, 0x00, 0x15};// 	vpshufb 0x0(%rip),%ymm2,%ymm2        # 47 <FUNC+0x47>	43: R_X86_64_PC32	shufle65s-0x4
        add(INSTR10, size(INSTR10));
        addConstant(shufle73s, false);
        int INSTR11[] = {0xc4, 0xe2, 0x65, 0x00, 0x1d};// 	vpshufb 0x0(%rip),%ymm3,%ymm3        # 50 <FUNC+0x50>	4c: R_X86_64_PC32	shufle65s-0x4
        add(INSTR11, size(INSTR11));
        addConstant(shufle73s, false);
        int INSTR12[] = {0xc4, 0xe2, 0x5d, 0x00, 0x25};// 	vpshufb 0x0(%rip),%ymm4,%ymm4        # 59 <FUNC+0x59>	55: R_X86_64_PC32	shufle65s-0x4
        add(INSTR12, size(INSTR12));
        addConstant(shufle73s, false);
        int INSTR13[] = {0xc4, 0xe2, 0x55, 0x00, 0x2d};// 	vpshufb 0x0(%rip),%ymm5,%ymm5        # 62 <FUNC+0x62>	5e: R_X86_64_PC32	shufle65s-0x4
        add(INSTR13, size(INSTR13));
        addConstant(shufle73s, false);
        int INSTR14[] = {0xc4, 0xe2, 0x4d, 0x00, 0x35};// 	vpshufb 0x0(%rip),%ymm6,%ymm6        # 6b <FUNC+0x6b>	67: R_X86_64_PC32	shufle65s-0x4
        add(INSTR14, size(INSTR14));
        addConstant(shufle73s, false);
        int INSTR15[] = {0xc4, 0xe2, 0x45, 0x00, 0x3d};// 	vpshufb 0x0(%rip),%ymm7,%ymm7        # 74 <FUNC+0x74>	70: R_X86_64_PC32	shufle65s-0x4
        add(INSTR15, size(INSTR15));
        addConstant(shufle73s, true);
        int INSTR16[] = {0xc4, 0xe2, 0x7d, 0x47, 0x05};// 	vpsllvd 0x0(%rip),%ymm0,%ymm0        # 7d <FUNC+0x7d>	79: R_X86_64_PC32	signed65-0x4
        add(INSTR16, size(INSTR16));
        addConstant(s, true);
        int INSTR17[] = {0xc4, 0xe2, 0x75, 0x47, 0x0d};// 	vpsllvd 0x0(%rip),%ymm1,%ymm1        # 86 <FUNC+0x86>	82: R_X86_64_PC32	signed65-0x4
        add(INSTR17, size(INSTR17));
        addConstant(s, false);
        int INSTR18[] = {0xc4, 0xe2, 0x6d, 0x47, 0x15};// 	vpsllvd 0x0(%rip),%ymm2,%ymm2        # 8f <FUNC+0x8f>	8b: R_X86_64_PC32	signed65-0x4
        add(INSTR18, size(INSTR18));
        addConstant(s, false);
        int INSTR19[] = {0xc4, 0xe2, 0x65, 0x47, 0x1d};// 	vpsllvd 0x0(%rip),%ymm3,%ymm3        # 98 <FUNC+0x98>	94: R_X86_64_PC32	signed65-0x4
        add(INSTR19, size(INSTR19));
        addConstant(s, false);
        int INSTR20[] = {0xc4, 0xe2, 0x5d, 0x47, 0x25};// 	vpsllvd 0x0(%rip),%ymm4,%ymm4        # a1 <FUNC+0xa1>	9d: R_X86_64_PC32	signed65-0x4
        add(INSTR20, size(INSTR20));
        addConstant(s, false);
        int INSTR21[] = {0xc4, 0xe2, 0x55, 0x47, 0x2d};// 	vpsllvd 0x0(%rip),%ymm5,%ymm5        # aa <FUNC+0xaa>	a6: R_X86_64_PC32	signed65-0x4
        add(INSTR21, size(INSTR21));
        addConstant(s, false);
        int INSTR22[] = {0xc4, 0xe2, 0x4d, 0x47, 0x35};// 	vpsllvd 0x0(%rip),%ymm6,%ymm6        # b3 <FUNC+0xb3>	af: R_X86_64_PC32	signed65-0x4
        add(INSTR22, size(INSTR22));
        addConstant(s, false);
        int INSTR23[] = {0xc4, 0xe2, 0x45, 0x47, 0x3d};// 	vpsllvd 0x0(%rip),%ymm7,%ymm7        # bc <FUNC+0xbc>	b8: R_X86_64_PC32	signed65-0x4
        add(INSTR23, size(INSTR23));
        addConstant(s, false);
        int INSTR24[] = {0xc4, 0xe2, 0x7d, 0x46, 0x05};// 	vpsravd 0x0(%rip),%ymm0,%ymm0        # 0xc5 ,0x<FUNC+0xc5>	c1: R_X86_64_PC32	signed65b-0x4
        add(INSTR24, size(INSTR24));
        addConstant(sb, true);
        int INSTR25[] = {0xc4, 0xe2, 0x75, 0x46, 0x0d};// 	vpsravd 0x0(%rip),%ymm1,%ymm1        # ce <FUNC+0xce>	ca: R_X86_64_PC32	signed65b-0x4
        add(INSTR25, size(INSTR25));
        addConstant(sb, false);
        int INSTR26[] = {0xc4, 0xe2, 0x6d, 0x46, 0x15};// 	vpsravd 0x0(%rip),%ymm2,%ymm2        # d7 <FUNC+0xd7>	d3: R_X86_64_PC32	signed65b-0x4
        add(INSTR26, size(INSTR26));
        addConstant(sb, false);
        int INSTR27[] = {0xc4, 0xe2, 0x65, 0x46, 0x1d};// 	vpsravd 0x0(%rip),%ymm3,%ymm3        # e0 <FUNC+0xe0>	dc: R_X86_64_PC32	signed65b-0x4
        add(INSTR27, size(INSTR27));
        addConstant(sb, false);
        int INSTR28[] = {0xc4, 0xe2, 0x5d, 0x46, 0x25};// 	vpsravd 0x0(%rip),%ymm4,%ymm4        # e9 <FUNC+0xe9>	e5: R_X86_64_PC32	signed65b-0x4
        add(INSTR28, size(INSTR28));
        addConstant(sb, false);
        int INSTR29[] = {0xc4, 0xe2, 0x55, 0x46, 0x2d};// 	vpsravd 0x0(%rip),%ymm5,%ymm5        # f2 <FUNC+0xf2>	ee: R_X86_64_PC32	signed65b-0x4
        add(INSTR29, size(INSTR29));
        addConstant(sb, false);
        int INSTR30[] = {0xc4, 0xe2, 0x4d, 0x46, 0x35};// 	vpsravd 0x0(%rip),%ymm6,%ymm6        # fb <FUNC+0xfb>	f7: R_X86_64_PC32	signed65b-0x4
        add(INSTR30, size(INSTR30));
        addConstant(sb, false);
        int INSTR31[] = {0xc4, 0xe2, 0x45, 0x46, 0x3d};// 	vpsravd 0x0(%rip),%ymm7,%ymm7        # 104 <FUNC+0x104>	100: R_X86_64_PC32	signed65b-0x4
        add(INSTR31, size(INSTR31));
        addConstant(sb, false);
    } else {
        int INSTR8[] = {0xc4, 0xe2, 0x7d, 0x00, 0x05};// 	vpshufb 0x0(%rip),%ymm0,%ymm0        # 35 <FUNC+0x35>	31: R_X86_64_PC32	shufle65-0x4
        add(INSTR8, size(INSTR8));
        addConstant(shufle73, true);
        int INSTR9[] = {0xc4, 0xe2, 0x75, 0x00, 0x0d};// 	vpshufb 0x0(%rip),%ymm1,%ymm1        # 3e <FUNC+0x3e>	3a: R_X86_64_PC32	shufle65-0x4
        add(INSTR9, size(INSTR9));
        addConstant(shufle73, false);
        int INSTR10[] = {0xc4, 0xe2, 0x6d, 0x00, 0x15};// 	vpshufb 0x0(%rip),%ymm2,%ymm2        # 47 <FUNC+0x47>	43: R_X86_64_PC32	shufle65-0x4
        add(INSTR10, size(INSTR10));
        addConstant(shufle73, false);
        int INSTR11[] = {0xc4, 0xe2, 0x65, 0x00, 0x1d};// 	vpshufb 0x0(%rip),%ymm3,%ymm3        # 50 <FUNC+0x50>	4c: R_X86_64_PC32	shufle65-0x4
        add(INSTR11, size(INSTR11));
        addConstant(shufle73, false);
        int INSTR12[] = {0xc4, 0xe2, 0x5d, 0x00, 0x25};// 	vpshufb 0x0(%rip),%ymm4,%ymm4        # 59 <FUNC+0x59>	55: R_X86_64_PC32	shufle65-0x4
        add(INSTR12, size(INSTR12));
        addConstant(shufle73, false);
        int INSTR13[] = {0xc4, 0xe2, 0x55, 0x00, 0x2d};// 	vpshufb 0x0(%rip),%ymm5,%ymm5        # 62 <FUNC+0x62>	5e: R_X86_64_PC32	shufle65-0x4
        add(INSTR13, size(INSTR13));
        addConstant(shufle73, false);
        int INSTR14[] = {0xc4, 0xe2, 0x4d, 0x00, 0x35};// 	vpshufb 0x0(%rip),%ymm6,%ymm6        # 6b <FUNC+0x6b>	67: R_X86_64_PC32	shufle65-0x4
        add(INSTR14, size(INSTR14));
        addConstant(shufle73, false);
        int INSTR15[] = {0xc4, 0xe2, 0x45, 0x00, 0x3d};// 	vpshufb 0x0(%rip),%ymm7,%ymm7        # 74 <FUNC+0x74>	70: R_X86_64_PC32	shufle65-0x4
        add(INSTR15, size(INSTR15));
        addConstant(shufle73, false);
    }
}

void Codegen::unpack73(bool isSigned) {
    unpackGen73(isSigned, 73);
}

void Codegen::unpack74(bool isSigned) {
    unpackGen73(isSigned, 74);
}

void Codegen::unpack75(bool isSigned) {
    unpackGen73(isSigned, 75);
}

void Codegen::unpack76(bool isSigned) {
    unpackGen73(isSigned, 76);
}

void Codegen::unpack77(bool isSigned) {
    unpackGen73(isSigned, 77);
}

void Codegen::unpack78(bool isSigned) {
    unpackGen73(isSigned, 78);
}

void Codegen::unpack79(bool isSigned) {
    unpackGen73(isSigned, 79);
}

void Codegen::unpack80(bool isSigned) {
    unpackGen73(isSigned, 80);
}

void Codegen::unpackGen81(bool isSigned, int p) {
    //typedef struct
    //    uint16_t isNull;
    //    uint16_t isEndOfDocument;
    //    BitArray81 values;
    // SM81;
    static __m256i shufle81 = _mm256_set_epi8((char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
                                              (char)0x80, (char)0x80, (char)0x80, (char)0x80, (char)0x80, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5);
    static __m256i shufle81s = _mm256_set_epi8(10, 10, 10, 10, 10, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
                                               15, 15, 15, 15, 15, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5);
    static __m256i signed81 = _mm256_set_epi32(15, 15, 0, 0, 15, 15, 0, 0);
    static __m256i signed81b = _mm256_set_epi32(32, 15, 0, 0, 32, 15, 0, 0);
    static __m256i signed82 = _mm256_set_epi32(14, 14, 0, 0, 14, 14, 0, 0);
    static __m256i signed82b = _mm256_set_epi32(32, 14, 0, 0, 32, 14, 0, 0);
    static __m256i signed83 = _mm256_set_epi32(13, 13, 0, 0, 13, 13, 0, 0);
    static __m256i signed83b = _mm256_set_epi32(32, 13, 0, 0, 32, 13, 0, 0);
    static __m256i signed84 = _mm256_set_epi32(12, 12, 0, 0, 12, 12, 0, 0);
    static __m256i signed84b = _mm256_set_epi32(32, 12, 0, 0, 32, 12, 0, 0);
    static __m256i signed85 = _mm256_set_epi32(11, 11, 0, 0, 11, 11, 0, 0);
    static __m256i signed85b = _mm256_set_epi32(32, 11, 0, 0, 32, 11, 0, 0);
    static __m256i signed86 = _mm256_set_epi32(10, 10, 0, 0, 10, 10, 0, 0);
    static __m256i signed86b = _mm256_set_epi32(32, 10, 0, 0, 32, 10, 0, 0);
    static __m256i signed87 = _mm256_set_epi32(9, 9, 0, 0, 9, 9, 0, 0);
    static __m256i signed87b = _mm256_set_epi32(32, 9, 0, 0, 32, 9, 0, 0);
    static __m256i signed88 = _mm256_set_epi32(8, 8, 0, 0, 8, 8, 0, 0);
    static __m256i signed88b = _mm256_set_epi32(32, 8, 0, 0, 32, 8, 0, 0);

    __m256i s, sb;
    switch (p) {
        case 81:
            s = signed81;
            sb = signed81b;
            break;
        case 82:
            s = signed82;
            sb = signed82b;
            break;
        case 83:
            s = signed83;
            sb = signed83b;
            break;
        case 84:
            s = signed84;
            sb = signed84b;
            break;
        case 85:
            s = signed85;
            sb = signed85b;
            break;
        case 86:
            s = signed86;
            sb = signed86b;
            break;
        case 87:
            s = signed87;
            sb = signed87b;
            break;
        case 88:
            s = signed88;
            sb = signed88b;
            break;
        default:
            break;
    }
    loadPointer();

    int INSTR0[] = {0xc5, 0xff, 0xf0, 0x41, -1};//      	vlddqu -1(%rcx),%ymm0
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc5, 0xff, 0xf0, 0x49, 21};//       	vlddqu 21(%rcx),%ymm1
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc5, 0xff, 0xf0, 0x51, 43};//       	vlddqu 43(%rcx),%ymm2
    add(INSTR2, size(INSTR2));
    int INSTR3[] = {0xc5, 0xff, 0xf0, 0x59, 65};//       	vlddqu 65(%rcx),%ymm3
    add(INSTR3, size(INSTR3));
    int INSTR4[] = {0xc5, 0xff, 0xf0, 0x61, 87};//       	vlddqu 87(%rcx),%ymm4
    add(INSTR4, size(INSTR0));
    int INSTR5[] = {0xc5, 0xff, 0xf0, 0x69, 109};//       	vlddqu 109(%rcx),%ymm5
    add(INSTR5, size(INSTR5));
    int INSTR6[] = {0xc5, 0xff, 0xf0, 0xb1, 131,0,0,0};//       	vlddqu 131(%rcx),%ymm6
    add(INSTR6, size(INSTR6));
    int INSTR7[] = {0xc5, 0xff, 0xf0, 0xb9, 153, 0 ,0 ,0 };//    	vlddqu 153(%rcx),%ymm7
    add(INSTR7, size(INSTR7));
    if (isSigned) {
        int INSTR8[] = {0xc4, 0xe2, 0x7d, 0x00, 0x05};// 	vpshufb 0x0(%rip),%ymm0,%ymm0        # 35 <FUNC+0x35>	31: R_X86_64_PC32	shufle65s-0x4
        add(INSTR8, size(INSTR8));
        addConstant(shufle81s, true);
        int INSTR9[] = {0xc4, 0xe2, 0x75, 0x00, 0x0d};// 	vpshufb 0x0(%rip),%ymm1,%ymm1        # 3e <FUNC+0x3e>	3a: R_X86_64_PC32	shufle65s-0x4
        add(INSTR9, size(INSTR9));
        addConstant(shufle81s, false);
        int INSTR10[] = {0xc4, 0xe2, 0x6d, 0x00, 0x15};// 	vpshufb 0x0(%rip),%ymm2,%ymm2        # 47 <FUNC+0x47>	43: R_X86_64_PC32	shufle65s-0x4
        add(INSTR10, size(INSTR10));
        addConstant(shufle81s, false);
        int INSTR11[] = {0xc4, 0xe2, 0x65, 0x00, 0x1d};// 	vpshufb 0x0(%rip),%ymm3,%ymm3        # 50 <FUNC+0x50>	4c: R_X86_64_PC32	shufle65s-0x4
        add(INSTR11, size(INSTR11));
        addConstant(shufle81s, false);
        int INSTR12[] = {0xc4, 0xe2, 0x5d, 0x00, 0x25};// 	vpshufb 0x0(%rip),%ymm4,%ymm4        # 59 <FUNC+0x59>	55: R_X86_64_PC32	shufle65s-0x4
        add(INSTR12, size(INSTR12));
        addConstant(shufle81s, false);
        int INSTR13[] = {0xc4, 0xe2, 0x55, 0x00, 0x2d};// 	vpshufb 0x0(%rip),%ymm5,%ymm5        # 62 <FUNC+0x62>	5e: R_X86_64_PC32	shufle65s-0x4
        add(INSTR13, size(INSTR13));
        addConstant(shufle81s, false);
        int INSTR14[] = {0xc4, 0xe2, 0x4d, 0x00, 0x35};// 	vpshufb 0x0(%rip),%ymm6,%ymm6        # 6b <FUNC+0x6b>	67: R_X86_64_PC32	shufle65s-0x4
        add(INSTR14, size(INSTR14));
        addConstant(shufle81s, false);
        int INSTR15[] = {0xc4, 0xe2, 0x45, 0x00, 0x3d};// 	vpshufb 0x0(%rip),%ymm7,%ymm7        # 74 <FUNC+0x74>	70: R_X86_64_PC32	shufle65s-0x4
        add(INSTR15, size(INSTR15));
        addConstant(shufle81s, true);
        int INSTR16[] = {0xc4, 0xe2, 0x7d, 0x47, 0x05};// 	vpsllvd 0x0(%rip),%ymm0,%ymm0        # 7d <FUNC+0x7d>	79: R_X86_64_PC32	signed65-0x4
        add(INSTR16, size(INSTR16));
        addConstant(s, true);
        int INSTR17[] = {0xc4, 0xe2, 0x75, 0x47, 0x0d};// 	vpsllvd 0x0(%rip),%ymm1,%ymm1        # 86 <FUNC+0x86>	82: R_X86_64_PC32	signed65-0x4
        add(INSTR17, size(INSTR17));
        addConstant(s, false);
        int INSTR18[] = {0xc4, 0xe2, 0x6d, 0x47, 0x15};// 	vpsllvd 0x0(%rip),%ymm2,%ymm2        # 8f <FUNC+0x8f>	8b: R_X86_64_PC32	signed65-0x4
        add(INSTR18, size(INSTR18));
        addConstant(s, false);
        int INSTR19[] = {0xc4, 0xe2, 0x65, 0x47, 0x1d};// 	vpsllvd 0x0(%rip),%ymm3,%ymm3        # 98 <FUNC+0x98>	94: R_X86_64_PC32	signed65-0x4
        add(INSTR19, size(INSTR19));
        addConstant(s, false);
        int INSTR20[] = {0xc4, 0xe2, 0x5d, 0x47, 0x25};// 	vpsllvd 0x0(%rip),%ymm4,%ymm4        # a1 <FUNC+0xa1>	9d: R_X86_64_PC32	signed65-0x4
        add(INSTR20, size(INSTR20));
        addConstant(s, false);
        int INSTR21[] = {0xc4, 0xe2, 0x55, 0x47, 0x2d};// 	vpsllvd 0x0(%rip),%ymm5,%ymm5        # aa <FUNC+0xaa>	a6: R_X86_64_PC32	signed65-0x4
        add(INSTR21, size(INSTR21));
        addConstant(s, false);
        int INSTR22[] = {0xc4, 0xe2, 0x4d, 0x47, 0x35};// 	vpsllvd 0x0(%rip),%ymm6,%ymm6        # b3 <FUNC+0xb3>	af: R_X86_64_PC32	signed65-0x4
        add(INSTR22, size(INSTR22));
        addConstant(s, false);
        int INSTR23[] = {0xc4, 0xe2, 0x45, 0x47, 0x3d};// 	vpsllvd 0x0(%rip),%ymm7,%ymm7        # bc <FUNC+0xbc>	b8: R_X86_64_PC32	signed65-0x4
        add(INSTR23, size(INSTR23));
        addConstant(s, false);
        int INSTR24[] = {0xc4, 0xe2, 0x7d, 0x46, 0x05};// 	vpsravd 0x0(%rip),%ymm0,%ymm0        # 0xc5 ,0x<FUNC+0xc5>	c1: R_X86_64_PC32	signed65b-0x4
        add(INSTR24, size(INSTR24));
        addConstant(sb, true);
        int INSTR25[] = {0xc4, 0xe2, 0x75, 0x46, 0x0d};// 	vpsravd 0x0(%rip),%ymm1,%ymm1        # ce <FUNC+0xce>	ca: R_X86_64_PC32	signed65b-0x4
        add(INSTR25, size(INSTR25));
        addConstant(sb, false);
        int INSTR26[] = {0xc4, 0xe2, 0x6d, 0x46, 0x15};// 	vpsravd 0x0(%rip),%ymm2,%ymm2        # d7 <FUNC+0xd7>	d3: R_X86_64_PC32	signed65b-0x4
        add(INSTR26, size(INSTR26));
        addConstant(sb, false);
        int INSTR27[] = {0xc4, 0xe2, 0x65, 0x46, 0x1d};// 	vpsravd 0x0(%rip),%ymm3,%ymm3        # e0 <FUNC+0xe0>	dc: R_X86_64_PC32	signed65b-0x4
        add(INSTR27, size(INSTR27));
        addConstant(sb, false);
        int INSTR28[] = {0xc4, 0xe2, 0x5d, 0x46, 0x25};// 	vpsravd 0x0(%rip),%ymm4,%ymm4        # e9 <FUNC+0xe9>	e5: R_X86_64_PC32	signed65b-0x4
        add(INSTR28, size(INSTR28));
        addConstant(sb, false);
        int INSTR29[] = {0xc4, 0xe2, 0x55, 0x46, 0x2d};// 	vpsravd 0x0(%rip),%ymm5,%ymm5        # f2 <FUNC+0xf2>	ee: R_X86_64_PC32	signed65b-0x4
        add(INSTR29, size(INSTR29));
        addConstant(sb, false);
        int INSTR30[] = {0xc4, 0xe2, 0x4d, 0x46, 0x35};// 	vpsravd 0x0(%rip),%ymm6,%ymm6        # fb <FUNC+0xfb>	f7: R_X86_64_PC32	signed65b-0x4
        add(INSTR30, size(INSTR30));
        addConstant(sb, false);
        int INSTR31[] = {0xc4, 0xe2, 0x45, 0x46, 0x3d};// 	vpsravd 0x0(%rip),%ymm7,%ymm7        # 104 <FUNC+0x104>	100: R_X86_64_PC32	signed65b-0x4
        add(INSTR31, size(INSTR31));
        addConstant(sb, false);
    } else {
        int INSTR8[] = {0xc4, 0xe2, 0x7d, 0x00, 0x05};// 	vpshufb 0x0(%rip),%ymm0,%ymm0        # 35 <FUNC+0x35>	31: R_X86_64_PC32	shufle65-0x4
        add(INSTR8, size(INSTR8));
        addConstant(shufle81, true);
        int INSTR9[] = {0xc4, 0xe2, 0x75, 0x00, 0x0d};// 	vpshufb 0x0(%rip),%ymm1,%ymm1        # 3e <FUNC+0x3e>	3a: R_X86_64_PC32	shufle65-0x4
        add(INSTR9, size(INSTR9));
        addConstant(shufle81, false);
        int INSTR10[] = {0xc4, 0xe2, 0x6d, 0x00, 0x15};// 	vpshufb 0x0(%rip),%ymm2,%ymm2        # 47 <FUNC+0x47>	43: R_X86_64_PC32	shufle65-0x4
        add(INSTR10, size(INSTR10));
        addConstant(shufle81, false);
        int INSTR11[] = {0xc4, 0xe2, 0x65, 0x00, 0x1d};// 	vpshufb 0x0(%rip),%ymm3,%ymm3        # 50 <FUNC+0x50>	4c: R_X86_64_PC32	shufle65-0x4
        add(INSTR11, size(INSTR11));
        addConstant(shufle81, false);
        int INSTR12[] = {0xc4, 0xe2, 0x5d, 0x00, 0x25};// 	vpshufb 0x0(%rip),%ymm4,%ymm4        # 59 <FUNC+0x59>	55: R_X86_64_PC32	shufle65-0x4
        add(INSTR12, size(INSTR12));
        addConstant(shufle81, false);
        int INSTR13[] = {0xc4, 0xe2, 0x55, 0x00, 0x2d};// 	vpshufb 0x0(%rip),%ymm5,%ymm5        # 62 <FUNC+0x62>	5e: R_X86_64_PC32	shufle65-0x4
        add(INSTR13, size(INSTR13));
        addConstant(shufle81, false);
        int INSTR14[] = {0xc4, 0xe2, 0x4d, 0x00, 0x35};// 	vpshufb 0x0(%rip),%ymm6,%ymm6        # 6b <FUNC+0x6b>	67: R_X86_64_PC32	shufle65-0x4
        add(INSTR14, size(INSTR14));
        addConstant(shufle81, false);
        int INSTR15[] = {0xc4, 0xe2, 0x45, 0x00, 0x3d};// 	vpshufb 0x0(%rip),%ymm7,%ymm7        # 74 <FUNC+0x74>	70: R_X86_64_PC32	shufle65-0x4
        add(INSTR15, size(INSTR15));
        addConstant(shufle81, false);
    }
}

void Codegen::unpack81(bool isSigned) {
    unpackGen81(isSigned, 81);
}

void Codegen::unpack82(bool isSigned) {
    unpackGen81(isSigned, 82);
}

void Codegen::unpack83(bool isSigned) {
    unpackGen81(isSigned, 83);
}

void Codegen::unpack84(bool isSigned) {
    unpackGen81(isSigned, 84);
}

void Codegen::unpack85(bool isSigned) {
    unpackGen81(isSigned, 85);
}

void Codegen::unpack86(bool isSigned) {
    unpackGen81(isSigned, 86);
}

void Codegen::unpack87(bool isSigned) {
    unpackGen81(isSigned, 87);
}

void Codegen::unpack88(bool isSigned) {
    unpackGen81(isSigned, 88);
}


void Codegen::unpackGen89(bool isSigned, int p) {
    //typedef struct
    //    uint16_t isNull;
    //    uint16_t isEndOfDocument;
    //    BitArray81 values;
    // SM89;
    static __m256i shufle89 = _mm256_set_epi8((char)0x80, (char)0x80, (char)0x80, (char)0x80, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
                                              (char)0x80, (char)0x80, (char)0x80, (char)0x80, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4);
    static __m256i shufle89s = _mm256_set_epi8(11, 11, 11, 11, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
                                               15, 15, 15, 15, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4);
    static __m256i signed89 = _mm256_set_epi32(7, 7, 0, 0, 7, 7, 0, 0);
    static __m256i signed89b = _mm256_set_epi32(32, 7, 0, 0, 32, 7, 0, 0);
    static __m256i signed90 = _mm256_set_epi32(6, 6, 0, 0, 6, 6, 0, 0);
    static __m256i signed90b = _mm256_set_epi32(32, 6, 0, 0, 32, 6, 0, 0);
    static __m256i signed91 = _mm256_set_epi32(5, 5, 0, 0, 5, 5, 0, 0);
    static __m256i signed91b = _mm256_set_epi32(32, 5, 0, 0, 32, 5, 0, 0);
    static __m256i signed92 = _mm256_set_epi32(4, 4, 0, 0, 4, 4, 0, 0);
    static __m256i signed92b = _mm256_set_epi32(32, 4, 0, 0, 32, 4, 0, 0);
    static __m256i signed93 = _mm256_set_epi32(3, 3, 0, 0, 3, 3, 0, 0);
    static __m256i signed93b = _mm256_set_epi32(32, 3, 0, 0, 32, 3, 0, 0);
    static __m256i signed94 = _mm256_set_epi32(2, 2, 0, 0, 2, 2, 0, 0);
    static __m256i signed94b = _mm256_set_epi32(32, 2, 0, 0, 32, 2, 0, 0);
    static __m256i signed95 = _mm256_set_epi32(1, 1, 0, 0, 1, 1, 0, 0);
    static __m256i signed95b = _mm256_set_epi32(32, 1, 0, 0, 32, 1, 0, 0);
    static __m256i signed96 = _mm256_set_epi32(31, 0, 0, 0, 31, 0, 0, 0);

    __m256i s, sb;
    switch (p) {
        case 89:
            s = signed89;
            sb = signed89b;
            break;
        case 90:
            s = signed90;
            sb = signed90b;
            break;
        case 91:
            s = signed91;
            sb = signed91b;
            break;
        case 92:
            s = signed92;
            sb = signed92b;
            break;
        case 93:
            s = signed93;
            sb = signed93b;
            break;
        case 94:
            s = signed94;
            sb = signed94b;
            break;
        case 95:
            s = signed95;
            sb = signed95b;
            break;
        case 96:
            s = signed96;
            sb = signed96;
            break;
        default:
            break;
    }
    loadPointer();

    int INSTR0[] = {0xc5, 0xff, 0xf0, 0x41, 0};//      	vlddqu 0(%rcx),%ymm0
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc5, 0xff, 0xf0, 0x49, 24};//       	vlddqu 24(%rcx),%ymm1
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc5, 0xff, 0xf0, 0x51, 48};//       	vlddqu 48(%rcx),%ymm2
    add(INSTR2, size(INSTR2));
    int INSTR3[] = {0xc5, 0xff, 0xf0, 0x59, 72};//       	vlddqu 72(%rcx),%ymm3
    add(INSTR3, size(INSTR3));
    int INSTR4[] = {0xc5, 0xff, 0xf0, 0x61, 96};//       	vlddqu 96(%rcx),%ymm4
    add(INSTR4, size(INSTR0));
    int INSTR5[] = {0xc5, 0xff, 0xf0, 0x69, 120};//       	vlddqu 120(%rcx),%ymm5
    add(INSTR5, size(INSTR5));
    int INSTR6[] = {0xc5, 0xff, 0xf0, 0xb1, 144,0,0,0};//       	vlddqu 144(%rcx),%ymm6
    add(INSTR6, size(INSTR6));
    int INSTR7[] = {0xc5, 0xff, 0xf0, 0xb9, 168,0,0,0};//       	vlddqu 168(%rcx),%ymm7
    add(INSTR7, size(INSTR7));
    if (isSigned) {
        int INSTR8[] = {0xc4, 0xe2, 0x7d, 0x00, 0x05};// 	vpshufb 0x0(%rip),%ymm0,%ymm0        # 35 <FUNC+0x35>	31: R_X86_64_PC32	shufle65s-0x4
        add(INSTR8, size(INSTR8));
        addConstant(shufle89s, true);
        int INSTR9[] = {0xc4, 0xe2, 0x75, 0x00, 0x0d};// 	vpshufb 0x0(%rip),%ymm1,%ymm1        # 3e <FUNC+0x3e>	3a: R_X86_64_PC32	shufle65s-0x4
        add(INSTR9, size(INSTR9));
        addConstant(shufle89s, false);
        int INSTR10[] = {0xc4, 0xe2, 0x6d, 0x00, 0x15};// 	vpshufb 0x0(%rip),%ymm2,%ymm2        # 47 <FUNC+0x47>	43: R_X86_64_PC32	shufle65s-0x4
        add(INSTR10, size(INSTR10));
        addConstant(shufle89s, false);
        int INSTR11[] = {0xc4, 0xe2, 0x65, 0x00, 0x1d};// 	vpshufb 0x0(%rip),%ymm3,%ymm3        # 50 <FUNC+0x50>	4c: R_X86_64_PC32	shufle65s-0x4
        add(INSTR11, size(INSTR11));
        addConstant(shufle89s, false);
        int INSTR12[] = {0xc4, 0xe2, 0x5d, 0x00, 0x25};// 	vpshufb 0x0(%rip),%ymm4,%ymm4        # 59 <FUNC+0x59>	55: R_X86_64_PC32	shufle65s-0x4
        add(INSTR12, size(INSTR12));
        addConstant(shufle89s, false);
        int INSTR13[] = {0xc4, 0xe2, 0x55, 0x00, 0x2d};// 	vpshufb 0x0(%rip),%ymm5,%ymm5        # 62 <FUNC+0x62>	5e: R_X86_64_PC32	shufle65s-0x4
        add(INSTR13, size(INSTR13));
        addConstant(shufle89s, false);
        int INSTR14[] = {0xc4, 0xe2, 0x4d, 0x00, 0x35};// 	vpshufb 0x0(%rip),%ymm6,%ymm6        # 6b <FUNC+0x6b>	67: R_X86_64_PC32	shufle65s-0x4
        add(INSTR14, size(INSTR14));
        addConstant(shufle89s, false);
        int INSTR15[] = {0xc4, 0xe2, 0x45, 0x00, 0x3d};// 	vpshufb 0x0(%rip),%ymm7,%ymm7        # 74 <FUNC+0x74>	70: R_X86_64_PC32	shufle65s-0x4
        add(INSTR15, size(INSTR15));
        addConstant(shufle89s, true);
        if (p < 96) {
            int INSTR16[] = {0xc4, 0xe2, 0x7d, 0x47, 0x05};// 	vpsllvd 0x0(%rip),%ymm0,%ymm0        # 7d <FUNC+0x7d>	79: R_X86_64_PC32	signed65-0x4
            add(INSTR16, size(INSTR16));
            addConstant(s, true);
            int INSTR17[] = {0xc4, 0xe2, 0x75, 0x47, 0x0d};// 	vpsllvd 0x0(%rip),%ymm1,%ymm1        # 86 <FUNC+0x86>	82: R_X86_64_PC32	signed65-0x4
            add(INSTR17, size(INSTR17));
            addConstant(s, false);
            int INSTR18[] = {0xc4, 0xe2, 0x6d, 0x47, 0x15};// 	vpsllvd 0x0(%rip),%ymm2,%ymm2        # 8f <FUNC+0x8f>	8b: R_X86_64_PC32	signed65-0x4
            add(INSTR18, size(INSTR18));
            addConstant(s, false);
            int INSTR19[] = {0xc4, 0xe2, 0x65, 0x47, 0x1d};// 	vpsllvd 0x0(%rip),%ymm3,%ymm3        # 98 <FUNC+0x98>	94: R_X86_64_PC32	signed65-0x4
            add(INSTR19, size(INSTR19));
            addConstant(s, false);
            int INSTR20[] = {0xc4, 0xe2, 0x5d, 0x47, 0x25};// 	vpsllvd 0x0(%rip),%ymm4,%ymm4        # a1 <FUNC+0xa1>	9d: R_X86_64_PC32	signed65-0x4
            add(INSTR20, size(INSTR20));
            addConstant(s, false);
            int INSTR21[] = {0xc4, 0xe2, 0x55, 0x47, 0x2d};// 	vpsllvd 0x0(%rip),%ymm5,%ymm5        # aa <FUNC+0xaa>	a6: R_X86_64_PC32	signed65-0x4
            add(INSTR21, size(INSTR21));
            addConstant(s, false);
            int INSTR22[] = {0xc4, 0xe2, 0x4d, 0x47, 0x35};// 	vpsllvd 0x0(%rip),%ymm6,%ymm6        # b3 <FUNC+0xb3>	af: R_X86_64_PC32	signed65-0x4
            add(INSTR22, size(INSTR22));
            addConstant(s, false);
            int INSTR23[] = {0xc4, 0xe2, 0x45, 0x47, 0x3d};// 	vpsllvd 0x0(%rip),%ymm7,%ymm7        # bc <FUNC+0xbc>	b8: R_X86_64_PC32	signed65-0x4
            add(INSTR23, size(INSTR23));
            addConstant(s, false);
        }
        int INSTR24[] = {0xc4, 0xe2, 0x7d, 0x46, 0x05};// 	vpsravd 0x0(%rip),%ymm0,%ymm0        # 0xc5 ,0x<FUNC+0xc5>	c1: R_X86_64_PC32	signed65b-0x4
        add(INSTR24, size(INSTR24));
        addConstant(sb, true);
        int INSTR25[] = {0xc4, 0xe2, 0x75, 0x46, 0x0d};// 	vpsravd 0x0(%rip),%ymm1,%ymm1        # ce <FUNC+0xce>	ca: R_X86_64_PC32	signed65b-0x4
        add(INSTR25, size(INSTR25));
        addConstant(sb, false);
        int INSTR26[] = {0xc4, 0xe2, 0x6d, 0x46, 0x15};// 	vpsravd 0x0(%rip),%ymm2,%ymm2        # d7 <FUNC+0xd7>	d3: R_X86_64_PC32	signed65b-0x4
        add(INSTR26, size(INSTR26));
        addConstant(sb, false);
        int INSTR27[] = {0xc4, 0xe2, 0x65, 0x46, 0x1d};// 	vpsravd 0x0(%rip),%ymm3,%ymm3        # e0 <FUNC+0xe0>	dc: R_X86_64_PC32	signed65b-0x4
        add(INSTR27, size(INSTR27));
        addConstant(sb, false);
        int INSTR28[] = {0xc4, 0xe2, 0x5d, 0x46, 0x25};// 	vpsravd 0x0(%rip),%ymm4,%ymm4        # e9 <FUNC+0xe9>	e5: R_X86_64_PC32	signed65b-0x4
        add(INSTR28, size(INSTR28));
        addConstant(sb, false);
        int INSTR29[] = {0xc4, 0xe2, 0x55, 0x46, 0x2d};// 	vpsravd 0x0(%rip),%ymm5,%ymm5        # f2 <FUNC+0xf2>	ee: R_X86_64_PC32	signed65b-0x4
        add(INSTR29, size(INSTR29));
        addConstant(sb, false);
        int INSTR30[] = {0xc4, 0xe2, 0x4d, 0x46, 0x35};// 	vpsravd 0x0(%rip),%ymm6,%ymm6        # fb <FUNC+0xfb>	f7: R_X86_64_PC32	signed65b-0x4
        add(INSTR30, size(INSTR30));
        addConstant(sb, false);
        int INSTR31[] = {0xc4, 0xe2, 0x45, 0x46, 0x3d};// 	vpsravd 0x0(%rip),%ymm7,%ymm7        # 104 <FUNC+0x104>	100: R_X86_64_PC32	signed65b-0x4
        add(INSTR31, size(INSTR31));
        addConstant(sb, false);
    } else {
        int INSTR8[] = {0xc4, 0xe2, 0x7d, 0x00, 0x05};// 	vpshufb 0x0(%rip),%ymm0,%ymm0        # 35 <FUNC+0x35>	31: R_X86_64_PC32	shufle65-0x4
        add(INSTR8, size(INSTR8));
        addConstant(shufle89, true);
        int INSTR9[] = {0xc4, 0xe2, 0x75, 0x00, 0x0d};// 	vpshufb 0x0(%rip),%ymm1,%ymm1        # 3e <FUNC+0x3e>	3a: R_X86_64_PC32	shufle65-0x4
        add(INSTR9, size(INSTR9));
        addConstant(shufle89, false);
        int INSTR10[] = {0xc4, 0xe2, 0x6d, 0x00, 0x15};// 	vpshufb 0x0(%rip),%ymm2,%ymm2        # 47 <FUNC+0x47>	43: R_X86_64_PC32	shufle65-0x4
        add(INSTR10, size(INSTR10));
        addConstant(shufle89, false);
        int INSTR11[] = {0xc4, 0xe2, 0x65, 0x00, 0x1d};// 	vpshufb 0x0(%rip),%ymm3,%ymm3        # 50 <FUNC+0x50>	4c: R_X86_64_PC32	shufle65-0x4
        add(INSTR11, size(INSTR11));
        addConstant(shufle89, false);
        int INSTR12[] = {0xc4, 0xe2, 0x5d, 0x00, 0x25};// 	vpshufb 0x0(%rip),%ymm4,%ymm4        # 59 <FUNC+0x59>	55: R_X86_64_PC32	shufle65-0x4
        add(INSTR12, size(INSTR12));
        addConstant(shufle89, false);
        int INSTR13[] = {0xc4, 0xe2, 0x55, 0x00, 0x2d};// 	vpshufb 0x0(%rip),%ymm5,%ymm5        # 62 <FUNC+0x62>	5e: R_X86_64_PC32	shufle65-0x4
        add(INSTR13, size(INSTR13));
        addConstant(shufle89, false);
        int INSTR14[] = {0xc4, 0xe2, 0x4d, 0x00, 0x35};// 	vpshufb 0x0(%rip),%ymm6,%ymm6        # 6b <FUNC+0x6b>	67: R_X86_64_PC32	shufle65-0x4
        add(INSTR14, size(INSTR14));
        addConstant(shufle89, false);
        int INSTR15[] = {0xc4, 0xe2, 0x45, 0x00, 0x3d};// 	vpshufb 0x0(%rip),%ymm7,%ymm7        # 74 <FUNC+0x74>	70: R_X86_64_PC32	shufle65-0x4
        add(INSTR15, size(INSTR15));
        addConstant(shufle89, false);
    }
}

void Codegen::unpack89(bool isSigned) {
    unpackGen89(isSigned, 89);
}

void Codegen::unpack90(bool isSigned) {
    unpackGen89(isSigned, 90);
}

void Codegen::unpack91(bool isSigned) {
    unpackGen89(isSigned, 91);
}

void Codegen::unpack92(bool isSigned) {
    unpackGen89(isSigned, 92);
}

void Codegen::unpack93(bool isSigned) {
    unpackGen89(isSigned, 93);
}

void Codegen::unpack94(bool isSigned) {
    unpackGen89(isSigned, 94);
}

void Codegen::unpack95(bool isSigned) {
    unpackGen89(isSigned, 95);
}

void Codegen::unpack96(bool isSigned) {
    unpackGen89(isSigned, 96);
}

void Codegen::unpackGen97(bool isSigned, int p) {
    //typedef struct
    //    uint16_t isNull;
    //    uint16_t isEndOfDocument;
    //    BitArray97 values;
    // SM97;
    static __m256i shufle97 = _mm256_set_epi8((char)0x80, (char)0x80, (char)0x80, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
                                              (char)0x80, (char)0x80, (char)0x80, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3);
    static __m256i shufle97s = _mm256_set_epi8(12, 12, 12, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
                                               15, 15, 15, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3);
    static __m256i signed97 = _mm256_set_epi32(31, 0, 0, 0, 31, 0, 0, 0);
    static __m256i signed98 = _mm256_set_epi32(30, 0, 0, 0, 30, 0, 0, 0);
    static __m256i signed99 = _mm256_set_epi32(29, 0, 0, 0, 29, 0, 0, 0);
    static __m256i signed100 = _mm256_set_epi32(28, 0, 0, 0, 28, 0, 0, 0);
    static __m256i signed101 = _mm256_set_epi32(27, 0, 0, 0, 27, 0, 0, 0);
    static __m256i signed102 = _mm256_set_epi32(26, 0, 0, 0, 26, 0, 0, 0);
    static __m256i signed103 = _mm256_set_epi32(25, 0, 0, 0, 25, 0, 0, 0);
    static __m256i signed104 = _mm256_set_epi32(24, 0, 0, 0, 24, 0, 0, 0);

    __m256i s;
    switch (p) {
        case 97:
            s = signed97;
            break;
        case 98:
            s = signed98;
            break;
        case 99:
            s = signed99;
            break;
        case 100:
            s = signed100;
            break;
        case 101:
            s = signed101;
            break;
        case 102:
            s = signed102;
            break;
        case 103:
            s = signed103;
            break;
        case 104:
            s = signed104;
            break;
        default:
            break;
    }
    loadPointer();

    int INSTR0[] = {0xc5, 0xff, 0xf0, 0x41, 1};//      	vlddqu 1(%rcx),%ymm0
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc5, 0xff, 0xf0, 0x49, 27};//       	vlddqu 27(%rcx),%ymm1
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc5, 0xff, 0xf0, 0x51, 53};//       	vlddqu 51(%rcx),%ymm2
    add(INSTR2, size(INSTR2));
    int INSTR3[] = {0xc5, 0xff, 0xf0, 0x59, 79};//       	vlddqu 79(%rcx),%ymm3
    add(INSTR3, size(INSTR3));
    int INSTR4[] = {0xc5, 0xff, 0xf0, 0x61, 105};//       	vlddqu 105(%rcx),%ymm4
    add(INSTR4, size(INSTR0));
    int INSTR5[] = {0xc5, 0xff, 0xf0, 0xa9, 131,0,0,0};//  	vlddqu 131(%rcx),%ymm5
    add(INSTR5, size(INSTR5));
    int INSTR6[] = {0xc5, 0xff, 0xf0, 0xb1, 157,0,0,0};//  	vlddqu 157(%rcx),%ymm6
    add(INSTR6, size(INSTR6));
    int INSTR7[] = {0xc5, 0xff, 0xf0, 0xb9, 183,0,0,0};//  	vlddqu 183(%rcx),%ymm7
    add(INSTR7, size(INSTR7));
    if (isSigned) {
        int INSTR8[] = {0xc4, 0xe2, 0x7d, 0x00, 0x05};// 	vpshufb 0x0(%rip),%ymm0,%ymm0        # 35 <FUNC+0x35>	31: R_X86_64_PC32	shufle65s-0x4
        add(INSTR8, size(INSTR8));
        addConstant(shufle97s, true);
        int INSTR9[] = {0xc4, 0xe2, 0x75, 0x00, 0x0d};// 	vpshufb 0x0(%rip),%ymm1,%ymm1        # 3e <FUNC+0x3e>	3a: R_X86_64_PC32	shufle65s-0x4
        add(INSTR9, size(INSTR9));
        addConstant(shufle97s, false);
        int INSTR10[] = {0xc4, 0xe2, 0x6d, 0x00, 0x15};// 	vpshufb 0x0(%rip),%ymm2,%ymm2        # 47 <FUNC+0x47>	43: R_X86_64_PC32	shufle65s-0x4
        add(INSTR10, size(INSTR10));
        addConstant(shufle97s, false);
        int INSTR11[] = {0xc4, 0xe2, 0x65, 0x00, 0x1d};// 	vpshufb 0x0(%rip),%ymm3,%ymm3        # 50 <FUNC+0x50>	4c: R_X86_64_PC32	shufle65s-0x4
        add(INSTR11, size(INSTR11));
        addConstant(shufle97s, false);
        int INSTR12[] = {0xc4, 0xe2, 0x5d, 0x00, 0x25};// 	vpshufb 0x0(%rip),%ymm4,%ymm4        # 59 <FUNC+0x59>	55: R_X86_64_PC32	shufle65s-0x4
        add(INSTR12, size(INSTR12));
        addConstant(shufle97s, false);
        int INSTR13[] = {0xc4, 0xe2, 0x55, 0x00, 0x2d};// 	vpshufb 0x0(%rip),%ymm5,%ymm5        # 62 <FUNC+0x62>	5e: R_X86_64_PC32	shufle65s-0x4
        add(INSTR13, size(INSTR13));
        addConstant(shufle97s, false);
        int INSTR14[] = {0xc4, 0xe2, 0x4d, 0x00, 0x35};// 	vpshufb 0x0(%rip),%ymm6,%ymm6        # 6b <FUNC+0x6b>	67: R_X86_64_PC32	shufle65s-0x4
        add(INSTR14, size(INSTR14));
        addConstant(shufle97s, false);
        int INSTR15[] = {0xc4, 0xe2, 0x45, 0x00, 0x3d};// 	vpshufb 0x0(%rip),%ymm7,%ymm7        # 74 <FUNC+0x74>	70: R_X86_64_PC32	shufle65s-0x4
        add(INSTR15, size(INSTR15));
        addConstant(shufle97s, true);
        if (p < 104) {
            int INSTR16[] = {0xc4, 0xe2, 0x7d, 0x47, 0x05};// 	vpsllvd 0x0(%rip),%ymm0,%ymm0        # 7d <FUNC+0x7d>	79: R_X86_64_PC32	signed65-0x4
            add(INSTR16, size(INSTR16));
            addConstant(s, true);
            int INSTR17[] = {0xc4, 0xe2, 0x75, 0x47, 0x0d};// 	vpsllvd 0x0(%rip),%ymm1,%ymm1        # 86 <FUNC+0x86>	82: R_X86_64_PC32	signed65-0x4
            add(INSTR17, size(INSTR17));
            addConstant(s, false);
            int INSTR18[] = {0xc4, 0xe2, 0x6d, 0x47, 0x15};// 	vpsllvd 0x0(%rip),%ymm2,%ymm2        # 8f <FUNC+0x8f>	8b: R_X86_64_PC32	signed65-0x4
            add(INSTR18, size(INSTR18));
            addConstant(s, false);
            int INSTR19[] = {0xc4, 0xe2, 0x65, 0x47, 0x1d};// 	vpsllvd 0x0(%rip),%ymm3,%ymm3        # 98 <FUNC+0x98>	94: R_X86_64_PC32	signed65-0x4
            add(INSTR19, size(INSTR19));
            addConstant(s, false);
            int INSTR20[] = {0xc4, 0xe2, 0x5d, 0x47, 0x25};// 	vpsllvd 0x0(%rip),%ymm4,%ymm4        # a1 <FUNC+0xa1>	9d: R_X86_64_PC32	signed65-0x4
            add(INSTR20, size(INSTR20));
            addConstant(s, false);
            int INSTR21[] = {0xc4, 0xe2, 0x55, 0x47, 0x2d};// 	vpsllvd 0x0(%rip),%ymm5,%ymm5        # aa <FUNC+0xaa>	a6: R_X86_64_PC32	signed65-0x4
            add(INSTR21, size(INSTR21));
            addConstant(s, false);
            int INSTR22[] = {0xc4, 0xe2, 0x4d, 0x47, 0x35};// 	vpsllvd 0x0(%rip),%ymm6,%ymm6        # b3 <FUNC+0xb3>	af: R_X86_64_PC32	signed65-0x4
            add(INSTR22, size(INSTR22));
            addConstant(s, false);
            int INSTR23[] = {0xc4, 0xe2, 0x45, 0x47, 0x3d};// 	vpsllvd 0x0(%rip),%ymm7,%ymm7        # bc <FUNC+0xbc>	b8: R_X86_64_PC32	signed65-0x4
            add(INSTR23, size(INSTR23));
            addConstant(s, false);
        }
        int INSTR24[] = {0xc4, 0xe2, 0x7d, 0x46, 0x05};// 	vpsravd 0x0(%rip),%ymm0,%ymm0        # 0xc5 ,0x<FUNC+0xc5>	c1: R_X86_64_PC32	signed65b-0x4
        add(INSTR24, size(INSTR24));
        addConstant(s, true);
        int INSTR25[] = {0xc4, 0xe2, 0x75, 0x46, 0x0d};// 	vpsravd 0x0(%rip),%ymm1,%ymm1        # ce <FUNC+0xce>	ca: R_X86_64_PC32	signed65b-0x4
        add(INSTR25, size(INSTR25));
        addConstant(s, false);
        int INSTR26[] = {0xc4, 0xe2, 0x6d, 0x46, 0x15};// 	vpsravd 0x0(%rip),%ymm2,%ymm2        # d7 <FUNC+0xd7>	d3: R_X86_64_PC32	signed65b-0x4
        add(INSTR26, size(INSTR26));
        addConstant(s, false);
        int INSTR27[] = {0xc4, 0xe2, 0x65, 0x46, 0x1d};// 	vpsravd 0x0(%rip),%ymm3,%ymm3        # e0 <FUNC+0xe0>	dc: R_X86_64_PC32	signed65b-0x4
        add(INSTR27, size(INSTR27));
        addConstant(s, false);
        int INSTR28[] = {0xc4, 0xe2, 0x5d, 0x46, 0x25};// 	vpsravd 0x0(%rip),%ymm4,%ymm4        # e9 <FUNC+0xe9>	e5: R_X86_64_PC32	signed65b-0x4
        add(INSTR28, size(INSTR28));
        addConstant(s, false);
        int INSTR29[] = {0xc4, 0xe2, 0x55, 0x46, 0x2d};// 	vpsravd 0x0(%rip),%ymm5,%ymm5        # f2 <FUNC+0xf2>	ee: R_X86_64_PC32	signed65b-0x4
        add(INSTR29, size(INSTR29));
        addConstant(s, false);
        int INSTR30[] = {0xc4, 0xe2, 0x4d, 0x46, 0x35};// 	vpsravd 0x0(%rip),%ymm6,%ymm6        # fb <FUNC+0xfb>	f7: R_X86_64_PC32	signed65b-0x4
        add(INSTR30, size(INSTR30));
        addConstant(s, false);
        int INSTR31[] = {0xc4, 0xe2, 0x45, 0x46, 0x3d};// 	vpsravd 0x0(%rip),%ymm7,%ymm7        # 104 <FUNC+0x104>	100: R_X86_64_PC32	signed65b-0x4
        add(INSTR31, size(INSTR31));
        addConstant(s, false);
    } else {
        int INSTR8[] = {0xc4, 0xe2, 0x7d, 0x00, 0x05};// 	vpshufb 0x0(%rip),%ymm0,%ymm0        # 35 <FUNC+0x35>	31: R_X86_64_PC32	shufle65-0x4
        add(INSTR8, size(INSTR8));
        addConstant(shufle97, true);
        int INSTR9[] = {0xc4, 0xe2, 0x75, 0x00, 0x0d};// 	vpshufb 0x0(%rip),%ymm1,%ymm1        # 3e <FUNC+0x3e>	3a: R_X86_64_PC32	shufle65-0x4
        add(INSTR9, size(INSTR9));
        addConstant(shufle97, false);
        int INSTR10[] = {0xc4, 0xe2, 0x6d, 0x00, 0x15};// 	vpshufb 0x0(%rip),%ymm2,%ymm2        # 47 <FUNC+0x47>	43: R_X86_64_PC32	shufle65-0x4
        add(INSTR10, size(INSTR10));
        addConstant(shufle97, false);
        int INSTR11[] = {0xc4, 0xe2, 0x65, 0x00, 0x1d};// 	vpshufb 0x0(%rip),%ymm3,%ymm3        # 50 <FUNC+0x50>	4c: R_X86_64_PC32	shufle65-0x4
        add(INSTR11, size(INSTR11));
        addConstant(shufle97, false);
        int INSTR12[] = {0xc4, 0xe2, 0x5d, 0x00, 0x25};// 	vpshufb 0x0(%rip),%ymm4,%ymm4        # 59 <FUNC+0x59>	55: R_X86_64_PC32	shufle65-0x4
        add(INSTR12, size(INSTR12));
        addConstant(shufle97, false);
        int INSTR13[] = {0xc4, 0xe2, 0x55, 0x00, 0x2d};// 	vpshufb 0x0(%rip),%ymm5,%ymm5        # 62 <FUNC+0x62>	5e: R_X86_64_PC32	shufle65-0x4
        add(INSTR13, size(INSTR13));
        addConstant(shufle97, false);
        int INSTR14[] = {0xc4, 0xe2, 0x4d, 0x00, 0x35};// 	vpshufb 0x0(%rip),%ymm6,%ymm6        # 6b <FUNC+0x6b>	67: R_X86_64_PC32	shufle65-0x4
        add(INSTR14, size(INSTR14));
        addConstant(shufle97, false);
        int INSTR15[] = {0xc4, 0xe2, 0x45, 0x00, 0x3d};// 	vpshufb 0x0(%rip),%ymm7,%ymm7        # 74 <FUNC+0x74>	70: R_X86_64_PC32	shufle65-0x4
        add(INSTR15, size(INSTR15));
        addConstant(shufle97, false);
    }
}

void Codegen::unpack97(bool isSigned) {
    unpackGen97(isSigned, 97);
}

void Codegen::unpack98(bool isSigned) {
    unpackGen97(isSigned, 98);
}

void Codegen::unpack99(bool isSigned) {
    unpackGen97(isSigned, 99);
}

void Codegen::unpack100(bool isSigned) {
    unpackGen97(isSigned, 100);
}

void Codegen::unpack101(bool isSigned) {
    unpackGen97(isSigned, 101);
}

void Codegen::unpack102(bool isSigned) {
    unpackGen97(isSigned, 102);
}

void Codegen::unpack103(bool isSigned) {
    unpackGen97(isSigned, 103);
}

void Codegen::unpack104(bool isSigned) {
    unpackGen97(isSigned, 104);
}

void Codegen::unpackGen105(bool isSigned, int p) {
    // typedef struct
    //    uint16_t isNull;
    //    uint16_t isEndOfDocument;
    //    BitArray105 values;
    // SM105;
    static __m256i shufle105 = _mm256_set_epi8((char)0x80, (char)0x80, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
                                               (char)0x80, (char)0x80, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2);
    static __m256i shufle105s = _mm256_set_epi8(13, 13, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
                                                15, 15, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2);
    static __m256i signed105 = _mm256_set_epi32(23, 0, 0, 0, 23, 0, 0, 0);
    static __m256i signed106 = _mm256_set_epi32(22, 0, 0, 0, 22, 0, 0, 0);
    static __m256i signed107 = _mm256_set_epi32(21, 0, 0, 0, 21, 0, 0, 0);
    static __m256i signed108 = _mm256_set_epi32(20, 0, 0, 0, 20, 0, 0, 0);
    static __m256i signed109 = _mm256_set_epi32(19, 0, 0, 0, 19, 0, 0, 0);
    static __m256i signed110 = _mm256_set_epi32(18, 0, 0, 0, 18, 0, 0, 0);
    static __m256i signed111 = _mm256_set_epi32(17, 0, 0, 0, 17, 0, 0, 0);
    static __m256i signed112 = _mm256_set_epi32(16, 0, 0, 0, 16, 0, 0, 0);

    __m256i s;
    switch (p) {
        case 105:
            s = signed105;
            break;
        case 106:
            s = signed106;
            break;
        case 107:
            s = signed107;
            break;
        case 108:
            s = signed108;
            break;
        case 109:
            s = signed109;
            break;
        case 110:
            s = signed110;
            break;
        case 111:
            s = signed111;
            break;
        case 112:
            s = signed112;
            break;
        default:
            break;
    }
    loadPointer();

    int INSTR0[] = {0xc5, 0xff, 0xf0, 0x41, 2};//      	vlddqu 2(%rcx),%ymm0
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc5, 0xff, 0xf0, 0x49, 30};//       	vlddqu 30(%rcx),%ymm1
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc5, 0xff, 0xf0, 0x51, 58};//       	vlddqu 58(%rcx),%ymm2
    add(INSTR2, size(INSTR2));
    int INSTR3[] = {0xc5, 0xff, 0xf0, 0x59, 86};//       	vlddqu 86(%rcx),%ymm3
    add(INSTR3, size(INSTR3));
    int INSTR4[] = {0xc5, 0xff, 0xf0, 0x61, 114};//       	vlddqu 114(%rcx),%ymm4
    add(INSTR4, size(INSTR0));
    int INSTR5[] = {0xc5, 0xff, 0xf0, 0xa9, 142,0,0,0};//    vlddqu 142(%rcx),%ymm5
    add(INSTR5, size(INSTR5));
    int INSTR6[] = {0xc5, 0xff, 0xf0, 0xb1, 170,0,0,0};//    vlddqu 170(%rcx),%ymm6
    add(INSTR6, size(INSTR6));
    int INSTR7[] = {0xc5, 0xff, 0xf0, 0xb9, 198,0,0,0};//    vlddqu 196(%rcx),%ymm7
    add(INSTR7, size(INSTR7));
    if (isSigned) {
        int INSTR8[] = {0xc4, 0xe2, 0x7d, 0x00, 0x05};// 	vpshufb 0x0(%rip),%ymm0,%ymm0        # 35 <FUNC+0x35>	31: R_X86_64_PC32	shufle65s-0x4
        add(INSTR8, size(INSTR8));
        addConstant(shufle105s, true);
        int INSTR9[] = {0xc4, 0xe2, 0x75, 0x00, 0x0d};// 	vpshufb 0x0(%rip),%ymm1,%ymm1        # 3e <FUNC+0x3e>	3a: R_X86_64_PC32	shufle65s-0x4
        add(INSTR9, size(INSTR9));
        addConstant(shufle105s, false);
        int INSTR10[] = {0xc4, 0xe2, 0x6d, 0x00, 0x15};// 	vpshufb 0x0(%rip),%ymm2,%ymm2        # 47 <FUNC+0x47>	43: R_X86_64_PC32	shufle65s-0x4
        add(INSTR10, size(INSTR10));
        addConstant(shufle105s, false);
        int INSTR11[] = {0xc4, 0xe2, 0x65, 0x00, 0x1d};// 	vpshufb 0x0(%rip),%ymm3,%ymm3        # 50 <FUNC+0x50>	4c: R_X86_64_PC32	shufle65s-0x4
        add(INSTR11, size(INSTR11));
        addConstant(shufle105s, false);
        int INSTR12[] = {0xc4, 0xe2, 0x5d, 0x00, 0x25};// 	vpshufb 0x0(%rip),%ymm4,%ymm4        # 59 <FUNC+0x59>	55: R_X86_64_PC32	shufle65s-0x4
        add(INSTR12, size(INSTR12));
        addConstant(shufle105s, false);
        int INSTR13[] = {0xc4, 0xe2, 0x55, 0x00, 0x2d};// 	vpshufb 0x0(%rip),%ymm5,%ymm5        # 62 <FUNC+0x62>	5e: R_X86_64_PC32	shufle65s-0x4
        add(INSTR13, size(INSTR13));
        addConstant(shufle105s, false);
        int INSTR14[] = {0xc4, 0xe2, 0x4d, 0x00, 0x35};// 	vpshufb 0x0(%rip),%ymm6,%ymm6        # 6b <FUNC+0x6b>	67: R_X86_64_PC32	shufle65s-0x4
        add(INSTR14, size(INSTR14));
        addConstant(shufle105s, false);
        int INSTR15[] = {0xc4, 0xe2, 0x45, 0x00, 0x3d};// 	vpshufb 0x0(%rip),%ymm7,%ymm7        # 74 <FUNC+0x74>	70: R_X86_64_PC32	shufle65s-0x4
        add(INSTR15, size(INSTR15));
        addConstant(shufle105s, true);
        int INSTR16[] = {0xc4, 0xe2, 0x7d, 0x47, 0x05};// 	vpsllvd 0x0(%rip),%ymm0,%ymm0        # 7d <FUNC+0x7d>	79: R_X86_64_PC32	signed65-0x4
        add(INSTR16, size(INSTR16));
        addConstant(s, true);
        int INSTR17[] = {0xc4, 0xe2, 0x75, 0x47, 0x0d};// 	vpsllvd 0x0(%rip),%ymm1,%ymm1        # 86 <FUNC+0x86>	82: R_X86_64_PC32	signed65-0x4
        add(INSTR17, size(INSTR17));
        addConstant(s, false);
        int INSTR18[] = {0xc4, 0xe2, 0x6d, 0x47, 0x15};// 	vpsllvd 0x0(%rip),%ymm2,%ymm2        # 8f <FUNC+0x8f>	8b: R_X86_64_PC32	signed65-0x4
        add(INSTR18, size(INSTR18));
        addConstant(s, false);
        int INSTR19[] = {0xc4, 0xe2, 0x65, 0x47, 0x1d};// 	vpsllvd 0x0(%rip),%ymm3,%ymm3        # 98 <FUNC+0x98>	94: R_X86_64_PC32	signed65-0x4
        add(INSTR19, size(INSTR19));
        addConstant(s, false);
        int INSTR20[] = {0xc4, 0xe2, 0x5d, 0x47, 0x25};// 	vpsllvd 0x0(%rip),%ymm4,%ymm4        # a1 <FUNC+0xa1>	9d: R_X86_64_PC32	signed65-0x4
        add(INSTR20, size(INSTR20));
        addConstant(s, false);
        int INSTR21[] = {0xc4, 0xe2, 0x55, 0x47, 0x2d};// 	vpsllvd 0x0(%rip),%ymm5,%ymm5        # aa <FUNC+0xaa>	a6: R_X86_64_PC32	signed65-0x4
        add(INSTR21, size(INSTR21));
        addConstant(s, false);
        int INSTR22[] = {0xc4, 0xe2, 0x4d, 0x47, 0x35};// 	vpsllvd 0x0(%rip),%ymm6,%ymm6        # b3 <FUNC+0xb3>	af: R_X86_64_PC32	signed65-0x4
        add(INSTR22, size(INSTR22));
        addConstant(s, false);
        int INSTR23[] = {0xc4, 0xe2, 0x45, 0x47, 0x3d};// 	vpsllvd 0x0(%rip),%ymm7,%ymm7        # bc <FUNC+0xbc>	b8: R_X86_64_PC32	signed65-0x4
        add(INSTR23, size(INSTR23));
        addConstant(s, false);
        int INSTR24[] = {0xc4, 0xe2, 0x7d, 0x46, 0x05};// 	vpsravd 0x0(%rip),%ymm0,%ymm0        # 0xc5 ,0x<FUNC+0xc5>	c1: R_X86_64_PC32	signed65b-0x4
        add(INSTR24, size(INSTR24));
        addConstant(s, true);
        int INSTR25[] = {0xc4, 0xe2, 0x75, 0x46, 0x0d};// 	vpsravd 0x0(%rip),%ymm1,%ymm1        # ce <FUNC+0xce>	ca: R_X86_64_PC32	signed65b-0x4
        add(INSTR25, size(INSTR25));
        addConstant(s, false);
        int INSTR26[] = {0xc4, 0xe2, 0x6d, 0x46, 0x15};// 	vpsravd 0x0(%rip),%ymm2,%ymm2        # d7 <FUNC+0xd7>	d3: R_X86_64_PC32	signed65b-0x4
        add(INSTR26, size(INSTR26));
        addConstant(s, false);
        int INSTR27[] = {0xc4, 0xe2, 0x65, 0x46, 0x1d};// 	vpsravd 0x0(%rip),%ymm3,%ymm3        # e0 <FUNC+0xe0>	dc: R_X86_64_PC32	signed65b-0x4
        add(INSTR27, size(INSTR27));
        addConstant(s, false);
        int INSTR28[] = {0xc4, 0xe2, 0x5d, 0x46, 0x25};// 	vpsravd 0x0(%rip),%ymm4,%ymm4        # e9 <FUNC+0xe9>	e5: R_X86_64_PC32	signed65b-0x4
        add(INSTR28, size(INSTR28));
        addConstant(s, false);
        int INSTR29[] = {0xc4, 0xe2, 0x55, 0x46, 0x2d};// 	vpsravd 0x0(%rip),%ymm5,%ymm5        # f2 <FUNC+0xf2>	ee: R_X86_64_PC32	signed65b-0x4
        add(INSTR29, size(INSTR29));
        addConstant(s, false);
        int INSTR30[] = {0xc4, 0xe2, 0x4d, 0x46, 0x35};// 	vpsravd 0x0(%rip),%ymm6,%ymm6        # fb <FUNC+0xfb>	f7: R_X86_64_PC32	signed65b-0x4
        add(INSTR30, size(INSTR30));
        addConstant(s, false);
        int INSTR31[] = {0xc4, 0xe2, 0x45, 0x46, 0x3d};// 	vpsravd 0x0(%rip),%ymm7,%ymm7        # 104 <FUNC+0x104>	100: R_X86_64_PC32	signed65b-0x4
        add(INSTR31, size(INSTR31));
        addConstant(s, false);
    } else {
        int INSTR8[] = {0xc4, 0xe2, 0x7d, 0x00, 0x05};// 	vpshufb 0x0(%rip),%ymm0,%ymm0        # 35 <FUNC+0x35>	31: R_X86_64_PC32	shufle65-0x4
        add(INSTR8, size(INSTR8));
        addConstant(shufle105, true);
        int INSTR9[] = {0xc4, 0xe2, 0x75, 0x00, 0x0d};// 	vpshufb 0x0(%rip),%ymm1,%ymm1        # 3e <FUNC+0x3e>	3a: R_X86_64_PC32	shufle65-0x4
        add(INSTR9, size(INSTR9));
        addConstant(shufle105, false);
        int INSTR10[] = {0xc4, 0xe2, 0x6d, 0x00, 0x15};// 	vpshufb 0x0(%rip),%ymm2,%ymm2        # 47 <FUNC+0x47>	43: R_X86_64_PC32	shufle65-0x4
        add(INSTR10, size(INSTR10));
        addConstant(shufle105, false);
        int INSTR11[] = {0xc4, 0xe2, 0x65, 0x00, 0x1d};// 	vpshufb 0x0(%rip),%ymm3,%ymm3        # 50 <FUNC+0x50>	4c: R_X86_64_PC32	shufle65-0x4
        add(INSTR11, size(INSTR11));
        addConstant(shufle105, false);
        int INSTR12[] = {0xc4, 0xe2, 0x5d, 0x00, 0x25};// 	vpshufb 0x0(%rip),%ymm4,%ymm4        # 59 <FUNC+0x59>	55: R_X86_64_PC32	shufle65-0x4
        add(INSTR12, size(INSTR12));
        addConstant(shufle105, false);
        int INSTR13[] = {0xc4, 0xe2, 0x55, 0x00, 0x2d};// 	vpshufb 0x0(%rip),%ymm5,%ymm5        # 62 <FUNC+0x62>	5e: R_X86_64_PC32	shufle65-0x4
        add(INSTR13, size(INSTR13));
        addConstant(shufle105, false);
        int INSTR14[] = {0xc4, 0xe2, 0x4d, 0x00, 0x35};// 	vpshufb 0x0(%rip),%ymm6,%ymm6        # 6b <FUNC+0x6b>	67: R_X86_64_PC32	shufle65-0x4
        add(INSTR14, size(INSTR14));
        addConstant(shufle105, false);
        int INSTR15[] = {0xc4, 0xe2, 0x45, 0x00, 0x3d};// 	vpshufb 0x0(%rip),%ymm7,%ymm7        # 74 <FUNC+0x74>	70: R_X86_64_PC32	shufle65-0x4
        add(INSTR15, size(INSTR15));
        addConstant(shufle105, false);
    }
}

void Codegen::unpack105(bool isSigned) {
    unpackGen105(isSigned, 105);
}

void Codegen::unpack106(bool isSigned) {
    unpackGen105(isSigned, 106);
}

void Codegen::unpack107(bool isSigned) {
    unpackGen105(isSigned, 107);
}

void Codegen::unpack108(bool isSigned) {
    unpackGen105(isSigned, 108);
}

void Codegen::unpack109(bool isSigned) {
    unpackGen105(isSigned, 109);
}

void Codegen::unpack110(bool isSigned) {
    unpackGen105(isSigned, 110);
}

void Codegen::unpack111(bool isSigned) {
    unpackGen105(isSigned, 111);
}

void Codegen::unpack112(bool isSigned) {
    unpackGen105(isSigned, 112);
}

void Codegen::unpackGen113(bool isSigned, int p) {
    // typedef struct
    //    uint16_t isNull;
    //    uint16_t isEndOfDocument;
    //    BitArray113 values;
    // SM113;
    static __m256i shufle113 = _mm256_set_epi8((char)0x80, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
                                               (char)0x80, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1);
    static __m256i shufle113s = _mm256_set_epi8(14, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0,
                                                15, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1);
    static __m256i signed113 = _mm256_set_epi32(15, 0, 0, 0, 15, 0, 0, 0);
    static __m256i signed114 = _mm256_set_epi32(14, 0, 0, 0, 14, 0, 0, 0);
    static __m256i signed115 = _mm256_set_epi32(13, 0, 0, 0, 13, 0, 0, 0);
    static __m256i signed116 = _mm256_set_epi32(12, 0, 0, 0, 12, 0, 0, 0);
    static __m256i signed117 = _mm256_set_epi32(11, 0, 0, 0, 11, 0, 0, 0);
    static __m256i signed118 = _mm256_set_epi32(10, 0, 0, 0, 10, 0, 0, 0);
    static __m256i signed119 = _mm256_set_epi32(9, 0, 0, 0, 9, 0, 0, 0);
    static __m256i signed120 = _mm256_set_epi32(8, 0, 0, 0, 8, 0, 0, 0);

    __m256i s;
    switch (p) {
        case 113:
            s = signed113;
            break;
        case 114:
            s = signed114;
            break;
        case 115:
            s = signed115;
            break;
        case 116:
            s = signed116;
            break;
        case 117:
            s = signed117;
            break;
        case 118:
            s = signed118;
            break;
        case 119:
            s = signed119;
            break;
        case 120:
            s = signed120;
            break;
        default:
            break;
    }
    loadPointer();

    int INSTR0[] = {0xc5, 0xff, 0xf0, 0x41, 3};//      	vlddqu 3(%rcx),%ymm0
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc5, 0xff, 0xf0, 0x49, 33};//       	vlddqu 33(%rcx),%ymm1
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc5, 0xff, 0xf0, 0x51, 63};//       	vlddqu 63(%rcx),%ymm2
    add(INSTR2, size(INSTR2));
    int INSTR3[] = {0xc5, 0xff, 0xf0, 0x59, 93};//       	vlddqu 93(%rcx),%ymm3
    add(INSTR3, size(INSTR3));
    int INSTR4[] = {0xc5, 0xff, 0xf0, 0x61, 123};//       	vlddqu 123(%rcx),%ymm4
    add(INSTR4, size(INSTR0));
    int INSTR5[] = {0xc5, 0xff, 0xf0, 0xa9, 153,0,0,0};//    vlddqu 153(%rcx),%ymm5
    add(INSTR5, size(INSTR5));
    int INSTR6[] = {0xc5, 0xff, 0xf0, 0xb1, 183,0,0,0};//    vlddqu 183(%rcx),%ymm6
    add(INSTR6, size(INSTR6));
    int INSTR7[] = {0xc5, 0xff, 0xf0, 0xb9, 213,0,0,0};//    vlddqu 213(%rcx),%ymm7
    add(INSTR7, size(INSTR7));

    if (isSigned) {
        int INSTR8[] = {0xc4, 0xe2, 0x7d, 0x00, 0x05};// 	vpshufb 0x0(%rip),%ymm0,%ymm0        # 35 <FUNC+0x35>	31: R_X86_64_PC32	shufle65s-0x4
        add(INSTR8, size(INSTR8));
        addConstant(shufle113s, true);
        int INSTR9[] = {0xc4, 0xe2, 0x75, 0x00, 0x0d};// 	vpshufb 0x0(%rip),%ymm1,%ymm1        # 3e <FUNC+0x3e>	3a: R_X86_64_PC32	shufle65s-0x4
        add(INSTR9, size(INSTR9));
        addConstant(shufle113s, false);
        int INSTR10[] = {0xc4, 0xe2, 0x6d, 0x00, 0x15};// 	vpshufb 0x0(%rip),%ymm2,%ymm2        # 47 <FUNC+0x47>	43: R_X86_64_PC32	shufle65s-0x4
        add(INSTR10, size(INSTR10));
        addConstant(shufle113s, false);
        int INSTR11[] = {0xc4, 0xe2, 0x65, 0x00, 0x1d};// 	vpshufb 0x0(%rip),%ymm3,%ymm3        # 50 <FUNC+0x50>	4c: R_X86_64_PC32	shufle65s-0x4
        add(INSTR11, size(INSTR11));
        addConstant(shufle113s, false);
        int INSTR12[] = {0xc4, 0xe2, 0x5d, 0x00, 0x25};// 	vpshufb 0x0(%rip),%ymm4,%ymm4        # 59 <FUNC+0x59>	55: R_X86_64_PC32	shufle65s-0x4
        add(INSTR12, size(INSTR12));
        addConstant(shufle113s, false);
        int INSTR13[] = {0xc4, 0xe2, 0x55, 0x00, 0x2d};// 	vpshufb 0x0(%rip),%ymm5,%ymm5        # 62 <FUNC+0x62>	5e: R_X86_64_PC32	shufle65s-0x4
        add(INSTR13, size(INSTR13));
        addConstant(shufle113s, false);
        int INSTR14[] = {0xc4, 0xe2, 0x4d, 0x00, 0x35};// 	vpshufb 0x0(%rip),%ymm6,%ymm6        # 6b <FUNC+0x6b>	67: R_X86_64_PC32	shufle65s-0x4
        add(INSTR14, size(INSTR14));
        addConstant(shufle113s, false);
        int INSTR15[] = {0xc4, 0xe2, 0x45, 0x00, 0x3d};// 	vpshufb 0x0(%rip),%ymm7,%ymm7        # 74 <FUNC+0x74>	70: R_X86_64_PC32	shufle65s-0x4
        add(INSTR15, size(INSTR15));
        addConstant(shufle113s, true);
        int INSTR16[] = {0xc4, 0xe2, 0x7d, 0x47, 0x05};// 	vpsllvd 0x0(%rip),%ymm0,%ymm0        # 7d <FUNC+0x7d>	79: R_X86_64_PC32	signed65-0x4
        add(INSTR16, size(INSTR16));
        addConstant(s, true);
        int INSTR17[] = {0xc4, 0xe2, 0x75, 0x47, 0x0d};// 	vpsllvd 0x0(%rip),%ymm1,%ymm1        # 86 <FUNC+0x86>	82: R_X86_64_PC32	signed65-0x4
        add(INSTR17, size(INSTR17));
        addConstant(s, false);
        int INSTR18[] = {0xc4, 0xe2, 0x6d, 0x47, 0x15};// 	vpsllvd 0x0(%rip),%ymm2,%ymm2        # 8f <FUNC+0x8f>	8b: R_X86_64_PC32	signed65-0x4
        add(INSTR18, size(INSTR18));
        addConstant(s, false);
        int INSTR19[] = {0xc4, 0xe2, 0x65, 0x47, 0x1d};// 	vpsllvd 0x0(%rip),%ymm3,%ymm3        # 98 <FUNC+0x98>	94: R_X86_64_PC32	signed65-0x4
        add(INSTR19, size(INSTR19));
        addConstant(s, false);
        int INSTR20[] = {0xc4, 0xe2, 0x5d, 0x47, 0x25};// 	vpsllvd 0x0(%rip),%ymm4,%ymm4        # a1 <FUNC+0xa1>	9d: R_X86_64_PC32	signed65-0x4
        add(INSTR20, size(INSTR20));
        addConstant(s, false);
        int INSTR21[] = {0xc4, 0xe2, 0x55, 0x47, 0x2d};// 	vpsllvd 0x0(%rip),%ymm5,%ymm5        # aa <FUNC+0xaa>	a6: R_X86_64_PC32	signed65-0x4
        add(INSTR21, size(INSTR21));
        addConstant(s, false);
        int INSTR22[] = {0xc4, 0xe2, 0x4d, 0x47, 0x35};// 	vpsllvd 0x0(%rip),%ymm6,%ymm6        # b3 <FUNC+0xb3>	af: R_X86_64_PC32	signed65-0x4
        add(INSTR22, size(INSTR22));
        addConstant(s, false);
        int INSTR23[] = {0xc4, 0xe2, 0x45, 0x47, 0x3d};// 	vpsllvd 0x0(%rip),%ymm7,%ymm7        # bc <FUNC+0xbc>	b8: R_X86_64_PC32	signed65-0x4
        add(INSTR23, size(INSTR23));
        addConstant(s, false);
        int INSTR24[] = {0xc4, 0xe2, 0x7d, 0x46, 0x05};// 	vpsravd 0x0(%rip),%ymm0,%ymm0        # 0xc5 ,0x<FUNC+0xc5>	c1: R_X86_64_PC32	signed65b-0x4
        add(INSTR24, size(INSTR24));
        addConstant(s, true);
        int INSTR25[] = {0xc4, 0xe2, 0x75, 0x46, 0x0d};// 	vpsravd 0x0(%rip),%ymm1,%ymm1        # ce <FUNC+0xce>	ca: R_X86_64_PC32	signed65b-0x4
        add(INSTR25, size(INSTR25));
        addConstant(s, false);
        int INSTR26[] = {0xc4, 0xe2, 0x6d, 0x46, 0x15};// 	vpsravd 0x0(%rip),%ymm2,%ymm2        # d7 <FUNC+0xd7>	d3: R_X86_64_PC32	signed65b-0x4
        add(INSTR26, size(INSTR26));
        addConstant(s, false);
        int INSTR27[] = {0xc4, 0xe2, 0x65, 0x46, 0x1d};// 	vpsravd 0x0(%rip),%ymm3,%ymm3        # e0 <FUNC+0xe0>	dc: R_X86_64_PC32	signed65b-0x4
        add(INSTR27, size(INSTR27));
        addConstant(s, false);
        int INSTR28[] = {0xc4, 0xe2, 0x5d, 0x46, 0x25};// 	vpsravd 0x0(%rip),%ymm4,%ymm4        # e9 <FUNC+0xe9>	e5: R_X86_64_PC32	signed65b-0x4
        add(INSTR28, size(INSTR28));
        addConstant(s, false);
        int INSTR29[] = {0xc4, 0xe2, 0x55, 0x46, 0x2d};// 	vpsravd 0x0(%rip),%ymm5,%ymm5        # f2 <FUNC+0xf2>	ee: R_X86_64_PC32	signed65b-0x4
        add(INSTR29, size(INSTR29));
        addConstant(s, false);
        int INSTR30[] = {0xc4, 0xe2, 0x4d, 0x46, 0x35};// 	vpsravd 0x0(%rip),%ymm6,%ymm6        # fb <FUNC+0xfb>	f7: R_X86_64_PC32	signed65b-0x4
        add(INSTR30, size(INSTR30));
        addConstant(s, false);
        int INSTR31[] = {0xc4, 0xe2, 0x45, 0x46, 0x3d};// 	vpsravd 0x0(%rip),%ymm7,%ymm7        # 104 <FUNC+0x104>	100: R_X86_64_PC32	signed65b-0x4
        add(INSTR31, size(INSTR31));
        addConstant(s, false);
    } else {
        int INSTR8[] = {0xc4, 0xe2, 0x7d, 0x00, 0x05};// 	vpshufb 0x0(%rip),%ymm0,%ymm0        # 35 <FUNC+0x35>	31: R_X86_64_PC32	shufle65-0x4
        add(INSTR8, size(INSTR8));
        addConstant(shufle113, true);
        int INSTR9[] = {0xc4, 0xe2, 0x75, 0x00, 0x0d};// 	vpshufb 0x0(%rip),%ymm1,%ymm1        # 3e <FUNC+0x3e>	3a: R_X86_64_PC32	shufle65-0x4
        add(INSTR9, size(INSTR9));
        addConstant(shufle113, false);
        int INSTR10[] = {0xc4, 0xe2, 0x6d, 0x00, 0x15};// 	vpshufb 0x0(%rip),%ymm2,%ymm2        # 47 <FUNC+0x47>	43: R_X86_64_PC32	shufle65-0x4
        add(INSTR10, size(INSTR10));
        addConstant(shufle113, false);
        int INSTR11[] = {0xc4, 0xe2, 0x65, 0x00, 0x1d};// 	vpshufb 0x0(%rip),%ymm3,%ymm3        # 50 <FUNC+0x50>	4c: R_X86_64_PC32	shufle65-0x4
        add(INSTR11, size(INSTR11));
        addConstant(shufle113, false);
        int INSTR12[] = {0xc4, 0xe2, 0x5d, 0x00, 0x25};// 	vpshufb 0x0(%rip),%ymm4,%ymm4        # 59 <FUNC+0x59>	55: R_X86_64_PC32	shufle65-0x4
        add(INSTR12, size(INSTR12));
        addConstant(shufle113, false);
        int INSTR13[] = {0xc4, 0xe2, 0x55, 0x00, 0x2d};// 	vpshufb 0x0(%rip),%ymm5,%ymm5        # 62 <FUNC+0x62>	5e: R_X86_64_PC32	shufle65-0x4
        add(INSTR13, size(INSTR13));
        addConstant(shufle113, false);
        int INSTR14[] = {0xc4, 0xe2, 0x4d, 0x00, 0x35};// 	vpshufb 0x0(%rip),%ymm6,%ymm6        # 6b <FUNC+0x6b>	67: R_X86_64_PC32	shufle65-0x4
        add(INSTR14, size(INSTR14));
        addConstant(shufle113, false);
        int INSTR15[] = {0xc4, 0xe2, 0x45, 0x00, 0x3d};// 	vpshufb 0x0(%rip),%ymm7,%ymm7        # 74 <FUNC+0x74>	70: R_X86_64_PC32	shufle65-0x4
        add(INSTR15, size(INSTR15));
        addConstant(shufle113, false);
    }
}

void Codegen::unpack113(bool isSigned) {
    unpackGen113(isSigned, 113);
}

void Codegen::unpack114(bool isSigned) {
    unpackGen113(isSigned, 114);
}

void Codegen::unpack115(bool isSigned) {
    unpackGen113(isSigned, 115);
}

void Codegen::unpack116(bool isSigned) {
    unpackGen113(isSigned, 116);
}

void Codegen::unpack117(bool isSigned) {
    unpackGen113(isSigned, 117);
}

void Codegen::unpack118(bool isSigned) {
    unpackGen113(isSigned, 118);
}

void Codegen::unpack119(bool isSigned) {
    unpackGen113(isSigned, 119);
}

void Codegen::unpack120(bool isSigned) {
    unpackGen113(isSigned, 120);
}

void Codegen::unpackGen121(bool isSigned, int p) {
    //typedef struct {
    //    uint16_t isNull;
    //    uint16_t isEndOfDocument;
    //    BitArray121 values;
    // SM121;
    static __m256i signed121 = _mm256_set_epi32(7, 0, 0, 0, 7, 0, 0, 0);
    static __m256i signed122 = _mm256_set_epi32(6, 0, 0, 0, 6, 0, 0, 0);
    static __m256i signed123 = _mm256_set_epi32(5, 0, 0, 0, 5, 0, 0, 0);
    static __m256i signed124 = _mm256_set_epi32(4, 0, 0, 0, 4, 0, 0, 0);
    static __m256i signed125 = _mm256_set_epi32(3, 0, 0, 0, 3, 0, 0, 0);
    static __m256i signed126 = _mm256_set_epi32(2, 0, 0, 0, 2, 0, 0, 0);
    static __m256i signed127 = _mm256_set_epi32(1, 0, 0, 0, 1, 0, 0, 0);

    __m256i s;
    switch (p) {
        case 121:
            s = signed121;
            break;
        case 122:
            s = signed122;
            break;
        case 123:
            s = signed123;
            break;
        case 124:
            s = signed124;
            break;
        case 125:
            s = signed125;
            break;
        case 126:
            s = signed126;
            break;
        case 127:
            s = signed127;
            break;
        default:
            break;
    }
    loadPointer();

    int INSTR0[] = {0xc5, 0xff, 0xf0, 0x41, 4};//      	    vlddqu 4(%rcx),%ymm0
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc5, 0xff, 0xf0, 0x49, 36};//       	vlddqu 36(%rcx),%ymm1
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc5, 0xff, 0xf0, 0x51, 68};//       	vlddqu 68(%rcx),%ymm2
    add(INSTR2, size(INSTR2));
    int INSTR3[] = {0xc5, 0xff, 0xf0, 0x59, 100};//       	vlddqu 100(%rcx),%ymm3
    add(INSTR3, size(INSTR3));
    int INSTR4[] = {0xc5, 0xff, 0xf0, 0xa1, 132,0,0,0};//       	vlddqu 132(%rcx),%ymm4
    add(INSTR4, size(INSTR4));
    int INSTR5[] = {0xc5, 0xff, 0xf0, 0xa9, 164,0,0,0};//       	vlddqu 164(%rcx),%ymm5
    add(INSTR5, size(INSTR5));
    int INSTR6[] = {0xc5, 0xff, 0xf0, 0xb1, 196,0,0,0};//       	vlddqu 196(%rcx),%ymm6
    add(INSTR6, size(INSTR6));
    int INSTR7[] = {0xc5, 0xff, 0xf0, 0xb9, 228,0,0,0};//       	vlddqu 228(%rcx),%ymm7
    add(INSTR7, size(INSTR7));
    if (isSigned) {
        if (p < 128) {
            int INSTR16[] = {0xc4, 0xe2, 0x7d, 0x47, 0x05};// 	vpsllvd 0x0(%rip),%ymm0,%ymm0        # 7d <FUNC+0x7d>	79: R_X86_64_PC32	signed65-0x4
            add(INSTR16, size(INSTR16));
            addConstant(s, true);
            int INSTR17[] = {0xc4, 0xe2, 0x75, 0x47, 0x0d};// 	vpsllvd 0x0(%rip),%ymm1,%ymm1        # 86 <FUNC+0x86>	82: R_X86_64_PC32	signed65-0x4
            add(INSTR17, size(INSTR17));
            addConstant(s, false);
            int INSTR18[] = {0xc4, 0xe2, 0x6d, 0x47, 0x15};// 	vpsllvd 0x0(%rip),%ymm2,%ymm2        # 8f <FUNC+0x8f>	8b: R_X86_64_PC32	signed65-0x4
            add(INSTR18, size(INSTR18));
            addConstant(s, false);
            int INSTR19[] = {0xc4, 0xe2, 0x65, 0x47, 0x1d};// 	vpsllvd 0x0(%rip),%ymm3,%ymm3        # 98 <FUNC+0x98>	94: R_X86_64_PC32	signed65-0x4
            add(INSTR19, size(INSTR19));
            addConstant(s, false);
            int INSTR20[] = {0xc4, 0xe2, 0x5d, 0x47, 0x25};// 	vpsllvd 0x0(%rip),%ymm4,%ymm4        # a1 <FUNC+0xa1>	9d: R_X86_64_PC32	signed65-0x4
            add(INSTR20, size(INSTR20));
            addConstant(s, false);
            int INSTR21[] = {0xc4, 0xe2, 0x55, 0x47, 0x2d};// 	vpsllvd 0x0(%rip),%ymm5,%ymm5        # aa <FUNC+0xaa>	a6: R_X86_64_PC32	signed65-0x4
            add(INSTR21, size(INSTR21));
            addConstant(s, false);
            int INSTR22[] = {0xc4, 0xe2, 0x4d, 0x47, 0x35};// 	vpsllvd 0x0(%rip),%ymm6,%ymm6        # b3 <FUNC+0xb3>	af: R_X86_64_PC32	signed65-0x4
            add(INSTR22, size(INSTR22));
            addConstant(s, false);
            int INSTR23[] = {0xc4, 0xe2, 0x45, 0x47, 0x3d};// 	vpsllvd 0x0(%rip),%ymm7,%ymm7        # bc <FUNC+0xbc>	b8: R_X86_64_PC32	signed65-0x4
            add(INSTR23, size(INSTR23));
            addConstant(s, false);
            int INSTR24[] = {0xc4, 0xe2, 0x7d, 0x46, 0x05};// 	vpsravd 0x0(%rip),%ymm0,%ymm0        # 0xc5 ,0x<FUNC+0xc5>	c1: R_X86_64_PC32	signed65b-0x4
            add(INSTR24, size(INSTR24));
            addConstant(s, true);
            int INSTR25[] = {0xc4, 0xe2, 0x75, 0x46, 0x0d};// 	vpsravd 0x0(%rip),%ymm1,%ymm1        # ce <FUNC+0xce>	ca: R_X86_64_PC32	signed65b-0x4
            add(INSTR25, size(INSTR25));
            addConstant(s, false);
            int INSTR26[] = {0xc4, 0xe2, 0x6d, 0x46, 0x15};// 	vpsravd 0x0(%rip),%ymm2,%ymm2        # d7 <FUNC+0xd7>	d3: R_X86_64_PC32	signed65b-0x4
            add(INSTR26, size(INSTR26));
            addConstant(s, false);
            int INSTR27[] = {0xc4, 0xe2, 0x65, 0x46, 0x1d};// 	vpsravd 0x0(%rip),%ymm3,%ymm3        # e0 <FUNC+0xe0>	dc: R_X86_64_PC32	signed65b-0x4
            add(INSTR27, size(INSTR27));
            addConstant(s, false);
            int INSTR28[] = {0xc4, 0xe2, 0x5d, 0x46, 0x25};// 	vpsravd 0x0(%rip),%ymm4,%ymm4        # e9 <FUNC+0xe9>	e5: R_X86_64_PC32	signed65b-0x4
            add(INSTR28, size(INSTR28));
            addConstant(s, false);
            int INSTR29[] = {0xc4, 0xe2, 0x55, 0x46, 0x2d};// 	vpsravd 0x0(%rip),%ymm5,%ymm5        # f2 <FUNC+0xf2>	ee: R_X86_64_PC32	signed65b-0x4
            add(INSTR29, size(INSTR29));
            addConstant(s, false);
            int INSTR30[] = {0xc4, 0xe2, 0x4d, 0x46, 0x35};// 	vpsravd 0x0(%rip),%ymm6,%ymm6        # fb <FUNC+0xfb>	f7: R_X86_64_PC32	signed65b-0x4
            add(INSTR30, size(INSTR30));
            addConstant(s, false);
            int INSTR31[] = {0xc4, 0xe2, 0x45, 0x46, 0x3d};// 	vpsravd 0x0(%rip),%ymm7,%ymm7        # 104 <FUNC+0x104>	100: R_X86_64_PC32	signed65b-0x4
            add(INSTR31, size(INSTR31));
            addConstant(s, false);
        }
    } else {
        if (p < 128) {
            int INSTR16[] = {0xc4, 0x62, 0x3d, 0x47, 0x05};// 	vpsllvd 0x0(%rip),%ymm8,%ymm8        # 7d <FUNC+0x7d>	79: R_X86_64_PC32	signed65-0x4
            add(INSTR16, size(INSTR16));
            addConstant(s, true);
            int INSTR17[] = {0xc4, 0x62, 0x35, 0x47, 0x0d};// 	vpsllvd 0x0(%rip),%ymm9,%ymm9        # 86 <FUNC+0x86>	82: R_X86_64_PC32	signed65-0x4
            add(INSTR17, size(INSTR17));
            addConstant(s, false);
            int INSTR18[] = {0xc4, 0x62, 0x2d, 0x47, 0x15};// 	vpsllvd 0x0(%rip),%ymm10,%ymm10        # 8f <FUNC+0x8f>	8b: R_X86_64_PC32	signed65-0x4
            add(INSTR18, size(INSTR18));
            addConstant(s, false);
            int INSTR19[] = {0xc4, 0x62, 0x25, 0x47, 0x1d};// 	vpsllvd 0x0(%rip),%ymm11,%ymm11        # 98 <FUNC+0x98>	94: R_X86_64_PC32	signed65-0x4
            add(INSTR19, size(INSTR19));
            addConstant(s, false);
            int INSTR20[] = {0xc4, 0x62, 0x1d, 0x47, 0x25};// 	vpsllvd 0x0(%rip),%ymm12,%ymm12        # a1 <FUNC+0xa1>	9d: R_X86_64_PC32	signed65-0x4
            add(INSTR20, size(INSTR20));
            addConstant(s, false);
            int INSTR21[] = {0xc4, 0x62, 0x15, 0x47, 0x2d};// 	vpsllvd 0x0(%rip),%ymm13,%ymm13        # aa <FUNC+0xaa>	a6: R_X86_64_PC32	signed65-0x4
            add(INSTR21, size(INSTR21));
            addConstant(s, false);
            int INSTR22[] = {0xc4, 0x62, 0x0d, 0x47, 0x35};// 	vpsllvd 0x0(%rip),%ymm14,%ymm14        # b3 <FUNC+0xb3>	af: R_X86_64_PC32	signed65-0x4
            add(INSTR22, size(INSTR22));
            addConstant(s, false);
            int INSTR23[] = {0xc4, 0x62, 0x05, 0x47, 0x3d};// 	vpsllvd 0x0(%rip),%ymm15,%ymm15        # bc <FUNC+0xbc>	b8: R_X86_64_PC32	signed65-0x4
            add(INSTR23, size(INSTR23));
            addConstant(s, false);
            int INSTR24[] = {0xc4, 0x62, 0x3d, 0x46, 0x05};// 	vpsravd 0x0(%rip),%ymm8,%ymm8        # 0xc5 ,0x<FUNC+0xc5>	c1: R_X86_64_PC32	signed65b-0x4
            add(INSTR24, size(INSTR24));
            addConstant(s, true);
            int INSTR25[] = {0xc4, 0x62, 0x35, 0x46, 0x0d};// 	vpsravd 0x0(%rip),%ymm9,%ymm9        # ce <FUNC+0xce>	ca: R_X86_64_PC32	signed65b-0x4
            add(INSTR25, size(INSTR25));
            addConstant(s, false);
            int INSTR26[] = {0xc4, 0x62, 0x2d, 0x46, 0x15};// 	vpsravd 0x0(%rip),%ymm10,%ymm10        # d7 <FUNC+0xd7>	d3: R_X86_64_PC32	signed65b-0x4
            add(INSTR26, size(INSTR26));
            addConstant(s, false);
            int INSTR27[] = {0xc4, 0x62, 0x25, 0x46, 0x1d};// 	vpsravd 0x0(%rip),%ymm11,%ymm11        # e0 <FUNC+0xe0>	dc: R_X86_64_PC32	signed65b-0x4
            add(INSTR27, size(INSTR27));
            addConstant(s, false);
            int INSTR28[] = {0xc4, 0x62, 0x1d, 0x46, 0x25};// 	vpsravd 0x0(%rip),%ymm12,%ymm12        # e9 <FUNC+0xe9>	e5: R_X86_64_PC32	signed65b-0x4
            add(INSTR28, size(INSTR28));
            addConstant(s, false);
            int INSTR29[] = {0xc4, 0x62, 0x15, 0x46, 0x2d};// 	vpsravd 0x0(%rip),%ymm13,%ymm13        # f2 <FUNC+0xf2>	ee: R_X86_64_PC32	signed65b-0x4
            add(INSTR29, size(INSTR29));
            addConstant(s, false);
            int INSTR30[] = {0xc4, 0x62, 0x0d, 0x46, 0x35};// 	vpsravd 0x0(%rip),%ymm14,%ymm14        # fb <FUNC+0xfb>	f7: R_X86_64_PC32	signed65b-0x4
            add(INSTR30, size(INSTR30));
            addConstant(s, false);
            int INSTR31[] = {0xc4, 0x62, 0x05, 0x46, 0x3d};// 	vpsravd 0x0(%rip),%ymm15,%ymm15        # 104 <FUNC+0x104>	100: R_X86_64_PC32	signed65b-0x4
            add(INSTR31, size(INSTR31));
            addConstant(s, false);
        }
    }
}

void Codegen::unpack121(bool isSigned) {
    unpackGen121(isSigned, 121);
}

void Codegen::unpack122(bool isSigned) {
    unpackGen121(isSigned, 122);
}

void Codegen::unpack123(bool isSigned) {
    unpackGen121(isSigned, 123);
}

void Codegen::unpack124(bool isSigned) {
    unpackGen121(isSigned, 124);
}

void Codegen::unpack125(bool isSigned) {
    unpackGen121(isSigned, 125);
}

void Codegen::unpack126(bool isSigned) {
    unpackGen121(isSigned, 126);
}

void Codegen::unpack127(bool isSigned) {
    unpackGen121(isSigned, 127);
}

void Codegen::unpack128(bool isSigned) {
    unpackGen121(isSigned, 128);
}

void Codegen::unpack(int len, bool isSigned) {
    switch (len) {
        case 1:
            unpack1(isSigned);
            break;
        case 2:
            unpack2(isSigned);
            break;
        case 3:
            unpack3(isSigned);
            break;
        case 4:
            unpack4(isSigned);
            break;
        case 5:
            unpack5(isSigned);
            break;
        case 6:
            unpack6(isSigned);
            break;
        case 7:
            unpack7(isSigned);
            break;
        case 8:
            unpack8(isSigned);
            break;
        case 9:
            unpack9(isSigned);
            break;
        case 10:
            unpack10(isSigned);
            break;
        case 11:
            unpack11(isSigned);
            break;
        case 12:
            unpack12(isSigned);
            break;
        case 13:
            unpack13(isSigned);
            break;
        case 14:
            unpack14(isSigned);
            break;
        case 15:
            unpack15(isSigned);
            break;
        case 16:
            unpack16(isSigned);
            break;
        case 17:
            unpack17(isSigned);
            break;
        case 18:
            unpack18(isSigned);
            break;
        case 19:
            unpack19(isSigned);
            break;
        case 20:
            unpack20(isSigned);
            break;
        case 21:
            unpack21(isSigned);
            break;
        case 22:
            unpack22(isSigned);
            break;
        case 23:
            unpack23(isSigned);
            break;
        case 24:
            unpack24(isSigned);
            break;
        case 25:
            unpack25(isSigned);
            break;
        case 26:
            unpack26(isSigned);
            break;
        case 27:
            unpack27(isSigned);
            break;
        case 28:
            unpack28(isSigned);
            break;
        case 29:
            unpack29(isSigned);
            break;
        case 30:
            unpack30(isSigned);
            break;
        case 31:
            unpack31(isSigned);
            break;
        case 32:
            unpack32(isSigned);
            break;
        case 33:
            unpack33(isSigned);
            break;
        case 34:
            unpack34(isSigned);
            break;
        case 35:
            unpack35(isSigned);
            break;
        case 36:
            unpack36(isSigned);
            break;
        case 37:
            unpack37(isSigned);
            break;
        case 38:
            unpack38(isSigned);
            break;
        case 39:
            unpack39(isSigned);
            break;
        case 40:
            unpack40(isSigned);
            break;
        case 41:
            unpack41(isSigned);
            break;
        case 42:
            unpack42(isSigned);
            break;
        case 43:
            unpack43(isSigned);
            break;
        case 44:
            unpack44(isSigned);
            break;
        case 45:
            unpack45(isSigned);
            break;
        case 46:
            unpack46(isSigned);
            break;
        case 47:
            unpack47(isSigned);
            break;
        case 48:
            unpack48(isSigned);
            break;
        case 49:
            unpack49(isSigned);
            break;
        case 50:
            unpack50(isSigned);
            break;
        case 51:
            unpack51(isSigned);
            break;
        case 52:
            unpack52(isSigned);
            break;
        case 53:
            unpack53(isSigned);
            break;
        case 54:
            unpack54(isSigned);
            break;
        case 55:
            unpack55(isSigned);
            break;
        case 56:
            unpack56(isSigned);
            break;
        case 57:
            unpack57(isSigned);
            break;
        case 58:
            unpack58(isSigned);
            break;
        case 59:
            unpack59(isSigned);
            break;
        case 60:
            unpack60(isSigned);
            break;
        case 61:
            unpack61(isSigned);
            break;
        case 62:
            unpack62(isSigned);
            break;
        case 63:
            unpack63(isSigned);
            break;
        case 64:
            unpack64(isSigned);
            break;
        case 65:
            unpack65(isSigned);
            break;
        case 66:
            unpack66(isSigned);
            break;
        case 67:
            unpack67(isSigned);
            break;
        case 68:
            unpack68(isSigned);
            break;
        case 69:
            unpack69(isSigned);
            break;
        case 70:
            unpack70(isSigned);
            break;
        case 71:
            unpack71(isSigned);
            break;
        case 72:
            unpack72(isSigned);
            break;
        case 73:
            unpack73(isSigned);
            break;
        case 74:
            unpack74(isSigned);
            break;
        case 75:
            unpack75(isSigned);
            break;
        case 76:
            unpack76(isSigned);
            break;
        case 77:
            unpack77(isSigned);
            break;
        case 78:
            unpack78(isSigned);
            break;
        case 79:
            unpack79(isSigned);
            break;
        case 80:
            unpack80(isSigned);
            break;
        case 81:
            unpack81(isSigned);
            break;
        case 82:
            unpack82(isSigned);
            break;
        case 83:
            unpack83(isSigned);
            break;
        case 84:
            unpack84(isSigned);
            break;
        case 85:
            unpack85(isSigned);
            break;
        case 86:
            unpack86(isSigned);
            break;
        case 87:
            unpack87(isSigned);
            break;
        case 88:
            unpack88(isSigned);
            break;
        case 89:
            unpack89(isSigned);
            break;
        case 90:
            unpack90(isSigned);
            break;
        case 91:
            unpack91(isSigned);
            break;
        case 92:
            unpack92(isSigned);
            break;
        case 93:
            unpack93(isSigned);
            break;
        case 94:
            unpack94(isSigned);
            break;
        case 95:
            unpack95(isSigned);
            break;
        case 96:
            unpack96(isSigned);
            break;
        case 97:
            unpack97(isSigned);
            break;
        case 98:
            unpack98(isSigned);
            break;
        case 99:
            unpack99(isSigned);
            break;
        case 100:
            unpack100(isSigned);
            break;
        case 101:
            unpack101(isSigned);
            break;
        case 102:
            unpack102(isSigned);
            break;
        case 103:
            unpack103(isSigned);
            break;
        case 104:
            unpack104(isSigned);
            break;
        case 105:
            unpack105(isSigned);
            break;
        case 106:
            unpack106(isSigned);
            break;
        case 107:
            unpack107(isSigned);
            break;
        case 108:
            unpack108(isSigned);
            break;
        case 109:
            unpack109(isSigned);
            break;
        case 110:
            unpack110(isSigned);
            break;
        case 111:
            unpack111(isSigned);
            break;
        case 112:
            unpack112(isSigned);
            break;
        case 113:
            unpack113(isSigned);
            break;
        case 114:
            unpack114(isSigned);
            break;
        case 115:
            unpack115(isSigned);
            break;
        case 116:
            unpack116(isSigned);
            break;
        case 117:
            unpack117(isSigned);
            break;
        case 118:
            unpack118(isSigned);
            break;
        case 119:
            unpack119(isSigned);
            break;
        case 120:
            unpack120(isSigned);
            break;
        case 121:
            unpack121(isSigned);
            break;
        case 122:
            unpack122(isSigned);
            break;
        case 123:
            unpack123(isSigned);
            break;
        case 124:
            unpack124(isSigned);
            break;
        case 125:
            unpack125(isSigned);
            break;
        case 126:
            unpack126(isSigned);
            break;
        case 127:
            unpack127(isSigned);
            break;
        case 128:
            unpack128(isSigned);
            break;
        default:
            Log::error("Unexpected bit length "+to_string(len));
            break;
    }
}

void Codegen::loadConstant32(const __uint128_t &cst,int bank) {
    auto v = (uint32_t)cst;
    int INSTR0[] = {0x48, 0xb8};// 	            movabs    $0x12134,%rax
    add(INSTR0, size(INSTR0));
    addMemoryOffsetL(v);
    int INSTR1[] = {0xc4, 0x61, 0xf9, 0x6e, 0xc0};//    vmovq  %rax,%xmm8
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc4 ,0xc2 ,0x7d ,0x58 ,0xc0};//   	vpbroadcastd %xmm8,%ymm0
    add(INSTR2, size(INSTR2));
    int INSTR3[] = {0xc4 ,0xc2 ,0x7d ,0x58 ,0xc8};//   	vpbroadcastd %xmm8,%ymm1
    add(INSTR3, size(INSTR3));
    save(32,bank);
}

void Codegen::loadConstant64(const __uint128_t &cst,int bank) {
    auto v = (uint64_t)cst;
    int INSTR0[] = {0x48, 0xb8};// 	            movabs    $0x12134,%rax
    add(INSTR0, size(INSTR0));
    addMemoryOffsetL(v);
    int INSTR1[] = {0xc4, 0x61, 0xf9, 0x6e, 0xc0};//    vmovq  %rax,%xmm8
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc4 ,0xc2 ,0x7d ,0x59 ,0xc0};//   	vpbroadcastq %xmm8,%ymm0
    add(INSTR2, size(INSTR2));
    int INSTR3[] = {0xc5, 0xfd, 0x7f, 0x86};//         	vmovdqa %ymm0,(%rsi)
    add(INSTR3, size(INSTR3));
    addMemoryOffset(bank, 0);
    add(INSTR3, size(INSTR3));
    addMemoryOffset(bank, 1);
    add(INSTR3, size(INSTR3));
    addMemoryOffset(bank, 2);
    add(INSTR3, size(INSTR3));
    addMemoryOffset(bank, 3);
}

void Codegen::loadConstant128(const __uint128_t &cst,int bank) {
    auto vl = (uint64_t)cst;
    auto vh = (uint64_t)(cst>>64);

    int INSTR0[] = {0x48, 0xb8};// 	            movabs    $0x12134,%rax
    add(INSTR0, size(INSTR0));
    addMemoryOffsetL(vl);
    int INSTR1[] = {0x48, 0xb9};// 	            movabs    $0x12134,%rcx
    add(INSTR1, size(INSTR1));
    addMemoryOffsetL(vh);
    int INSTR2[] = {0x48 ,0x89 ,0x4c ,0x24 ,0xf8};//       	mov    %rcx,-0x8(%rsp)
    add(INSTR2, size(INSTR2));
    int INSTR3[] = {0x48 ,0x89 ,0x44 ,0x24 ,0xf0};//       	mov    %rax,-0x10(%rsp)
    add(INSTR3, size(INSTR3));
    int INSTR4[] = {0x48 ,0x89 ,0x4c ,0x24 ,0xe8};//       	mov    %rcx,-0x18(%rsp)
    add(INSTR4, size(INSTR4));
    int INSTR5[] = {0x48 ,0x89 ,0x44 ,0x24 ,0xe0};//       	mov    %rax,-0x20(%rsp)
    add(INSTR5, size(INSTR5));
    int INSTR6[] = {0xc5 ,0xff ,0xf0 ,0x44 ,0x24 ,0xe0};//    	vlddqu -0x20(%rsp),%ymm0
    add(INSTR6, size(INSTR6));
    int INSTR7[] = {0xc5 ,0xff ,0xf0 ,0x4c ,0x24 ,0xe0};//    	vlddqu -0x20(%rsp),%ymm1
    add(INSTR7, size(INSTR7));
    int INSTR8[] = {0xc5 ,0xff ,0xf0 ,0x54 ,0x24 ,0xe0};//    	vlddqu -0x20(%rsp),%ymm2
    add(INSTR8, size(INSTR8));
    int INSTR9[] = {0xc5 ,0xff ,0xf0 ,0x5c ,0x24 ,0xe0};//    	vlddqu -0x20(%rsp),%ymm3
    add(INSTR9, size(INSTR9));
    int INSTR10[] = {0xc5 ,0xff ,0xf0 ,0x64 ,0x24 ,0xe0};//    	vlddqu -0x20(%rsp),%ymm4
    add(INSTR10, size(INSTR10));
    int INSTR11[] = {0xc5 ,0xff ,0xf0 ,0x6c ,0x24 ,0xe0};//    	vlddqu -0x20(%rsp),%ymm5
    add(INSTR11, size(INSTR11));
    int INSTR12[] = {0xc5 ,0xff ,0xf0 ,0x74 ,0x24 ,0xe0};//    	vlddqu -0x20(%rsp),%ymm6
    add(INSTR12, size(INSTR12));
    int INSTR13[] = {0xc5 ,0xff ,0xf0 ,0x7c ,0x24 ,0xe0};//    	vlddqu -0x20(%rsp),%ymm7
    add(INSTR13, size(INSTR13));
    save(128,bank);
}

void Codegen::loadConstantF16(const __uint128_t &cst,int bank) {
    auto h = halfFloatToFloat(cst);
    auto v = *(uint32_t*)&h;

    int INSTR0[] = {0x48, 0xb8};// 	            movabs    $0x12134,%rax
    add(INSTR0, size(INSTR0));
    addMemoryOffsetL(v);
    int INSTR1[] = {0xc4, 0x61, 0xf9, 0x6e, 0xc0};//    vmovq  %rax,%xmm8
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc4 ,0xc2 ,0x7d ,0x58 ,0xc0};//       	vpbroadcastd %xmm8,%ymm0
    add(INSTR2, size(INSTR2));
    int INSTR3[] = {0xc4 ,0xc2 ,0x7d ,0x58 ,0xc8};//       	vpbroadcastd %xmm8,%ymm1
    add(INSTR3, size(INSTR3));
    save(32,bank);
}

void Codegen::addMemoryOffset(int bank, int regIndex) {
    uint64_t offset = (bank * 8 + regIndex) * 32;
    addMemoryOffset(offset);
}

void Codegen::addMemoryOffset32(int bank, int regIndex) {
    uint64_t offset = bank * 8 * 32 + regIndex * 4;
    addMemoryOffset(offset);
}

void Codegen::addMemoryOffset64(int bank, int regIndex) {
    uint64_t offset = bank * 8 * 32 + regIndex * 8;
    addMemoryOffset(offset);
}

void Codegen::addMemoryOffset128(int bank, int regIndex) {
    uint64_t offset = bank * 8 * 32 + regIndex * 16;
    addMemoryOffset(offset);
}

void Codegen::save(int len, int bank) {
    if (len <= 32) {
        int INSTR0[] = {0xc5, 0xfd, 0x7f, 0x86};//         	vmovdqa %ymm0,(%rsi)
        add(INSTR0, size(INSTR0));
        addMemoryOffset(bank, 0);
        int INSTR1[] = {0xc5, 0xfd, 0x7f, 0x8e};//       	vmovdqa %ymm1,0x20(%rsi)
        add(INSTR1, size(INSTR1));
        addMemoryOffset(bank, 1);
    } else if (len <= 64) {
        int INSTR0[] = {0xc5, 0xfd, 0x7f, 0x86};//         	vmovdqa %ymm0,(%rsi)
        add(INSTR0, size(INSTR0));
        addMemoryOffset(bank, 0);
        int INSTR1[] = {0xc5, 0xfd, 0x7f, 0x8e};//       	vmovdqa %ymm1,0x20(%rsi)
        add(INSTR1, size(INSTR1));
        addMemoryOffset(bank, 1);
        int INSTR2[] = {0xc5, 0xfd, 0x7f, 0x96};//       	vmovdqa %ymm2,0x40(%rsi)
        add(INSTR2, size(INSTR2));
        addMemoryOffset(bank, 2);
        int INSTR3[] = {0xc5, 0xfd, 0x7f, 0x9e};//       	vmovdqa %ymm3,0x60(%rsi)
        add(INSTR3, size(INSTR3));
        addMemoryOffset(bank, 3);
    } else {
        int INSTR0[] = {0xc5, 0xfd, 0x7f, 0x86};//         	vmovdqa %ymm0,(%rsi)
        add(INSTR0, size(INSTR0));
        addMemoryOffset(bank, 0);
        int INSTR1[] = {0xc5, 0xfd, 0x7f, 0x8e};//       	vmovdqa %ymm1,0x20(%rsi)
        add(INSTR1, size(INSTR1));
        addMemoryOffset(bank, 1);
        int INSTR2[] = {0xc5, 0xfd, 0x7f, 0x96};//       	vmovdqa %ymm2,0x40(%rsi)
        add(INSTR2, size(INSTR2));
        addMemoryOffset(bank, 2);
        int INSTR3[] = {0xc5, 0xfd, 0x7f, 0x9e};//       	vmovdqa %ymm3,0x60(%rsi)
        add(INSTR3, size(INSTR3));
        addMemoryOffset(bank, 3);
        int INSTR4[] = {0xc5, 0xfd, 0x7f, 0xa6};//         	vmovdqa %ymm4,(%rsi)
        add(INSTR4, size(INSTR4));
        addMemoryOffset(bank, 4);
        int INSTR5[] = {0xc5, 0xfd, 0x7f, 0xae};//       	vmovdqa %ymm5,0x20(%rsi)
        add(INSTR5, size(INSTR5));
        addMemoryOffset(bank, 5);
        int INSTR6[] = {0xc5, 0xfd, 0x7f, 0xb6};//       	vmovdqa %ymm6,0x40(%rsi)
        add(INSTR6, size(INSTR6));
        addMemoryOffset(bank, 6);
        int INSTR7[] = {0xc5, 0xfd, 0x7f, 0xbe};//       	vmovdqa %ymm7,0x60(%rsi)
        add(INSTR7, size(INSTR7));
        addMemoryOffset(bank, 7);
    }
}

void Codegen::load_src32(int bank1) {
    int INSTR0[] = {0xc5, 0xfd, 0x6f, 0x86};//          	vmovdqa x(%rsi),%ymm0
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank1, 0);
    int INSTR1[] = {0xc5, 0xfd, 0x6f, 0x8e};//          	vmovdqa x(%rsi),%ymm1
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank1, 1);
}

// save in bank1
void Codegen::mulf_mm(int bank1, int bank2) {
    load_src32(bank1);
    int INSTR2[] = {0xc5, 0xfc, 0x59, 0x86};//          	vmulps x(%rsi),%ymm0,%ymm0
    add(INSTR2, size(INSTR2));
    addMemoryOffset(bank2, 0);
    int INSTR3[] = {0xc5, 0xf4, 0x59, 0x8e};//          	vmulps x(%rsi),%ymm0,%ymm0
    add(INSTR3, size(INSTR3));
    addMemoryOffset(bank2, 1);
    save(32,bank1);
}

// save in bank1
void Codegen::divf_mm(int bank1, int bank2) {
    load_src32(bank1);
    int INSTR2[] = {0xc5, 0xfc, 0x5e, 0x86};//          	vdivps x(%rsi),%ymm0,%ymm0
    add(INSTR2, size(INSTR2));
    addMemoryOffset(bank2, 0);
    int INSTR3[] = {0xc5, 0xf4, 0x5e, 0x8e};//          	vdivps x(%rsi),%ymm1,%ymm1
    add(INSTR3, size(INSTR3));
    addMemoryOffset(bank2, 1);
    save(32,bank1);
}

void Codegen::addf_mm(int bank1, int bank2) {
    load_src32(bank1);
    int INSTR2[] = {0xc5, 0xfc, 0x58, 0x86};//          	vaddps x(%rsi),%ymm0,%ymm0
    add(INSTR2, size(INSTR2));
    addMemoryOffset(bank2, 0);
    int INSTR3[] = {0xc5, 0xf4, 0x58, 0x8e};//          	vaddps x(%rsi),%ymm0,%ymm0
    add(INSTR3, size(INSTR3));
    addMemoryOffset(bank2, 1);
    save(32,bank1);
}

void Codegen::subf_mm(int bank1, int bank2) {
    load_src32(bank1);
    int INSTR2[] = {0xc5, 0xfc, 0x5c, 0x86};//          	vsubps x(%rsi),%ymm0,%ymm0
    add(INSTR2, size(INSTR2));
    addMemoryOffset(bank2, 0);
    int INSTR3[] = {0xc5, 0xf4, 0x5c, 0x8e};//          	vsubps x(%rsi),%ymm0,%ymm0
    add(INSTR3, size(INSTR3));
    addMemoryOffset(bank2, 1);
    save(32,bank1);
}

void Codegen::minf_mm(int bank1, int bank2) {
    load_src32(bank1);
    int INSTR0[] = {0xc5 ,0xfc ,0x5d ,0x86};//  	vminps 0x1246(%rsi),%ymm0,%ymm0
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank2, 0);
    int INSTR1[] = {0xc5 ,0xf4 ,0x5d ,0x8e};// 2 	vminps 0x1246(%rsi),%ymm1,%ymm1
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank2, 1);
    save(32,bank1);
}

void Codegen::maxf_mm(int bank1, int bank2) {
    load_src32(bank1);
    int INSTR0[] = {0xc5 ,0xfc ,0x5f ,0x86};//  	vminps 0x1246(%rsi),%ymm0,%ymm0
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank2, 0);
    int INSTR1[] = {0xc5 ,0xf4 ,0x5f ,0x8e};// 2 	vminps 0x1246(%rsi),%ymm1,%ymm1
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank2, 1);
    save(32,bank1);
}

void Codegen::merge_null() {
    int INSTR0[] = {0x48 ,0x8b ,0x0f};//             	mov    (%rdi),%rcx
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0x48 ,0x0f ,0xb7 ,0x09};//       	movzwq (%rcx),%rcx
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc4 ,0xe2 ,0xf0 ,0xf2 ,0xc0};//   	andn   %rax,%rcx,%rax
    add(INSTR2, size(INSTR2));
}

void Codegen::merge_cmp32() {
    int INSTR0[] = {0xc5 ,0xfd ,0x6b ,0xc1};//       	vpackssdw %ymm0,%ymm1,%ymm0
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc4 ,0xe3 ,0xfd ,0x01 ,0xc0 ,0xd8};// 	vpermpd $0b01101100,%ymm0,%ymm0
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc4 ,0xe3 ,0x7d ,0x06 ,0xc8 ,0x11};//  vperm2f128 $0x11,%ymm0,%ymm0,%ymm1
    add(INSTR2, size(INSTR2));
    int INSTR3[] = {0xc5 ,0xfd ,0x63 ,0xc1};//          	vpacksswb %ymm1,%ymm0,%ymm0
    add(INSTR3, size(INSTR3));
    int INSTR4[] = {0xc5 ,0xfd ,0xd7 ,0xc0};//          	vpmovmskb %ymm0,%eax
    add(INSTR4, size(INSTR4));
}

void Codegen::cmpeqf_mm(int bank1, int bank2) {
    load_src32(bank1);
    int INSTR0[] = {0xc5, 0xfc, 0xc2, 0x86};//          	vcmpps_eq x(%rsi),%ymm0,%ymm0
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank2, 0);
    mem[cursor++] = 0x10;
    int INSTR1[] = {0xc5, 0xf4, 0xc2, 0x8e};//          	vcmpps_eq x(%rsi),%ymm0,%ymm0
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank2, 1);
    mem[cursor++] = 0x10;
    merge_cmp32();
}

void Codegen::cmpnef_mm(int bank1, int bank2) {
    load_src32(bank1);
    int INSTR0[] = {0xc5, 0xfc, 0xc2, 0x86};//          	vcmpps_ne x(%rsi),%ymm0,%ymm0
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank2, 0);
    mem[cursor++] = 20;
    int INSTR1[] = {0xc5, 0xf4, 0xc2, 0x8e};//          	vcmpps_ne x(%rsi),%ymm1,%ymm1
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank2, 1);
    mem[cursor++] = 20;
    merge_cmp32();
}

void Codegen::cmplef_mm(int bank1, int bank2) {
    load_src32(bank1);
    int INSTR0[] = {0xc5, 0xfc, 0xc2, 0x86};//          	vcmple_oqps x(%rsi),%ymm0,%ymm0
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank2, 0);
    mem[cursor++] = 0x12;
    int INSTR1[] = {0xc5, 0xf4, 0xc2, 0x8e};//          	vcmple_oqps x(%rsi),%ymm0,%ymm0
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank2, 1);
    mem[cursor++] = 0x12;
    merge_cmp32();
}

void Codegen::cmpltf_mm(int bank1, int bank2) {
    load_src32(bank1);
    int INSTR0[] = {0xc5, 0xfc, 0xc2, 0x86};//          	vcmple_oqps x(%rsi),%ymm0,%ymm0
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank2, 0);
    mem[cursor++] = 0x11;
    int INSTR1[] = {0xc5, 0xf4, 0xc2, 0x8e};//          	vcmple_oqps x(%rsi),%ymm0,%ymm0
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank2, 1);
    mem[cursor++] = 0x11;
    merge_cmp32();
}

void Codegen::cmpgtf_mm(int bank1, int bank2) {
    load_src32(bank1);
    int INSTR0[] = {0xc5, 0xfc, 0xc2, 0x86};//          	vcmpgt_oqps x(%rsi),%ymm0,%ymm0
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank2, 0);
    mem[cursor++] = 0x1e;
    int INSTR1[] = {0xc5, 0xf4, 0xc2, 0x8e};//          	vcmpgt_oqps x(%rsi),%ymm0,%ymm0
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank2, 1);
    mem[cursor++] = 0x1e;
    merge_cmp32();
}

void Codegen::cmpgef_mm(int bank1, int bank2) {
    load_src32(bank1);
    int INSTR0[] = {0xc5, 0xfc, 0xc2, 0x86};//          	vcmpge_oqps x(%rsi),%ymm0,%ymm0
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank2, 0);
    mem[cursor++] = 0xd;
    int INSTR1[] = {0xc5, 0xf4, 0xc2, 0x8e};//          	vcmpge_oqps x(%rsi),%ymm0,%ymm0
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank2, 1);
    mem[cursor++] = 0xd;
    merge_cmp32();
}

void Codegen::load_src64(int bank1) {
    int INSTR0[] = {0xc5, 0xfd, 0x6f, 0x86};//          	vmovdqa x(%rsi),%ymm0
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank1, 0);
    int INSTR1[] = {0xc5, 0xfd, 0x6f, 0x8e};//          	vmovdqa x(%rsi),%ymm1
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank1, 1);
    int INSTR2[] = {0xc5, 0xfd, 0x6f, 0x96};//          	vmovdqa x(%rsi),%ymm2
    add(INSTR2, size(INSTR2));
    addMemoryOffset(bank1, 2);
    int INSTR3[] = {0xc5, 0xfd, 0x6f, 0x9e};//          	vmovdqa x(%rsi),%ymm3
    add(INSTR3, size(INSTR3));
    addMemoryOffset(bank1, 3);
}

void Codegen::load_src128(int bank1) {
    int INSTR0[] = {0xc5, 0xfd, 0x6f, 0x86};//          	vmovdqa x(%rsi),%ymm0
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank1, 0);
    int INSTR1[] = {0xc5, 0xfd, 0x6f, 0x8e};//          	vmovdqa x(%rsi),%ymm1
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank1, 1);
    int INSTR2[] = {0xc5, 0xfd, 0x6f, 0x96};//          	vmovdqa x(%rsi),%ymm2
    add(INSTR2, size(INSTR2));
    addMemoryOffset(bank1, 2);
    int INSTR3[] = {0xc5, 0xfd, 0x6f, 0x9e};//          	vmovdqa x(%rsi),%ymm3
    add(INSTR3, size(INSTR3));
    addMemoryOffset(bank1, 3);
    int INSTR4[] = {0xc5, 0xfd, 0x6f, 0xa6};//          	vmovdqa x(%rsi),%ymm4
    add(INSTR4, size(INSTR4));
    addMemoryOffset(bank1, 4);
    int INSTR5[] = {0xc5, 0xfd, 0x6f, 0xae};//          	vmovdqa x(%rsi),%ymm5
    add(INSTR5, size(INSTR5));
    addMemoryOffset(bank1, 5);
    int INSTR6[] = {0xc5, 0xfd, 0x6f, 0xb6};//          	vmovdqa x(%rsi),%ymm6
    add(INSTR6, size(INSTR6));
    addMemoryOffset(bank1, 6);
    int INSTR7[] = {0xc5, 0xfd, 0x6f, 0xbe};//          	vmovdqa x(%rsi),%ymm7
    add(INSTR7, size(INSTR7));
    addMemoryOffset(bank1, 7);
}

void Codegen::muld_mm(int bank1, int bank2) {
    load_src64(bank1);
    int INSTR4[] = {0xc5, 0xfd, 0x59, 0x86};//          	vmulpd x(%rsi),%ymm0,%ymm0
    add(INSTR4, size(INSTR4));
    addMemoryOffset(bank2, 0);
    int INSTR5[] = {0xc5, 0xf5, 0x59, 0x8e};//          	vmulpd x(%rsi),%ymm1,%ymm1
    add(INSTR5, size(INSTR5));
    addMemoryOffset(bank2, 1);
    int INSTR6[] = {0xc5, 0xed, 0x59, 0x96};//          	vmulpd x(%rsi),%ymm2,%ymm2
    add(INSTR6, size(INSTR6));
    addMemoryOffset(bank2, 2);
    int INSTR7[] = {0xc5, 0xe5, 0x59, 0x9e};//          	vmulpd x(%rsi),%ymm3,%ymm3
    add(INSTR7, size(INSTR7));
    addMemoryOffset(bank2, 3);
    save(64,bank1);
}

void Codegen::divd_mm(int bank1, int bank2) {
    load_src64(bank1);
    int INSTR4[] = {0xc5, 0xfd, 0x5e, 0x86};//          	vdivpd x(%rsi),%ymm0,%ymm0
    add(INSTR4, size(INSTR4));
    addMemoryOffset(bank2, 0);
    int INSTR5[] = {0xc5, 0xf5, 0x5e, 0x8e};//          	vdivpd x(%rsi),%ymm1,%ymm1
    add(INSTR5, size(INSTR5));
    addMemoryOffset(bank2, 1);
    int INSTR6[] = {0xc5, 0xed, 0x5e, 0x96};//          	vdivpd x(%rsi),%ymm2,%ymm2
    add(INSTR6, size(INSTR6));
    addMemoryOffset(bank2, 2);
    int INSTR7[] = {0xc5, 0xe5, 0x5e, 0x9e};//          	vdivpd x(%rsi),%ymm3,%ymm3
    add(INSTR7, size(INSTR7));
    addMemoryOffset(bank2, 3);
    save(64,bank1);
}

void Codegen::addd_mm(int bank1, int bank2) {
    load_src64(bank1);
    int INSTR4[] = {0xc5, 0xfd, 0x58, 0x86};//          	vaddpd x(%rsi),%ymm0,%ymm0
    add(INSTR4, size(INSTR4));
    addMemoryOffset(bank2, 0);
    int INSTR5[] = {0xc5, 0xf5, 0x58, 0x8e};//          	vaddpd x(%rsi),%ymm1,%ymm1
    add(INSTR5, size(INSTR5));
    addMemoryOffset(bank2, 1);
    int INSTR6[] = {0xc5, 0xed, 0x58, 0x96};//          	vaddpd x(%rsi),%ymm2,%ymm2
    add(INSTR6, size(INSTR6));
    addMemoryOffset(bank2, 2);
    int INSTR7[] = {0xc5, 0xe5, 0x58, 0x9e};//          	vaddpd x(%rsi),%ymm3,%ymm3
    add(INSTR7, size(INSTR7));
    addMemoryOffset(bank2, 3);
    save(64,bank1);
}

void Codegen::subd_mm(int bank1, int bank2) {
    load_src64(bank1);
    int INSTR4[] = {0xc5, 0xfd, 0x5c, 0x86};//          	vsubpd x(%rsi),%ymm0,%ymm0
    add(INSTR4, size(INSTR4));
    addMemoryOffset(bank2, 0);
    int INSTR5[] = {0xc5, 0xf5, 0x5c, 0x8e};//          	vsubpd x(%rsi),%ymm1,%ymm1
    add(INSTR5, size(INSTR5));
    addMemoryOffset(bank2, 1);
    int INSTR6[] = {0xc5, 0xed, 0x5c, 0x96};//          	vsubpd x(%rsi),%ymm2,%ymm2
    add(INSTR6, size(INSTR6));
    addMemoryOffset(bank2, 2);
    int INSTR7[] = {0xc5, 0xe5, 0x5c, 0x9e};//          	vsubpd x(%rsi),%ymm3,%ymm3
    add(INSTR7, size(INSTR7));
    addMemoryOffset(bank2, 3);
    save(64,bank1);
}

void Codegen::mind_mm(int bank1, int bank2) {
    load_src64(bank1);
    int INSTR0[] = {0xc5 ,0xfd ,0x5d ,0x86};//  	vminpd 0x1246(%rsi),%ymm0,%ymm0
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank2, 0);
    int INSTR1[] = {0xc5 ,0xf5 ,0x5d ,0x8e};// 2 	vminps 0x1246(%rsi),%ymm1,%ymm1
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank2, 1);
    int INSTR2[] = {0xc5 ,0xed ,0x5d ,0x96};//  	vminpd 0x1246(%rsi),%ymm0,%ymm0
    add(INSTR2, size(INSTR2));
    addMemoryOffset(bank2, 2);
    int INSTR3[] = {0xc5 ,0xe5 ,0x5d ,0x9e};// 2 	vminps 0x1246(%rsi),%ymm1,%ymm1
    add(INSTR3, size(INSTR3));
    addMemoryOffset(bank2, 3);
    save(64,bank1);
}

void Codegen::maxd_mm(int bank1, int bank2) {
    load_src64(bank1);
    int INSTR0[] = {0xc5 ,0xfd ,0x5f ,0x86};//  	vmaxpd 0x1246(%rsi),%ymm0,%ymm0
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank2, 0);
    int INSTR1[] = {0xc5 ,0xf5 ,0x5f ,0x8e};// 2 	vmaxps 0x1246(%rsi),%ymm1,%ymm1
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank2, 1);
    int INSTR2[] = {0xc5 ,0xed ,0x5f ,0x96};//  	vmaxpd 0x1246(%rsi),%ymm0,%ymm0
    add(INSTR2, size(INSTR2));
    addMemoryOffset(bank2, 2);
    int INSTR3[] = {0xc5 ,0xe5 ,0x5f ,0x9e};// 2 	vmaxps 0x1246(%rsi),%ymm1,%ymm1
    add(INSTR3, size(INSTR3));
    addMemoryOffset(bank2, 3);
    save(64,bank1);
}

void Codegen::merge_cmp64() {
    static __m256i perm1 = _mm256_set_epi8((char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x08,(char)0x00,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x08,(char)0x00);
    static __m256i perm2 = _mm256_set_epi8((char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x08,(char)0x00,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x08,(char)0x00,(char)0x80,(char)0x80,(char)0x80,(char)0x80);
    static __m256i perm3 = _mm256_set_epi8((char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x08,(char)0x00,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x08,(char)0x00,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80);
    static __m256i perm4 = _mm256_set_epi8((char)0x08,(char)0x00,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x08,(char)0x00,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80,(char)0x80);

    int INSTR0[] = {0xc4 ,0xe2 ,0x7d ,0x00 ,0x05};// 	vpshufb 0x23131(%rip),%ymm0,%ymm0        # 0x2313a
    add(INSTR0, size(INSTR0));
    addConstant(perm1,true);
    int INSTR1[] = {0xc4 ,0xe2 ,0x75 ,0x00 ,0x0d};// 	vpshufb 0x23131(%rip),%ymm1,%ymm1        # 0x23143
    add(INSTR1, size(INSTR1));
    addConstant(perm2,true);
    int INSTR2[] = {0xc4 ,0xe2 ,0x6d ,0x00 ,0x15};// 	vpshufb 0x23131(%rip),%ymm2,%ymm2        # 0x2314c
    add(INSTR2, size(INSTR2));
    addConstant(perm3,true);
    int INSTR3[] = {0xc4 ,0xe2 ,0x65 ,0x00 ,0x1d};// 	vpshufb 0x23131(%rip),%ymm3,%ymm3        # 0x23155
    add(INSTR3, size(INSTR3));
    addConstant(perm4,true);
    int INSTR4[] = {0xc5 ,0xf5 ,0xeb ,0xc0};//      	vpor   %ymm0,%ymm1,%ymm0
    add(INSTR4, size(INSTR4));
    int INSTR5[] = {0xc5 ,0xe5 ,0xeb ,0xd2};//      	vpor   %ymm2,%ymm3,%ymm2
    add(INSTR5, size(INSTR5));
    int INSTR6[] = {0xc5 ,0xed ,0xeb ,0xc0};//      	vpor   %ymm0,%ymm2,%ymm0
    add(INSTR6, size(INSTR6));
    int INSTR7[] = {0xc4 ,0xe3 ,0x7d ,0x06 ,0xc8 ,0x11};//	vperm2f128 $0x11,%ymm0,%ymm0,%ymm1
    add(INSTR7, size(INSTR7));
    int INSTR8[] = {0xc5 ,0xf5 ,0xeb ,0xc0};//      	vpor   %ymm0,%ymm1,%ymm0
    add(INSTR8, size(INSTR8));
    int INSTR9[] = {0xc5 ,0xfd ,0xd7 ,0xc0};//          vpmovmskb %ymm0,%eax
    add(INSTR9, size(INSTR9));
}

void Codegen::cmpeqd_mm(int bank1, int bank2) {
    load_src64(bank1);
    int INSTR4[] = {0xc5, 0xfd, 0xc2, 0x86};//          	vcmpeq_ospd 0x12345(%rsi),%ymm0,%ymm0
    add(INSTR4, size(INSTR4));
    addMemoryOffset(bank2, 0);
    mem[cursor++] = 16;
    int INSTR5[] = {0xc5, 0xf5, 0xc2, 0x8e};//          	vcmpeq_ospd 0x12345(%rsi),%ymm1,%ymm1
    add(INSTR5, size(INSTR5));
    addMemoryOffset(bank2, 1);
    mem[cursor++] = 16;
    int INSTR6[] = {0xc5, 0xed, 0xc2, 0x96};//          	vcmpeq_ospd 0x12345(%rsi),%ymm2,%ymm2
    add(INSTR6, size(INSTR6));
    addMemoryOffset(bank2, 2);
    mem[cursor++] = 16;
    int INSTR7[] = {0xc5, 0xe5, 0xc2, 0x9e};//          	vcmpeq_ospd 0x12345(%rsi),%ymm3,%ymm3
    add(INSTR7, size(INSTR7));
    addMemoryOffset(bank2, 3);
    mem[cursor++] = 16;
    merge_cmp64();
}

void Codegen::cmpned_mm(int bank1, int bank2) {
    load_src64(bank1);
    int INSTR4[] = {0xc5, 0xfd, 0xc2, 0x86};//          	vcmpeq_ospd 0x12345(%rsi),%ymm0,%ymm0
    add(INSTR4, size(INSTR4));
    addMemoryOffset(bank2, 0);
    mem[cursor++] = 20;
    int INSTR5[] = {0xc5, 0xf5, 0xc2, 0x8e};//          	vcmpeq_ospd 0x12345(%rsi),%ymm1,%ymm1
    add(INSTR5, size(INSTR5));
    addMemoryOffset(bank2, 1);
    mem[cursor++] = 20;
    int INSTR6[] = {0xc5, 0xed, 0xc2, 0x96};//          	vcmpeq_ospd 0x12345(%rsi),%ymm2,%ymm2
    add(INSTR6, size(INSTR6));
    addMemoryOffset(bank2, 2);
    mem[cursor++] = 20;
    int INSTR7[] = {0xc5, 0xe5, 0xc2, 0x9e};//          	vcmpeq_ospd 0x12345(%rsi),%ymm3,%ymm3
    add(INSTR7, size(INSTR7));
    addMemoryOffset(bank2, 3);
    mem[cursor++] = 20;
    merge_cmp64();
}

void Codegen::cmpled_mm(int bank1, int bank2) {
    load_src64(bank1);
    int INSTR0[] = {0xc5, 0xfd, 0xc2, 0x86};//          	vcmple_oqpd x(%rsi),%ymm0,%ymm0
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank2, 0);
    mem[cursor++] = 0x12;
    int INSTR1[] = {0xc5, 0xf5, 0xc2, 0x8e};//          	vcmple_oqpd x(%rsi),%ymm1,%ymm1
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank2, 1);
    mem[cursor++] = 0x12;
    int INSTR2[] = {0xc5, 0xed, 0xc2, 0x96};//          	vcmple_oqpd x(%rsi),%ymm2,%ymm2
    add(INSTR2, size(INSTR2));
    addMemoryOffset(bank2, 2);
    mem[cursor++] = 0x12;
    int INSTR3[] = {0xc5, 0xe5, 0xc2, 0x9e};//          	vcmple_oqpd x(%rsi),%ymm3,%ymm3
    add(INSTR3, size(INSTR3));
    addMemoryOffset(bank2, 3);
    mem[cursor++] = 0x12;
    merge_cmp64();
}

void Codegen::cmpltd_mm(int bank1, int bank2) {
    load_src64(bank1);
    int INSTR0[] = {0xc5, 0xfd, 0xc2, 0x86};//          	vcmplt_oqpd x(%rsi),%ymm0,%ymm0
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank2, 0);
    mem[cursor++] = 0x11;
    int INSTR1[] = {0xc5, 0xf5, 0xc2, 0x8e};//          	vcmplt_oqpd x(%rsi),%ymm1,%ymm1
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank2, 1);
    mem[cursor++] = 0x11;
    int INSTR2[] = {0xc5, 0xed, 0xc2, 0x96};//          	vcmplt_oqpd x(%rsi),%ymm2,%ymm2
    add(INSTR2, size(INSTR2));
    addMemoryOffset(bank2, 2);
    mem[cursor++] = 0x11;
    int INSTR3[] = {0xc5, 0xe5, 0xc2, 0x9e};//          	vcmplt_oqpd x(%rsi),%ymm3,%ymm3
    add(INSTR3, size(INSTR3));
    addMemoryOffset(bank2, 3);
    mem[cursor++] = 0x11;
    merge_cmp64();
}

void Codegen::cmpgtd_mm(int bank1, int bank2) {
    load_src64(bank1);
    int INSTR0[] = {0xc5, 0xfd, 0xc2, 0x86};//          	vcmpgt_oqpd x(%rsi),%ymm0,%ymm0
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank2, 0);
    mem[cursor++] = 0x1e;
    int INSTR1[] = {0xc5, 0xf5, 0xc2, 0x8e};//          	vcmpgt_oqpd x(%rsi),%ymm1,%ymm1
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank2, 1);
    mem[cursor++] = 0x1e;
    int INSTR2[] = {0xc5, 0xed, 0xc2, 0x96};//          	vcmpgt_oqpd x(%rsi),%ymm2,%ymm2
    add(INSTR2, size(INSTR2));
    addMemoryOffset(bank2, 2);
    mem[cursor++] = 0x1d;
    int INSTR3[] = {0xc5, 0xe5, 0xc2, 0x9e};//          	vcmpgt_oqpd x(%rsi),%ymm3,%ymm3
    add(INSTR3, size(INSTR3));
    addMemoryOffset(bank2, 3);
    mem[cursor++] = 0x1e;
    merge_cmp64();
}

void Codegen::cmpged_mm(int bank1, int bank2) {
    load_src64(bank1);
    int INSTR0[] = {0xc5, 0xfd, 0xc2, 0x86};//          	vcmpge_oqpd x(%rsi),%ymm0,%ymm0
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank2, 0);
    mem[cursor++] = 0xd;
    int INSTR1[] = {0xc5, 0xf5, 0xc2, 0x8e};//          	vcmpge_oqpd x(%rsi),%ymm1,%ymm1
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank2, 1);
    mem[cursor++] = 0xd;
    int INSTR2[] = {0xc5, 0xed, 0xc2, 0x96};//          	vcmpge_oqpd x(%rsi),%ymm2,%ymm2
    add(INSTR2, size(INSTR2));
    addMemoryOffset(bank2, 2);
    mem[cursor++] = 0xd;
    int INSTR3[] = {0xc5, 0xe5, 0xc2, 0x9e};//          	vcmpge_oqpd x(%rsi),%ymm3,%ymm3
    add(INSTR3, size(INSTR3));
    addMemoryOffset(bank2, 3);
    mem[cursor++] = 0xd;
    merge_cmp64();
}

void Codegen::mul32b_mm(int bank1, int bank2) {
    load_src32(bank1);
    int INSTR2[] = {0xc4, 0xe2, 0x7d, 0x40, 0x86};//          	vpmulld 0x123456(%rsi),%ymm0,%ymm0
    add(INSTR2, size(INSTR2));
    addMemoryOffset(bank2, 0);
    int INSTR3[] = {0xc4, 0xe2, 0x75, 0x40, 0x8e};//          	vpmulld 0x123456(%rsi),%ymm1,%ymm1
    add(INSTR3, size(INSTR3));
    addMemoryOffset(bank2, 1);
    save(32,bank1);
}

// 225 cycles
void Codegen::div32b_mm(int bank1, int bank2) {
    for (int i=0; i< 16; i++) {
        int INSTR0[] = {0x48, 0x8b, 0x86};     //	mov    0x123456(%rsi),%rax
        add(INSTR0, size(INSTR0));
        addMemoryOffset32(bank1, i);
        int INSTR1[] = {0x99};                  // cltd
        add(INSTR1, size(INSTR1));
        int INSTR2[] = {0xf7, 0xbe};            //  idivl   (%rsi)
        add(INSTR2, size(INSTR2));
        addMemoryOffset32(bank2, i);
        int INSTR3[] = {0x89, 0x86};            //  movl    %eax, 0x1234(%rsi)
        add(INSTR3, size(INSTR3));
        addMemoryOffset32(bank1, i);
    }
}

void Codegen::add32b_mm(int bank1, int bank2) {
    load_src32(bank1);
    int INSTR2[] = {0xc5, 0xfd, 0xfe, 0x86};//          	vpaddd 0x123456(%rsi),%ymm0,%ymm0
    add(INSTR2, size(INSTR2));
    addMemoryOffset(bank2, 0);
    int INSTR3[] = {0xc5, 0xf5, 0xfe, 0x8e};//          	vpaddd 0x123456(%rsi),%ymm1,%ymm1
    add(INSTR3, size(INSTR3));
    addMemoryOffset(bank2, 1);
    save(32,bank1);
}

void Codegen::sub32b_mm(int bank1, int bank2) {
    load_src32(bank1);
    int INSTR0[] = {0xc5, 0xfd, 0xfa, 0x86};//          	vpsubd 0x123456(%rsi),%ymm0,%ymm0
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank2, 0);
    int INSTR1[] = {0xc5, 0xf5, 0xfa, 0x8e};//          	vpsubd 0x123456(%rsi),%ymm1,%ymm1
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank2, 1);
    save(32,bank1);
}

void Codegen::xor32b_mm(int bank1, int bank2) {
    load_src32(bank1);
    int INSTR0[] = {0xc5 ,0xfd ,0xef ,0x86};//          	vpxor  0x1234(%rsi),%ymm0,%ymm0
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank2, 0);
    int INSTR1[] = {0xc5 ,0xf5 ,0xef ,0x8e};//          	vpxor  0x1234(%rsi),%ymm1,%ymm1
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank2, 1);
    save(32,bank1);
}

void Codegen::min32b_mm(int bank1, int bank2) {
    load_src32(bank1);
    int INSTR0[] = {0xc4 ,0xe2 ,0x7d ,0x39 ,0x86};//  	vpminsd 0x1246(%rsi),%ymm0,%ymm0
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank2, 0);
    int INSTR1[] = {0xc4 ,0xe2 ,0x75 ,0x39 ,0x8e};// 2 	vpminsd 0x1246(%rsi),%ymm1,%ymm1
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank2, 1);
    save(32,bank1);
}

void Codegen::max32b_mm(int bank1, int bank2) {
    load_src32(bank1);
    int INSTR0[] = {0xc4 ,0xe2 ,0x7d ,0x3d ,0x86};//  	vpmaxsd 0x1246(%rsi),%ymm0,%ymm0
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank2, 0);
    int INSTR1[] = {0xc4 ,0xe2 ,0x75 ,0x3d ,0x8e};// 2 	vpmaxsd 0x1246(%rsi),%ymm1,%ymm1
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank2, 1);
    save(32,bank1);
}

void Codegen::cmpeq32b_mm(int bank1, int bank2) {
    load_src32(bank1);
    int INSTR0[] = {0xc5, 0xfd, 0x76, 0x86};//          	vpcmpeqd x(%rsi),%ymm0,%ymm0
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank2, 0);
    int INSTR1[] = {0xc5, 0xf5, 0x76, 0x8e};//          	vpcmpeqd x(%rsi),%ymm0,%ymm0
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank2, 1);
    merge_cmp32();
}

void Codegen::cmple32b_mm(int bank1, int bank2,int len,bool isSigned) {
    cmpgt32b_mm(bank1,bank2,len,isSigned);
    int INSTR0[] = {0x48 ,0xf7 ,0xd0};//             	not    %rax
    add(INSTR0, size(INSTR0));
}

void Codegen::cmplt32b_mm(int bank1, int bank2,int len, bool isSigned) {
    load_src32(bank2);
    if (len == 32 && !isSigned) {
        int INSTR0[] = {0xc4 ,0xc1 , 0x2d, 0x76 ,0xe2};//      	vpcmpeqd %ymm10,%ymm10,%ymm4
        add(INSTR0, size(INSTR0));
        int INSTR1[] = {0xc5 ,0x7d ,0x6f ,0x86};//  	        vmovdqa 0x54544(%rsi),%ymm8
        add(INSTR1, size(INSTR1));
        addMemoryOffset(bank1, 0);
        int INSTR2[] = {0xc5 ,0x7d ,0x6f ,0x8e};//          	vmovdqa 0x54544(%rsi),%ymm9
        add(INSTR2, size(INSTR2));
        addMemoryOffset(bank1, 1);
        int INSTR3[] = {0xc5 ,0xdd ,0x72 ,0xf4 ,0x1f};//       	vpslld $0x1f,%ymm4,%ymm4
        add(INSTR3, size(INSTR3));
        int INSTR4[] = {0xc5 ,0xfd ,0xef ,0xc4};//          	vpxor  %ymm4,%ymm0,%ymm0
        add(INSTR4, size(INSTR4));
        int INSTR5[] = {0xc5 ,0xf5 ,0xef ,0xcc};//          	vpxor  %ymm4,%ymm1,%ymm1
        add(INSTR5, size(INSTR5));
        int INSTR6[] = {0xc5 ,0x3d ,0xef ,0xc4};//          	vpxor  %ymm4,%ymm8,%ymm8
        add(INSTR6, size(INSTR6));
        int INSTR7[] = {0xc5 ,0x35 ,0xef ,0xcc};//          	vpxor  %ymm4,%ymm9,%ymm9
        add(INSTR7, size(INSTR7));
        int INSTR8[] = {0xc4 ,0xc1 ,0x7d ,0x76 ,0xd0};//       	vpcmpeqd %ymm8,%ymm0,%ymm2
        add(INSTR8, size(INSTR8));
        int INSTR9[] = {0xc4 ,0xc1 ,0x75 ,0x76 ,0xd9};//       	vpcmpeqd %ymm9,%ymm1,%ymm3
        add(INSTR9, size(INSTR9));
        int INSTR10[] = {0xc4 ,0xc1 ,0x7d ,0x66 ,0xc0};//       vpcmpgtd %ymm8,%ymm0,%ymm0
        add(INSTR10, size(INSTR10));
        int INSTR11[] = {0xc4 ,0xc1 ,0x75 ,0x66 ,0xc9};//       vpcmpgtd %ymm9,%ymm1,%ymm1
        add(INSTR11, size(INSTR11));
    } else {
        int INSTR0[] = {0xc5 ,0xfd ,0x76 ,0x96};//	vpcmpeqd 0x565651(%rsi),%ymm0,%ymm2
        add(INSTR0, size(INSTR0));
        addMemoryOffset(bank1, 0);
        int INSTR1[] = {0xc5 ,0xf5 ,0x76 ,0x9e};// vpcmpeqd 0x565652(%rsi),%ymm1,%ymm3
        add(INSTR1, size(INSTR1));
        addMemoryOffset(bank1, 1);
        int INSTR2[] = {0xc5 ,0xfd ,0x66 ,0x86};//	vpcmgtd 0x565651(%rsi),%ymm0,%ymm0
        add(INSTR2, size(INSTR2));
        addMemoryOffset(bank1, 0);
        int INSTR3[] = {0xc5 ,0xf5 ,0x66 ,0x8e};// vpcmpgtd 0x565652(%rsi),%ymm1,%ymm1
        add(INSTR3, size(INSTR3));
        addMemoryOffset(bank1, 1);
    }
    int INSTR4[] = {0xc5 ,0xed ,0xdf ,0xc0};// vpandn %ymm0,%ymm2,%ymm0
    add(INSTR4, size(INSTR4));
    int INSTR5[] = {0xc5 ,0xe5 ,0xdf ,0xc9};//vpandn %ymm1,%ymm3,%ymm1
    add(INSTR5, size(INSTR5));
    merge_cmp32();
}

void Codegen::cmpgt32b_mm(int bank1, int bank2,int len,bool isSigned) {
    load_src32(bank1);
    if (len == 32 && !isSigned) {
        int INSTR0[] = {0xc4 ,0xc1 , 0x2d, 0x76 ,0xe2};//      	vpcmpeqd %ymm10,%ymm10,%ymm4
        add(INSTR0, size(INSTR0));
        int INSTR1[] = {0xc5 ,0x7d ,0x6f ,0x86};//  	        vmovdqa 0x54544(%rsi),%ymm8
        add(INSTR1, size(INSTR1));
        addMemoryOffset(bank2, 0);
        int INSTR2[] = {0xc5 ,0x7d ,0x6f ,0x8e};//          	vmovdqa 0x54544(%rsi),%ymm9
        add(INSTR2, size(INSTR2));
        addMemoryOffset(bank2, 1);
        int INSTR3[] = {0xc5 ,0xdd ,0x72 ,0xf4 ,0x1f};//       	vpslld $0x1f,%ymm4,%ymm4
        add(INSTR3, size(INSTR3));
        int INSTR4[] = {0xc5 ,0xfd ,0xef ,0xc4};//          	vpxor  %ymm4,%ymm0,%ymm0
        add(INSTR4, size(INSTR4));
        int INSTR5[] = {0xc5 ,0xf5 ,0xef ,0xcc};//          	vpxor  %ymm4,%ymm1,%ymm1
        add(INSTR5, size(INSTR5));
        int INSTR6[] = {0xc5 ,0x3d ,0xef ,0xc4};//          	vpxor  %ymm4,%ymm8,%ymm8
        add(INSTR6, size(INSTR6));
        int INSTR7[] = {0xc5 ,0x35 ,0xef ,0xcc};//          	vpxor  %ymm4,%ymm9,%ymm9
        add(INSTR7, size(INSTR7));
        int INSTR8[] = {0xc4 ,0xc1 ,0x7d ,0x66 ,0xc0};//       vpcmpgtd %ymm8,%ymm0,%ymm0
        add(INSTR8, size(INSTR8));
        int INSTR9[] = {0xc4 ,0xc1 ,0x75 ,0x66 ,0xc9};//       vpcmpgtd %ymm9,%ymm1,%ymm1
        add(INSTR9, size(INSTR9));
    } else {
        int INSTR0[] = {0xc5 ,0xfd ,0x66 ,0x86};//	vpcmpgtd 0x565651(%rsi),%ymm0,%ymm0
        add(INSTR0, size(INSTR0));
        addMemoryOffset(bank2, 0);
        int INSTR1[] = {0xc5 ,0xf5 ,0x66 ,0x8e};// vpcmpgtd 0x565652(%rsi),%ymm1,%ymm1
        add(INSTR1, size(INSTR1));
        addMemoryOffset(bank2, 1);
    }
    merge_cmp32();
}

void Codegen::cmpge32b_mm(int bank1, int bank2,int len,bool isSigned) {
    cmple32b_mm(bank2,bank1,len, isSigned);
}


void Codegen::cmpne32b_mm(int bank1, int bank2) {
    cmpeq32b_mm(bank1,bank2);
    int INSTR0[] = {0x48 ,0xf7 ,0xd0};//             	not    %rax
    add(INSTR0, size(INSTR0));
}

// sounds a lot, but 25 CPU cycles on zen2. 1.5 CPU cycle per number
void Codegen::mul64b_mm(int bank1, int bank2) {
    for (int i = 0; i < 16; i++) {
        int INSTR0[] = {0x48, 0x8b, 0x86};     //	mov    0x123456(%rsi),%rax
        add(INSTR0, size(INSTR0));
        addMemoryOffset64(bank1, i);
        int INSTR1[] = {0x48, 0x0f, 0xaf, 0x86};// 	imul   0x123456(%rsi),%rax
        add(INSTR1, size(INSTR1));
        addMemoryOffset64(bank2, i);
        int INSTR2[] = {0x48, 0x89, 0x86};     // 	mov    %rax,0x123456(%rsi)
        add(INSTR2, size(INSTR2));
        addMemoryOffset64(bank1, i);
    }
}

// 225 cycles
void Codegen::div64b_mm(int bank1, int bank2) {
    for (int i = 0; i < 16; i++) {
        int INSTR0[] = {0x48, 0x8b, 0x86};     //	mov    0x123456(%rsi),%rax
        add(INSTR0, size(INSTR0));
        addMemoryOffset64(bank1, i);
        int INSTR1[] = {0x48, 0x99};            // cqto
        add(INSTR1, size(INSTR1));
        int INSTR2[] = {0x48, 0xf7, 0xbe};      // 	idivq   0x123456(%rsi)
        add(INSTR2, size(INSTR2));
        addMemoryOffset64(bank2, i);
        int INSTR3[] = {0x48, 0x89, 0x86};     // 	mov    %rax,0x123456(%rsi)
        add(INSTR3, size(INSTR3));
        addMemoryOffset64(bank1, i);
    }
}

void Codegen::add64b_mm(int bank1, int bank2) {
    load_src64(bank1);
    int INSTR4[] = {0xc5, 0xfd, 0xd4, 0x86};//          	vpaddq 0x123456(%rsi),%ymm0,%ymm0
    add(INSTR4, size(INSTR4));
    addMemoryOffset(bank2, 0);
    int INSTR5[] = {0xc5, 0xf5, 0xd4, 0x8e};//          	vpaddq 0x123456(%rsi),%ymm0,%ymm0
    add(INSTR5, size(INSTR5));
    addMemoryOffset(bank2, 1);
    int INSTR6[] = {0xc5, 0xed, 0xd4, 0x96};//          	vpaddq 0x123456(%rsi),%ymm0,%ymm0
    add(INSTR6, size(INSTR6));
    addMemoryOffset(bank2, 2);
    int INSTR7[] = {0xc5, 0xe5, 0xd4, 0x9e};//          	vpaddq 0x123456(%rsi),%ymm0,%ymm0
    add(INSTR7, size(INSTR7));
    addMemoryOffset(bank2, 3);
    save(64,bank1);
}

void Codegen::sub64b_mm(int bank1, int bank2) {
    load_src64(bank1);
    int INSTR4[] = {0xc5, 0xfd, 0xfb, 0x86};//          	vpsubq 0x123456(%rsi),%ymm0,%ymm0
    add(INSTR4, size(INSTR4));
    addMemoryOffset(bank2, 0);
    int INSTR5[] = {0xc5, 0xf5, 0xfb, 0x8e};//          	vpaddq 0x123456(%rsi),%ymm0,%ymm0
    add(INSTR5, size(INSTR5));
    addMemoryOffset(bank2, 1);
    int INSTR6[] = {0xc5, 0xed, 0xfb, 0x96};//          	vpsubq 0x123456(%rsi),%ymm0,%ymm0
    add(INSTR6, size(INSTR6));
    addMemoryOffset(bank2, 2);
    int INSTR7[] = {0xc5, 0xe5, 0xfb, 0x9e};//          	vpsubq 0x123456(%rsi),%ymm0,%ymm0
    add(INSTR7, size(INSTR7));
    addMemoryOffset(bank2, 3);
    save(64,bank1);
}

void Codegen::xor64b_mm(int bank1, int bank2) {
    load_src64(bank1);
    int INSTR0[] = {0xc5 ,0xfd ,0xef ,0x86};//          	vpxor  0x1234(%rsi),%ymm0,%ymm0
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank2, 0);
    int INSTR1[] = {0xc5 ,0xf5 ,0xef ,0x8e};//          	vpxor  0x1234(%rsi),%ymm1,%ymm1
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank2, 1);
    int INSTR2[] = {0xc5 ,0xed ,0xef ,0x96};//          	vpxor  0x1234(%rsi),%ymm2,%ymm2
    add(INSTR2, size(INSTR2));
    addMemoryOffset(bank2, 2);
    int INSTR3[] = {0xc5 ,0xe5 ,0xef ,0x9e};//          	vpxor  0x1234(%rsi),%ymm3,%ymm3
    add(INSTR3, size(INSTR3));
    addMemoryOffset(bank2, 3);
    save(64,bank1);
}

void Codegen::min64b_mm(int bank1, int bank2) {
    for (int i=0; i< 16; i++) {
        int INSTR0[] = {0x48 ,0x8b ,0x86};// 	mov    0x1213(%rsi),%rax
        add(INSTR0, size(INSTR0));
        addMemoryOffset64(bank1, i);
        int INSTR1[] = {0x48 ,0x8b ,0x8e};//	mov    0x1234(%rsi),%rcx
        add(INSTR1, size(INSTR1));
        addMemoryOffset64(bank2, i);
        int INSTR2[] = {0x48 ,0x39 ,0xc8};//     cmp    %rcx,%rax
        add(INSTR2, size(INSTR2));
        int INSTR3[] = {0x48 ,0x0f ,0x4c ,0xc8};//cmovl  %rax,%rcx
        add(INSTR3, size(INSTR3));
        int INSTR4[] = {0x48 ,0x89 ,0x8e};// 	mov    %rcx,0x1213(%rsi)
        add(INSTR4, size(INSTR4));
        addMemoryOffset64(bank1, i);
    }
}

void Codegen::max64b_mm(int bank1, int bank2) {
    for (int i=0; i< 16; i++) {
        int INSTR0[] = {0x48 ,0x8b ,0x86};// 	mov    0x1213(%rsi),%rax
        add(INSTR0, size(INSTR0));
        addMemoryOffset64(bank1, i);
        int INSTR1[] = {0x48 ,0x8b ,0x8e};//	mov    0x1234(%rsi),%rcx
        add(INSTR1, size(INSTR1));
        addMemoryOffset64(bank2, i);
        int INSTR2[] = {0x48 ,0x39 ,0xc8};//     cmp    %rcx,%rax
        add(INSTR2, size(INSTR2));
        int INSTR3[] = {0x48 ,0x0f ,0x4f ,0xc8};//cmovgq  %rax, %rcx
        add(INSTR3, size(INSTR3));
        int INSTR4[] = {0x48 ,0x89 ,0x8e};// 	mov    %rcx,0x1213(%rsi)
        add(INSTR4, size(INSTR4));
        addMemoryOffset64(bank1, i);
    }
}

void Codegen::cmpeq64b_mm(int bank1, int bank2) {
    load_src64(bank1);
    int INSTR4[] = {0xc4, 0xe2, 0x7d, 0x29, 0x86};//          	vpcmpeqq 0x12345(%rsi),%ymm0,%ymm0
    add(INSTR4, size(INSTR4));
    addMemoryOffset(bank2, 0);
    int INSTR5[] = {0xc4, 0xe2, 0x75, 0x29, 0x8e};//          	vpcmpeqq 0x12345(%rsi),%ymm1,%ymm1
    add(INSTR5, size(INSTR5));
    addMemoryOffset(bank2, 1);
    int INSTR6[] = {0xc4, 0xe2, 0x6d, 0x29, 0x96};//          	vpcmpeqq 0x12345(%rsi),%ymm2,%ymm2
    add(INSTR6, size(INSTR6));
    addMemoryOffset(bank2, 2);
    int INSTR7[] = {0xc4, 0xe2, 0x65, 0x29, 0x9e};//          	vpcmpeqq 0x12345(%rsi),%ymm3,%ymm3
    add(INSTR7, size(INSTR7));
    addMemoryOffset(bank2, 3);
    merge_cmp64();
}

void Codegen::cmpne64b_mm(int bank1, int bank2) {
    cmpeq64b_mm(bank1,bank2);
    int INSTR0[] = {0x48 ,0xf7 ,0xd0};//             	not    %rax
    add(INSTR0, size(INSTR0));
}

void Codegen::cmple64b_mm(int bank1, int bank2,int len,bool isSigned) {
    cmpgt64b_mm(bank1,bank2,len,isSigned);
    int INSTR0[] = {0x48 ,0xf7 ,0xd0};//             	not    %rax
    add(INSTR0, size(INSTR0));
}

void Codegen::cmplt64b_mm(int bank1, int bank2,int len, bool isSigned) {
    load_src64(bank2);
    if (len == 64 && !isSigned) {
        int INSTR0[] = {0xc4 ,0xc2 , 0x2d, 0x29 ,0xe2};//      	vpcmpeqq %ymm10,%ymm10,%ymm4
        add(INSTR0, size(INSTR0));
        int INSTR1[] = {0xc5 ,0x7d ,0x6f ,0x86};//  	        vmovdqa 0x54544(%rsi),%ymm8
        add(INSTR1, size(INSTR1));
        addMemoryOffset(bank1, 0);
        int INSTR2[] = {0xc5 ,0x7d ,0x6f ,0x8e};//          	vmovdqa 0x54544(%rsi),%ymm9
        add(INSTR2, size(INSTR2));
        addMemoryOffset(bank1, 1);
        int INSTR3[] = {0xc5 ,0x7d ,0x6f ,0x96};//  	        vmovdqa 0x54544(%rsi),%ymm10
        add(INSTR3, size(INSTR3));
        addMemoryOffset(bank1, 2);
        int INSTR4[] = {0xc5 ,0x7d ,0x6f ,0x9e};//          	vmovdqa 0x54544(%rsi),%ymm9
        add(INSTR4, size(INSTR4));
        addMemoryOffset(bank1, 3);
        int INSTR5[] = {0xc5 ,0xdd ,0x73 ,0xf4 ,0x3f};//       	vpsllq $31,%ymm4,%ymm4
        add(INSTR5, size(INSTR5));
        int INSTR6[] = {0xc5 ,0xfd ,0xef ,0xc4};//          	vpxor  %ymm4,%ymm0,%ymm0
        add(INSTR6, size(INSTR6));
        int INSTR7[] = {0xc5 ,0xf5 ,0xef ,0xcc};//          	vpxor  %ymm4,%ymm1,%ymm1
        add(INSTR7, size(INSTR7));
        int INSTR8[] = {0xc5 ,0xed ,0xef ,0xd4};//          	vpxor  %ymm4,%ymm2,%ymm2
        add(INSTR8, size(INSTR8));
        int INSTR9[] = {0xc5 ,0xe5 ,0xef ,0xdc};//          	vpxor  %ymm4,%ymm3,%ymm3
        add(INSTR9, size(INSTR9));
        int INSTR10[] = {0xc5 ,0x3d ,0xef ,0xc4};//          	vpxor  %ymm4,%ymm8,%ymm8
        add(INSTR10, size(INSTR10));
        int INSTR11[] = {0xc5 ,0x35 ,0xef ,0xcc};//          	vpxor  %ymm4,%ymm9,%ymm9
        add(INSTR11, size(INSTR11));
        int INSTR12[] = {0xc5 ,0x2d ,0xef ,0xd4};//          	vpxor  %ymm4,%ymm10,%ymm10
        add(INSTR12, size(INSTR12));
        int INSTR13[] = {0xc5 ,0x25 ,0xef ,0xdc};//          	vpxor  %ymm4,%ymm11,%ymm11
        add(INSTR13, size(INSTR13));
        int INSTR14[] = {0xc4 ,0xc2 ,0x7d ,0x29 ,0xe0};//      	vpcmpeqq %ymm8,%ymm0,%ymm4
        add(INSTR14, size(INSTR14));
        int INSTR15[] = {0xc4 ,0xc2 ,0x75 ,0x29 ,0xe9};//      	vpcmpeqq %ymm9,%ymm1,%ymm5
        add(INSTR15, size(INSTR15));
        int INSTR16[] = {0xc4 ,0xc2 ,0x6d ,0x29 ,0xf2};//      	vpcmpeqq %ymm10,%ymm2,%ymm6
        add(INSTR16, size(INSTR16));
        int INSTR17[] = {0xc4 ,0xc2 ,0x65 ,0x29 ,0xfb};//      	vpcmpeqq %ymm11,%ymm3,%ymm7
        add(INSTR17, size(INSTR17));
        int INSTR18[] = {0xc4 ,0xc2 ,0x7d ,0x37 ,0xc0};//       vpcmpgtq %ymm8,%ymm0,%ymm0
        add(INSTR18, size(INSTR18));
        int INSTR19[] = {0xc4 ,0xc2 ,0x75 ,0x37 ,0xc9};//       vpcmpgtq %ymm9,%ymm1,%ymm1
        add(INSTR19, size(INSTR19));
        int INSTR20[] = {0xc4 ,0xc2 ,0x6d ,0x37 ,0xd2};//       vpcmpgtq %ymm10,%ymm2,%ymm2
        add(INSTR20, size(INSTR20));
        int INSTR21[] = {0xc4 ,0xc2 ,0x65 ,0x37 ,0xdb};//       vpcmpgtq %ymm11,%ymm3,%ymm3
        add(INSTR21, size(INSTR21));
   } else {
        int INSTR0[] = {0xc4 ,0xe2 ,0x7d ,0x29, 0xa6};//	vpcmpeqq 0x565651(%rsi),%ymm0,%ymm4
        add(INSTR0, size(INSTR0));
        addMemoryOffset(bank1, 0);
        int INSTR1[] = {0xc4 ,0xe2 ,0x75 ,0x29, 0xae};//    vpcmpeqq 0x565652(%rsi),%ymm1,%ymm3
        add(INSTR1, size(INSTR1));
        addMemoryOffset(bank1, 1);
        int INSTR2[] = {0xc4 ,0xe2 ,0x6d ,0x29, 0xb6};//	vpcmpeqq 0x565651(%rsi),%ymm2,%ymm6
        add(INSTR2, size(INSTR2));
        addMemoryOffset(bank1, 2);
        int INSTR3[] = {0xc4 ,0xe2 ,0x65 ,0x29, 0xbe};//    vpcmpeqq 0x565652(%rsi),%ymm1,%ymm3
        add(INSTR3, size(INSTR3));
        addMemoryOffset(bank1, 3);
        int INSTR4[] = {0xc4 ,0xe2 ,0x7d ,0x37, 0x86};//	vpcmgtq 0x565651(%rsi),%ymm0,%ymm0
        add(INSTR4, size(INSTR4));
        addMemoryOffset(bank1, 0);
        int INSTR5[] = {0xc4 ,0xe2 ,0x75 ,0x37, 0x8e};//	vpcmgtq 0x565651(%rsi),%ymm1,%ymm1
        add(INSTR5, size(INSTR5));
        addMemoryOffset(bank1, 1);
        int INSTR6[] = {0xc4 ,0xe2 ,0x6d ,0x37, 0x96};//	vpcmgtq 0x565651(%rsi),%ymm2,%ymm2
        add(INSTR6, size(INSTR6));
        addMemoryOffset(bank1, 2);
        int INSTR7[] = {0xc4 ,0xe2 ,0x65 ,0x37, 0x9e};//	vpcmgtq 0x565651(%rsi),%ymm3,%ymm3
        add(INSTR7, size(INSTR7));
        addMemoryOffset(bank1, 3);
    }
    int INSTR8[] = {0xc5 ,0xdd ,0xdf ,0xc0};//          	vpandn %ymm0,%ymm4,%ymm0
    add(INSTR8, size(INSTR8));
    int INSTR9[] = {0xc5 ,0xd5 ,0xdf ,0xc9};//          	vpandn %ymm1,%ymm5,%ymm1
    add(INSTR9, size(INSTR9));
    int INSTR10[] = {0xc5 ,0xcd ,0xdf ,0xd2};//          	vpandn %ymm2,%ymm6,%ymm2
    add(INSTR10, size(INSTR10));
    int INSTR11[] = {0xc5 ,0xc5 ,0xdf ,0xdb};//          	vpandn %ymm3,%ymm7,%ymm3
    add(INSTR11, size(INSTR11));
    merge_cmp64();
}

void Codegen::cmpgt64b_mm(int bank1, int bank2,int len,bool isSigned) {
    load_src64(bank1);
    if (len == 64 && !isSigned) {
        int INSTR0[] = {0xc4 ,0xc2 , 0x2d, 0x29 ,0xe2};//      	vpcmpeqq %ymm10,%ymm10,%ymm4
        add(INSTR0, size(INSTR0));
        int INSTR1[] = {0xc5 ,0x7d ,0x6f ,0x86};//  	        vmovdqa 0x54544(%rsi),%ymm8
        add(INSTR1, size(INSTR1));
        addMemoryOffset(bank2, 0);
        int INSTR2[] = {0xc5 ,0x7d ,0x6f ,0x8e};//          	vmovdqa 0x54544(%rsi),%ymm9
        add(INSTR2, size(INSTR2));
        addMemoryOffset(bank2, 1);
        int INSTR3[] = {0xc5 ,0x7d ,0x6f ,0x96};//  	        vmovdqa 0x54544(%rsi),%ymm10
        add(INSTR3, size(INSTR3));
        addMemoryOffset(bank2, 2);
        int INSTR4[] = {0xc5 ,0x7d ,0x6f ,0x9e};//          	vmovdqa 0x54544(%rsi),%ymm9
        add(INSTR4, size(INSTR4));
        addMemoryOffset(bank2, 3);
        int INSTR5[] = {0xc5 ,0xdd ,0x73 ,0xf4 ,0x3f};//       	vpsllq $31,%ymm4,%ymm4
        add(INSTR5, size(INSTR5));
        int INSTR6[] = {0xc5 ,0xfd ,0xef ,0xc4};//          	vpxor  %ymm4,%ymm0,%ymm0
        add(INSTR6, size(INSTR6));
        int INSTR7[] = {0xc5 ,0xf5 ,0xef ,0xcc};//          	vpxor  %ymm4,%ymm1,%ymm1
        add(INSTR7, size(INSTR7));
        int INSTR8[] = {0xc5 ,0xed ,0xef ,0xd4};//          	vpxor  %ymm4,%ymm2,%ymm2
        add(INSTR8, size(INSTR8));
        int INSTR9[] = {0xc5 ,0xe5 ,0xef ,0xdc};//          	vpxor  %ymm4,%ymm3,%ymm3
        add(INSTR9, size(INSTR9));
        int INSTR10[] = {0xc5 ,0x3d ,0xef ,0xc4};//          	vpxor  %ymm4,%ymm8,%ymm8
        add(INSTR10, size(INSTR10));
        int INSTR11[] = {0xc5 ,0x35 ,0xef ,0xcc};//          	vpxor  %ymm4,%ymm9,%ymm9
        add(INSTR11, size(INSTR11));
        int INSTR12[] = {0xc5 ,0x2d ,0xef ,0xd4};//          	vpxor  %ymm4,%ymm10,%ymm10
        add(INSTR12, size(INSTR12));
        int INSTR13[] = {0xc5 ,0x25 ,0xef ,0xdc};//          	vpxor  %ymm4,%ymm11,%ymm11
        add(INSTR13, size(INSTR13));
        int INSTR14[] = {0xc4 ,0xc2 ,0x7d ,0x37 ,0xc0};//       vpcmpgtq %ymm8,%ymm0,%ymm0
        add(INSTR14, size(INSTR14));
        int INSTR15[] = {0xc4 ,0xc2 ,0x75 ,0x37 ,0xc9};//       vpcmpgtq %ymm9,%ymm1,%ymm1
        add(INSTR15, size(INSTR15));
        int INSTR16[] = {0xc4 ,0xc2 ,0x6d ,0x37 ,0xd2};//       vpcmpgtq %ymm10,%ymm2,%ymm2
        add(INSTR16, size(INSTR16));
        int INSTR17[] = {0xc4 ,0xc2 ,0x65 ,0x37 ,0xdb};//       vpcmpgtq %ymm11,%ymm3,%ymm3
        add(INSTR17, size(INSTR17));
    } else {
        int INSTR0[] = {0xc4 ,0xe2 ,0x7d ,0x37, 0x86};//	vpcmgtq 0x565651(%rsi),%ymm0,%ymm0
        add(INSTR0, size(INSTR0));
        addMemoryOffset(bank2, 0);
        int INSTR1[] = {0xc4 ,0xe2 ,0x75 ,0x37, 0x8e};//	vpcmgtq 0x565651(%rsi),%ymm1,%ymm1
        add(INSTR1, size(INSTR1));
        addMemoryOffset(bank2, 1);
        int INSTR2[] = {0xc4 ,0xe2 ,0x6d ,0x37, 0x96};//	vpcmgtq 0x565651(%rsi),%ymm2,%ymm2
        add(INSTR2, size(INSTR2));
        addMemoryOffset(bank2, 2);
        int INSTR3[] = {0xc4 ,0xe2 ,0x65 ,0x37, 0x9e};//	vpcmgtq 0x565651(%rsi),%ymm3,%ymm3
        add(INSTR3, size(INSTR3));
        addMemoryOffset(bank2, 3);
    }
    merge_cmp64();
}

void Codegen::cmpge64b_mm(int bank1, int bank2,int len,bool isSigned) {
    cmple64b_mm(bank2,bank1,len, isSigned);
}

// 58 cycles on zen2. 3.67 cycles per number
void Codegen::mul128b_mm(int bank1, int bank2) {
    for (int i = 0; i < 16; i++) {
        int INSTR0[] = {0x48, 0x8b, 0x86};     //	mov    0x123456(%rsi),%rax
        add(INSTR0, size(INSTR0));
        addMemoryOffset64(bank1, 2*i+1);
        int INSTR1[] = {0x48, 0x8b, 0x8e};     //	mov    0x123456(%rsi),%rcx
        add(INSTR1, size(INSTR1));
        addMemoryOffset64(bank2, 2*i+1);
        int INSTR2[] = {0x48, 0x0f, 0xaf, 0x86};     // imul   0x15356(%rsi),%rax
        add(INSTR2, size(INSTR2));
        addMemoryOffset64(bank2, 2*i);
        int INSTR3[] = {0x48, 0x0f, 0xaf, 0x8e};     // imul   0x15356(%rsi),%rcx
        add(INSTR3, size(INSTR3));
        addMemoryOffset64(bank1, 2*i);
        int INSTR4[] = {0x48 ,0x01 ,0xc1};            // addq    %rax, %rcx
        add(INSTR4, size(INSTR4));
        int INSTR5[] = {0x48 ,0x8b ,0x86};            // movq    (%rsi), %rax
        add(INSTR5, size(INSTR5));
        addMemoryOffset64(bank1, 2*i);
        int INSTR6[] = {0x48 ,0xf7 ,0xa6};              // mulq   0x123456(%rdi)
        add(INSTR6, size(INSTR6));
        addMemoryOffset64(bank2, 2*i);
        int INSTR7[] = {0x48 ,0x01 ,0xca};//             	add    %rcx,%rdx
        add(INSTR7, size(INSTR7));
        int INSTR8[] = {0x48 ,0x89 ,0x86}; //           movq    %rax, (%rsi)
        add(INSTR8, size(INSTR8));
        addMemoryOffset64(bank1, 2*i);
        int INSTR9[] = {0x48 ,0x89 ,0x96}; //           movq    %rcx, 8(%rsi)
        add(INSTR9, size(INSTR9));
        addMemoryOffset64(bank1, 2*i+1);
    }
}

static void div128_C(__int128_t *a,const __int128_t *b) {
    *a = (*a) / (*b);
}

// 310 cycles on zen 2
void Codegen::div128b_mm(int bank1, int bank2) {
    auto p = div128_C;
    int INSTR0[] = { 0x56, 0x57, 0x41, 0x54, 0x41, 0x55, 0x49, 0x89, 0xf4}; // push   %rsi; push   %rdi; push   %r12; push %r13, mov    %rsi,%r12
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {  0x48, 0xb8 };
    add(INSTR1, size(INSTR1));
    addMemoryOffsetL((uint64_t)p);
    int INSTR2[] = { 0x49 ,0x89 ,0xc5}; //  mov    %rax,%r13
    add(INSTR2, size(INSTR2));
    for (int i=0; i< 16; i++) {
        int INSTR3[] = { 0x49 ,0x8d ,0xbc ,0x24}; // lea    0x123(%r12),%rdi
        add(INSTR3, size(INSTR3));
        addMemoryOffset128(bank1, i);
        int INSTR4[] = { 0x49 ,0x8d ,0xb4 ,0x24}; // lea    0x123(%r12),%rsi
        add(INSTR4, size(INSTR4));
        addMemoryOffset128(bank2, i);
        int INSTR5[] = { 0x41 ,0xff ,0xd5};// call %rax13
        add(INSTR5, size(INSTR5));
    }
    int INSTR6[] = { 0x41, 0x5d, 0x41, 0x5c, 0x5f, 0x5e};
    add(INSTR6, size(INSTR6));
}

// 31 cycles on zen2
void Codegen::add128b_mm(int bank1, int bank2) {
    for (int i = 0; i < 16; i++) {
        int INSTR0[] = {0x48, 0x8b, 0x86};     //	mov    0x123456(%rsi),%rax
        add(INSTR0, size(INSTR0));
        addMemoryOffset64(bank2, 2*i);
        int INSTR1[] = {0x48, 0x8b, 0x8e};     //	mov    0x123456(%rsi),%rcx
        add(INSTR1, size(INSTR1));
        addMemoryOffset64(bank2, 2*i+1);
        int INSTR2[] = {0x48 ,0x01 ,0x86}; //    add    %rax,0x4545(%rsi)
        add(INSTR2, size(INSTR2));
        addMemoryOffset64(bank1, 2*i);
        int INSTR3[] = {0x48 ,0x11 ,0x8e}; //    addc    %rcx,0x4545(%rsi)
        add(INSTR3, size(INSTR3));
        addMemoryOffset64(bank1, 2*i+1);
    }
}

void Codegen::sub128b_mm(int bank1, int bank2) {
    for (int i = 0; i < 16; i++) {
        int INSTR0[] = {0x48, 0x8b, 0x86};     //	mov    0x123456(%rsi),%rax
        add(INSTR0, size(INSTR0));
        addMemoryOffset64(bank2, 2*i);
        int INSTR1[] = {0x48, 0x8b, 0x8e};     //	mov    0x123456(%rsi),%rcx
        add(INSTR1, size(INSTR1));
        addMemoryOffset64(bank2, 2*i+1);
        int INSTR2[] = {0x48 ,0x29 ,0x86}; //    subq    %rax,0x4545(%rsi)
        add(INSTR2, size(INSTR2));
        addMemoryOffset64(bank1, 2*i);
        int INSTR3[] = {0x48 ,0x19 ,0x8e}; //    sbbq    %rcx,0x4545(%rsi)
        add(INSTR3, size(INSTR3));
        addMemoryOffset64(bank1, 2*i+1);
    }
}

void Codegen::xor128b_mm(int bank1, int bank2) {
    load_src128(bank1);
    int INSTR0[] = {0xc5 ,0xfd ,0xef ,0x86};//          	vpxor  0x1234(%rsi),%ymm0,%ymm0
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank2, 0);
    int INSTR1[] = {0xc5 ,0xf5 ,0xef ,0x8e};//          	vpxor  0x1234(%rsi),%ymm1,%ymm1
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank2, 1);
    int INSTR2[] = {0xc5 ,0xed ,0xef ,0x96};//          	vpxor  0x1234(%rsi),%ymm2,%ymm2
    add(INSTR2, size(INSTR2));
    addMemoryOffset(bank2, 2);
    int INSTR3[] = {0xc5 ,0xe5 ,0xef ,0x9e};//          	vpxor  0x1234(%rsi),%ymm3,%ymm3
    add(INSTR3, size(INSTR3));
    addMemoryOffset(bank2, 3);
    int INSTR4[] = {0xc5 ,0xdd ,0xef ,0xa6};//          	vpxor  0x1234(%rsi),%ymm4,%ymm4
    add(INSTR4, size(INSTR4));
    int INSTR5[] = {0xc5 ,0xd5 ,0xef ,0xae};//          	vpxor  0x1234(%rsi),%ymm5,%ymm5
    add(INSTR5, size(INSTR5));
    int INSTR6[] = {0xc5 ,0xcd ,0xef ,0xb6};//          	vpxor  0x1234(%rsi),%ymm6,%ymm6
    add(INSTR6, size(INSTR6));
    int INSTR7[] = {0xc5 ,0xc5 ,0xef ,0xbe};//          	vpxor  0x1234(%rsi),%ymm7,%ymm7
    add(INSTR7, size(INSTR7));
    save(128,bank1);
}

void Codegen::cmpeq128b_mm(int bank1, int bank2) {
    int INSTR0[] = {0x48 ,0x31 ,0xc9};//             	xor    %rcx,%rcx
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0x4d ,0x31 ,0xc0};//             	xor    %r8,%r8
    add(INSTR1, size(INSTR1));

    for (int i = 0; i < 16; i++) {
        int INSTR2[] = {0x48 ,0x8b ,0x86};//  	mov    0x1234(%rsi),%rax
        add(INSTR2, size(INSTR2));
        addMemoryOffset64(bank1, 2*i);
        int INSTR3[] = {0x48 ,0x8b ,0x96};//  	mov    0x12348(%rsi),%rdx
        add(INSTR3, size(INSTR3));
        addMemoryOffset64(bank1, 2*i+1);
        int INSTR4[] = {0x48 ,0x33 ,0x86};//  	xor    0x1245(%rsi),%rax
        add(INSTR4, size(INSTR4));
        addMemoryOffset64(bank2, 2*i);
        int INSTR5[] = {0x48 ,0x33 ,0x96};//  	xor    0x45458(%rsi),%rdx
        add(INSTR5, size(INSTR5));
        addMemoryOffset64(bank2, 2*i+1);
        int INSTR6[] = {0x48 ,0x09 ,0xd0};//   	or     %rdx,%rax
        add(INSTR6, size(INSTR6));
        int INSTR7[] = {0x0f ,0x94 ,0xc1};//   	sete   %cl
        add(INSTR7, size(INSTR7));
        if (i>0) {
            int INSTR8[] = {0x48 ,0xc1 ,0xe1};// shl    $0x4,%rcx
            add(INSTR8, size(INSTR8));
            mem[cursor++] = i;
        }
        int INSTR9[] = {0x49 ,0x09 ,0xc8};//    	or    %rcx,%r8
        add(INSTR9, size(INSTR9));
        add(INSTR0, size(INSTR0));
    }
    int INSTR10[] = {0x4c,0x89 ,0xc0};//           mov    %r8,%rax
  	add(INSTR10, size(INSTR10));
}

void Codegen::min128b_mm(int bank1, int bank2) {
    for (int i=0; i< 16; i++) {
        int INSTR0[] = {0x48 ,0x8b ,0x86};// 	mov    0x1234(%rsi),%rax
        add(INSTR0, size(INSTR0));
        addMemoryOffset64(bank1, 2*i+1);
        int INSTR1[] = {0x4c ,0x8b ,0x86};// 	mov    0x1234(%rsi),%r8
        add(INSTR1, size(INSTR1));
        addMemoryOffset64(bank1, 2*i);
        int INSTR2[] = {0x4c ,0x8b ,0x8e};// 	mov    0x1234(%rsi),%r9
        add(INSTR2, size(INSTR2));
        addMemoryOffset64(bank2, 2*i);
        int INSTR3[] = {0x4c ,0x8b ,0x96};// 	mov    0x1234(%rsi),%r10
        add(INSTR3, size(INSTR3));
        addMemoryOffset64(bank2, 2*i+1);
        int INSTR4[] = {0x4d ,0x39 ,0xc8};//   	cmp    %r9,%r8
        add(INSTR4, size(INSTR4));
        int INSTR5[] = {0x48 ,0x89 ,0xc1};//   	mov    %rax,%rcx
        add(INSTR5, size(INSTR5));
        int INSTR6[] = {0x4c ,0x19 ,0xd1};//   	sbb    %r10,%rcx
        add(INSTR6, size(INSTR6));
        int INSTR7[] = {0x4c ,0x0f ,0x4c ,0xd0};// 	cmovl  %rax,%r10
        add(INSTR7, size(INSTR7));
        int INSTR8[] = {0x4d ,0x0f ,0x4c ,0xc8};// 	cmovl  %r8,%r9
        add(INSTR8, size(INSTR8));
        int INSTR9[] = {0x4c ,0x89 ,0x8e};//	mov    %r9,0x1234(%rsi)
        add(INSTR9, size(INSTR9));
        addMemoryOffset64(bank1, 2*i);
        int INSTR10[] = {0x4c ,0x89 ,0x96};// 	mov    %r10,0x1234(%rsi)
        add(INSTR10, size(INSTR10));
        addMemoryOffset64(bank1, 2*i+1);
    }
}

void Codegen::max128b_mm(int bank1, int bank2) {
    for (int i=0; i< 16; i++) {
        int INSTR0[] = {0x4c ,0x8b ,0x86};// 	mov    0x1234(%rsi),%r8
        add(INSTR0, size(INSTR0));
        addMemoryOffset64(bank1, 2*i);
        int INSTR1[] = {0x48 ,0x8b ,0x8e};// 	mov    0x1234(%rsi),%rcx
        add(INSTR1, size(INSTR1));
        addMemoryOffset64(bank1, 2*i+1);
        int INSTR2[] = {0x4c ,0x8b ,0x8e};// 	mov    0x1245(%rsi),%r9
        add(INSTR2, size(INSTR2));
        addMemoryOffset64(bank2, 2*i);
        int INSTR3[] = {0x48 ,0x8b ,0x86};// 	mov    0x1234(%rsi),%rax
        add(INSTR3, size(INSTR3));
        addMemoryOffset64(bank2, 2*i+1);
        int INSTR4[] = {0x4d ,0x39 ,0xc1};//  	cmp    %r8,%r9
        add(INSTR4, size(INSTR4));
        int INSTR5[] = {0x49 ,0x89 ,0xc2};//   	mov    %rax,%r10
        add(INSTR5, size(INSTR5));
        int INSTR6[] = {0x49 ,0x19 ,0xca};//  	sbb    %rcx,%r10
        add(INSTR6, size(INSTR6));
        int INSTR7[] = {0x48 ,0x0f ,0x4c ,0xc1};// 	cmovl  %rcx,%rax
        add(INSTR7, size(INSTR7));
        int INSTR8[] = {0x4d ,0x0f ,0x4c ,0xc8};// 	cmovl  %r8,%r9
        add(INSTR8, size(INSTR8));
        int INSTR9[] = {0x4c ,0x89 ,0x8e};// 	mov    %r9,0x1234(%rsi)
        add(INSTR9, size(INSTR9));
        addMemoryOffset64(bank1, 2*i);
        int INSTR10[] = {0x48 ,0x89 ,0x86};// 	mov    %rax,0x1234(%rsi)
        add(INSTR10, size(INSTR10));
        addMemoryOffset64(bank1, 2*i+1);
    }
}

void Codegen::cmpne128b_mm(int bank1, int bank2) {
    cmpeq128b_mm(bank1,bank2);
    int INSTR0[] = {0x48 ,0xf7 ,0xd0};//             	not    %rax
    add(INSTR0, size(INSTR0));
}

void Codegen::cmple128b_mm(int bank1, int bank2) {
    cmpgt128b_mm(bank1,bank2);
    int INSTR0[] = {0x48 ,0xf7 ,0xd0};//             	not    %rax
    add(INSTR0, size(INSTR0));
}

void Codegen::cmplt128b_mm(int bank1, int bank2) {
    int INSTR0[] = {0x48 ,0x31 ,0xc9};//             	xor    %rcx,%rcx
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0x4d ,0x31 ,0xc0};//             	xor    %r8,%r8
    add(INSTR1, size(INSTR1));

    for (int i = 0; i < 16; i++) {
        int INSTR2[] = {0x48 ,0x8b ,0x96};//  	mov    0x1234(%rsi),%rdx
        add(INSTR2, size(INSTR2));
        addMemoryOffset64(bank2, 2*i);
        int INSTR3[] = {0x48 ,0x39 ,0x96};//  	cmp    %rdx,0x12348(%rsi)
        add(INSTR3, size(INSTR3));
        addMemoryOffset64(bank1, 2*i);
        int INSTR4[] = {0x48 ,0x8b ,0x86};//  	mov    0x1245(%rsi),%rax
        add(INSTR4, size(INSTR4));
        addMemoryOffset64(bank1, 2*i+1);
        int INSTR5[] = {0x48 ,0x1b ,0x86};//  	sbbq    8(%rsi), %rax
        add(INSTR5, size(INSTR5));
        addMemoryOffset64(bank2, 2*i+1);
        int INSTR6[] = {0x0f ,0x9c ,0xc1};//             	setl   %cl
        add(INSTR6, size(INSTR6));
        if (i>0) {
            int INSTR7[] = {0x48 ,0xc1 ,0xe1};// shl    $0x4,%rcx
            add(INSTR7, size(INSTR7));
            mem[cursor++] = i;
        }
        int INSTR8[] = {0x49 ,0x09 ,0xc8};//             	or    %rcx,%r8
        add(INSTR8, size(INSTR8));
        add(INSTR0, size(INSTR0));
    }
    int INSTR9[] = {0x4c,0x89 ,0xc0};//           mov    %r8,%rax
    add(INSTR9, size(INSTR9));
}

void Codegen::cmpgt128b_mm(int bank1, int bank2) {
    int INSTR0[] = {0x48 ,0x31 ,0xc9};//             	xor    %rcx,%rcx
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0x4d ,0x31 ,0xc0};//             	xor    %r8,%r8
    add(INSTR1, size(INSTR1));

    for (int i = 0; i < 16; i++) {
        int INSTR2[] = {0x48 ,0x8b ,0x96};//  	mov    0x1234(%rsi),%rdx
        add(INSTR2, size(INSTR2));
        addMemoryOffset64(bank1, 2*i);
        int INSTR3[] = {0x48 ,0x39 ,0x96};//  	cmp    %rdx,0x12348(%rsi)
        add(INSTR3, size(INSTR3));
        addMemoryOffset64(bank2, 2*i);
        int INSTR4[] = {0x48 ,0x8b ,0x86};//  	mov    0x1245(%rsi),%rax
        add(INSTR4, size(INSTR4));
        addMemoryOffset64(bank2, 2*i+1);
        int INSTR5[] = {0x48 ,0x1b ,0x86};//  	sbbq    8(%rsi), %rax
        add(INSTR5, size(INSTR5));
        addMemoryOffset64(bank1, 2*i+1);
        int INSTR6[] = {0x0f ,0x9c ,0xc1};//    setl   %cl
        add(INSTR6, size(INSTR6));
        if (i>0) {
            int INSTR7[] = {0x48 ,0xc1 ,0xe1};// shl    $0x4,%rcx
            add(INSTR7, size(INSTR7));
            mem[cursor++] = i;
        }
        int INSTR8[] = {0x49 ,0x09 ,0xc8};//             	or    %rcx,%r8
        add(INSTR8, size(INSTR8));
        add(INSTR0, size(INSTR0));
    }
    int INSTR9[] = {0x4c,0x89 ,0xc0};//           mov    %r8,%rax
    add(INSTR9, size(INSTR9));
}

void Codegen::cmpge128b_mm(int bank1, int bank2) {
    cmplt128b_mm(bank1,bank2);
    int INSTR0[] = {0x48 ,0xf7 ,0xd0};//             	not    %rax
    add(INSTR0, size(INSTR0));
}

void Codegen::fetchVariable(int len,bool isFloat, bool isSigned, int targetLen, bool targetIsFloat, int srcIndex, int bank, double scale) {
    sourceIndex = srcIndex;
    if (len == 16 && isFloat) {
        f16Tof32(srcIndex,bank);
    } else {
        unpack(len,isSigned);
    }
    int alen = getKernelLengthForColumn(len);
    bool needReload = addConvertor(alen, targetLen, isFloat, targetIsFloat,isSigned, bank);
    if (scale != 1) {
        if (targetLen == 32 && targetIsFloat) {
            if (needReload) {
                load_src32(bank);
                needReload = false;
            }
            scalef((float)scale);
        }
        if (targetLen == 64 && targetIsFloat) {
            if (needReload) {
                load_src64(bank);
                needReload = false;
            }
            scaled(scale);
        }
    }
    if (!needReload) {
        save(targetLen,bank);
    }
}

void Codegen::loadConstant(__uint128_t &cst,int len, bool isFloat, int bank, bool isInClause) {
    if (isInClause) {
        if (isFloat && len == 16) {
            auto h = halfFloatToFloat(cst);
            auto v = *(uint32_t*)&h;
            inValues.push_back(v);
        } else {
            inValues.push_back(cst);
        }
    }  else {
        if (isFloat && len == 16) {
            loadConstantF16(cst,bank);
        } else {
            if (len <= 32) {
                loadConstant32(cst,bank);
            } else if (len <= 64) {
                loadConstant64(cst,bank);
            } else {
                loadConstant128(cst,bank);
            }
        }
    }
}

void Codegen::cmpeq(int len, bool isFloat, bool isSigned, int bank1, int bank2) {
    if (isFloat) {
        switch (len) {
            case 16:
            case 32:
                cmpeqf_mm(bank1,bank2);
                break;
            case 64:
                cmpeqd_mm(bank1,bank2);
                break;
            default:
                break;
        }
    } else {
        if (len <= 32) {
            cmpeq32b_mm(bank1,bank2);
        } else if (len <= 64) {
            cmpeq64b_mm(bank1,bank2);
        }  else {
            cmpeq128b_mm(bank1,bank2);
        }
    }
    merge_null();
}

void Codegen::cmpne(int len, bool isFloat, bool isSigned, int bank1, int bank2) {
    if (isFloat) {
        switch (len) {
            case 16:
            case 32:
                cmpnef_mm(bank1, bank2);
                break;
            case 64:
                cmpned_mm(bank1, bank2);
                break;
            default:
                break;
        }
    } else {
        if (len <= 32) {
            cmpne32b_mm(bank1, bank2);
        } else if (len <= 64) {
            cmpne64b_mm(bank1, bank2);
        } else {
            cmpne128b_mm(bank1, bank2);
        }
    }
    merge_null();
}

void Codegen::cmplt(int len, bool isFloat, bool isSigned, int bank1, int bank2) {
    if (isFloat) {
        switch (len) {
            case 16:
            case 32:
                cmpltf_mm(bank1, bank2);
                break;
            case 64:
                cmpltd_mm(bank1, bank2);
                break;
            default:
                break;
        }
    } else {
        if (len <= 32) {
            cmplt32b_mm(bank1, bank2,len, isSigned);
        } else if (len <= 64) {
            cmplt64b_mm(bank1, bank2,len, isSigned);
        } else {
            cmplt128b_mm(bank1, bank2);
        }
    }
    merge_null();
}

void Codegen::cmple(int len, bool isFloat, bool isSigned, int bank1, int bank2) {
    if (isFloat) {
        switch (len) {
            case 16:
            case 32:
                cmplef_mm(bank1, bank2);
                break;
            case 64:
                cmpled_mm(bank1, bank2);
                break;
            default:
                break;
        }
    } else {
        if (len <= 32) {
            cmple32b_mm(bank1, bank2,len,isSigned);
        } else if (len <= 64) {
            cmple64b_mm(bank1, bank2,len,isSigned);
        } else {
            cmple128b_mm(bank1, bank2);
        }
    }
    merge_null();
}

void Codegen::cmpgt(int len, bool isFloat, bool isSigned, int bank1, int bank2) {
    if (isFloat) {
        switch (len) {
            case 16:
            case 32:
                cmpgtf_mm(bank1, bank2);
                break;
            case 64:
                cmpgtd_mm(bank1, bank2);
                break;
            default:
                break;
        }
    } else {
        if (len <= 32) {
            cmpgt32b_mm(bank1, bank2,len, isSigned);
        } else if (len <= 64) {
            cmpgt64b_mm(bank1, bank2,len, isSigned);
        } else {
            cmpgt128b_mm(bank1, bank2);
        }
    }
    merge_null();
}

void Codegen::cmpge(int len, bool isFloat, bool isSigned, int bank1, int bank2) {
    if (isFloat) {
        switch (len) {
            case 16:
            case 32:
                cmpgef_mm(bank1, bank2);
                break;
            case 64:
                cmpged_mm(bank1, bank2);
                break;
            default:
                break;
        }
    } else {
        if (len <= 32) {
            cmpge32b_mm(bank1, bank2,len,isSigned);
        } else if (len <= 64) {
            cmpge64b_mm(bank1, bank2,len,isSigned);
        } else {
            cmpge128b_mm(bank1, bank2);
        }
    }
    merge_null();
}

void Codegen::add(int len, bool isFloat, int bank1, int bank2) {
    if (isFloat) {
        switch (len) {
            case 16:
            case 32:
                addf_mm(bank1, bank2);
                break;
            case 64:
                addd_mm(bank1, bank2);
                break;
            default:
                break;
        }
    } else {
        if (len <= 32) {
            add32b_mm(bank1, bank2);
        } else if (len <= 64) {
            add64b_mm(bank1, bank2);
        } else {
            add128b_mm(bank1, bank2);
        }
    }
}

void Codegen::sub(int len, bool isFloat, int bank1, int bank2) {
    if (isFloat) {
        switch (len) {
            case 16:
            case 32:
                subf_mm(bank1, bank2);
                break;
            case 64:
                subd_mm(bank1, bank2);
                break;
            default:
                break;
        }
    } else {
        if (len <= 32) {
            sub32b_mm(bank1, bank2);
        } else if (len <= 64) {
            sub64b_mm(bank1, bank2);
        } else {
            sub128b_mm(bank1, bank2);
        }
    }
}

void Codegen::mul(int len, bool isFloat, int bank1, int bank2) {
    if (isFloat) {
        switch (len) {
            case 16:
            case 32:
                mulf_mm(bank1, bank2);
                break;
            case 64:
                muld_mm(bank1, bank2);
                break;
            default:
                break;
        }
    } else {
        if (len <= 32) {
            mul32b_mm(bank1, bank2);
        } else if (len <= 64) {
            mul64b_mm(bank1, bank2);
        } else {
            mul128b_mm(bank1, bank2);
        }
    }
}

void Codegen::div(int len, bool isFloat, int bank1, int bank2) {
    if (isFloat) {
        switch (len) {
            case 16:
            case 32:
                divf_mm(bank1, bank2);
                break;
            case 64:
                divd_mm(bank1, bank2);
                break;
            default:
                break;
        }
    } else {
        if (len <= 32) {
            div32b_mm(bank1, bank2);
        } else if (len <= 64) {
            div64b_mm(bank1, bank2);
        } else {
            div128b_mm(bank1, bank2);
        }
    }
}

void Codegen::xor_mm(int len, bool isFloat, int bank1, int bank2) {
    if (len <= 32) {
        xor32b_mm(bank1, bank2);
    } else if (len <= 64) {
        xor64b_mm(bank1, bank2);
    } else {
        xor128b_mm(bank1, bank2);
    }
}

void Codegen::min(int len, bool isFloat, int bank1, int bank2) {
    if (isFloat) {
        switch (len) {
            case 16:
            case 32:
                minf_mm(bank1, bank2);
                break;
            case 64:
                mind_mm(bank1, bank2);
                break;
            default:
                break;
        }
    } else {
        if (len <= 32) {
            min32b_mm(bank1, bank2);
        } else if (len <= 64) {
            min64b_mm(bank1, bank2);
        } else {
            min128b_mm(bank1, bank2);
        }
    }
}

void Codegen::max(int len, bool isFloat, int bank1, int bank2) {
    if (isFloat) {
        switch (len) {
            case 16:
            case 32:
                maxf_mm(bank1, bank2);
                break;
            case 64:
                maxd_mm(bank1, bank2);
                break;
            default:
                break;
        }
    } else {
        if (len <= 32) {
            max32b_mm(bank1, bank2);
        } else if (len <= 64) {
            max64b_mm(bank1, bank2);
        } else {
            max128b_mm(bank1, bank2);
        }
    }
}

void Codegen::cmp(OperatorType T,int len, bool isFloat, bool isSigned, int bank1, int bank2) {
    if (T == OperEqualType) cmpeq(len, isFloat, isSigned, bank1, bank2);
    if (T == OperNotEqualType) cmpne(len, isFloat, isSigned, bank1, bank2);
    if (T == OperLTType) cmplt(len, isFloat, isSigned, bank1, bank2);
    if (T == OperGTType) cmpgt(len, isFloat, isSigned, bank1, bank2);
    if (T == OperLEType) cmple(len, isFloat, isSigned, bank1, bank2);
    if (T == OperGEType) cmpge(len, isFloat, isSigned, bank1, bank2);
}


void Codegen::f16Tof32(int srcIndex,int bank) {
    loadPointer();
    int INSTR0[] = {0xc5, 0xfe, 0x6f, 0x41, 0x04};//        vmovqdu x(%rsi),%ymm0
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc5, 0xfe, 0x6f, 0x49, 0x14};//        vmovqdu x(%rsi),%ymm1
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc4 ,0xe2 ,0x7d ,0x13 ,0xc0};//       	vcvtph2ps %xmm0,%ymm0
    add(INSTR2, size(INSTR2));
    int INSTR3[] = {0xc4 ,0xe2 ,0x7d ,0x13 ,0xc9};//       	vcvtph2ps %xmm1,%ymm1
    add(INSTR3, size(INSTR3));
}

void Codegen::loadCenter(uint64_t v) {
    float x, y;
    Uint64ToLocation((int64_t)v,x,y);
    auto CenterX = (double)x*(M_PI / 180.);
    auto CenterY = (double)y*(M_PI / 180.);
    __m256i center = _mm256_castpd_si256(_mm256_set_pd(CenterY, CenterX, CenterY, CenterX));
    int INSTR0[] = {0xc5 ,0xfe ,0x6f ,0x15};//	vmovdqu 0x123456(%rip),%ymm2        # 0x12345e
    add(INSTR0, size(INSTR0));
    addConstant(center,true);
    __m256i cosCenter = _mm256_castps_si256(_mm256_set_ps(1, cosf((float)CenterY), 1, cosf((float)CenterY),1, cosf((float)CenterY), 1, cosf((float)CenterY)));
    int INSTR1[] = { 0xc5 ,0xfe ,0x6f ,0x1d};// 	vmovdqu 0x123456(%rip),%ymm3        # 0x123466
    add(INSTR1, size(INSTR1));
    addConstant(cosCenter,true);
}

void Codegen::distanceSphere(int bank1, int targetLen) {
    if (inValues.empty()) return; // ????
    int INSTR0[] = { 0x57, 0x56 }; //  push   %rdi, push rsi
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc5 ,0xff ,0xf0 ,0x86}; // vlddqu 0x1234(%rsi),%ymm0
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank1,0);
    int INSTR2[] = {0xc5 ,0xff ,0xf0 ,0x8e}; // vlddqu 0x1234(%rsi),%ymm1
    add(INSTR2, size(INSTR2));
    addMemoryOffset(bank1,1);
    loadCenter(inValues.at(0));
    int INSTR5[] = {  0x48, 0xb8 };
    add(INSTR5, size(INSTR5));
    addMemoryOffsetL((uint64_t)cal_distance_sphere);
    int INSTR6[] = { 0xff ,0xd0};           // call %rax
    add(INSTR6, size(INSTR6));
    int INSTR7[] = {0x5e,0x56,0xc5 ,0xfd ,0x7f ,0x86}; // vmovdqa %ymm0,0x1234(%rsi)
    add(INSTR7, size(INSTR7));
    addMemoryOffset(bank1,0);

    int INSTR8[] = {0xc5 ,0xff ,0xf0 ,0x86}; // vlddqu 0x1234(%rsi),%ymm0
    add(INSTR8, size(INSTR8));
    addMemoryOffset(bank1,2);
    int INSTR9[] = {0xc5 ,0xff ,0xf0 ,0x8e}; // vlddqu 0x1234(%rsi),%ymm1
    add(INSTR9, size(INSTR9));
    addMemoryOffset(bank1,3);
    loadCenter(inValues.at(0));
    add(INSTR5, size(INSTR5));
    addMemoryOffsetL((uint64_t)cal_distance_sphere);
    add(INSTR6, size(INSTR6));
    int INSTR10[] = {0x5e,0x5f, 0xc5 ,0xfd ,0x7f ,0x86}; // vmovdqa %ymm0,0x1234(%rsi)
    add(INSTR10, size(INSTR10));
    addMemoryOffset(bank1,1);

    if (targetLen == 64) {
        cvt_float_double(bank1,true);
        save(64,bank1);
    }
}

void Codegen::dumpFloat(int bank, void *banks) {
    auto q = (float*)(((uint8_t*)banks)+bank*256);
    for (int i=0; i< 16; i++) cout << hex << q[i] << " ";
    cout << endl;
}

void Codegen::cvt_unsigned_64_128(int bank, bool reload) {
    __m256i maskcvt_64_128 = _mm256_set_epi64x(0x000000000,(long long)0xffffffffffffffffL,0x000000000,(long long)0xffffffffffffffffL);

    if (reload) {
        load_src64(bank);
    }
    int INSTR0[] = {0xc4 ,0xe3 ,0xfd ,0x01 ,0xfb ,0xfa};//    	vpermpd $0xfa,%ymm3,%ymm7
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc4 ,0xe3 ,0xfd ,0x01 ,0xf3 ,0x50};//    	vpermpd $0x50,%ymm3,%ymm6
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc4 ,0xe3 ,0xfd ,0x01 ,0xea ,0xfa};//    	vpermpd $0xfa,%ymm2,%ymm5
    add(INSTR2, size(INSTR2));
    int INSTR3[] = {0xc4 ,0xe3 ,0xfd ,0x01 ,0xe2 ,0x50};//    	vpermpd $0x50,%ymm2,%ymm4
    add(INSTR3, size(INSTR3));
    int INSTR4[] = {0xc4 ,0xe3 ,0xfd ,0x01 ,0xd9 ,0xfa};//    	vpermpd $0xfa,%ymm1,%ymm3
    add(INSTR4, size(INSTR4));
    int INSTR5[] = {0xc4 ,0xe3 ,0xfd ,0x01 ,0xd1 ,0x50};//    	vpermpd $0x50,%ymm1,%ymm2
    add(INSTR5, size(INSTR5));
    int INSTR6[] = {0xc4 ,0xe3 ,0xfd ,0x01 ,0xc8 ,0xfa};//    	vpermpd $0xfa,%ymm0,%ymm1
    add(INSTR6, size(INSTR6));
    int INSTR7[] = {0xc4 ,0xe3 ,0xfd ,0x01 ,0xc0 ,0x50};//    	vpermpd $0x50,%ymm0,%ymm0
    add(INSTR7, size(INSTR7));
    int INSTR8[] = {0xc5 ,0x7d ,0x6f ,0x05};            // maskcvt_64_128(%rip),%ymm8
    add(INSTR8, size(INSTR8));
    addConstant(maskcvt_64_128, true);
    int INSTR9[] = {0xc4 ,0xc1 ,0x7d ,0xdb ,0xc0};// 	vpand %ymm8,%ymm0,%ymm0
    add(INSTR9, size(INSTR9));
    int INSTR10[] = {0xc4 ,0xc1 ,0x75 ,0xdb ,0xc8};// 	vpand %ymm8,%ymm1,%ymm1
    add(INSTR10, size(INSTR10));
    int INSTR11[] = {0xc4 ,0xc1 ,0x6d ,0xdb ,0xd0};// 	vpand %ymm8,%ymm2,%ymm2
    add(INSTR11, size(INSTR11));
    int INSTR12[] = {0xc4 ,0xc1 ,0x65 ,0xdb ,0xd8};// 	vpand %ymm8,%ymm3,%ymm3
    add(INSTR12, size(INSTR12));
    int INSTR13[] = {0xc4 ,0xc1 ,0x5d ,0xdb ,0xe0};// 	vpand %ymm8,%ymm4,%ymm4
    add(INSTR13, size(INSTR13));
    int INSTR14[] = {0xc4 ,0xc1 ,0x55 ,0xdb ,0xe8};// 	vpand %ymm8,%ymm5,%ymm5
    add(INSTR14, size(INSTR14));
    int INSTR15[] = {0xc4 ,0xc1 ,0x4d ,0xdb ,0xf0};// 	vpand %ymm8,%ymm6,%ymm6
    add(INSTR15, size(INSTR15));
    int INSTR16[] = {0xc4 ,0xc1 ,0x45 ,0xdb ,0xf8};// 	vpand %ymm8,%ymm7,%ymm7
    add(INSTR16, size(INSTR16));
}

void Codegen::cvt_signed_64_128(int bank, bool reload) {
    static __m256i shufcvt_64_128 = _mm256_set_epi8(7,7,7,7,7,7,7,7,7,6,5,4,3,2,1,0,7,7,7,7,7,7,7,7,7,6,5,4,3,2,1,0);
    static __m256i shiftcvt_64_128= _mm256_set_epi32(31,31,0,0,31,31,0,0);

    if (reload) {
        load_src64(bank);
    }
    int INSTR0[] = {0xc4 ,0xe3 ,0xfd ,0x01 ,0xfb ,0xfa};//    	vpermpd $0xfa,%ymm3,%ymm7
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0xc4 ,0xe3 ,0xfd ,0x01 ,0xf3 ,0x50};//    	vpermpd $0x50,%ymm3,%ymm6
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc4 ,0xe3 ,0xfd ,0x01 ,0xea ,0xfa};//    	vpermpd $0xfa,%ymm2,%ymm5
    add(INSTR2, size(INSTR2));
    int INSTR3[] = {0xc4 ,0xe3 ,0xfd ,0x01 ,0xe2 ,0x50};//    	vpermpd $0x50,%ymm2,%ymm4
    add(INSTR3, size(INSTR3));
    int INSTR4[] = {0xc4 ,0xe3 ,0xfd ,0x01 ,0xd9 ,0xfa};//    	vpermpd $0xfa,%ymm1,%ymm3
    add(INSTR4, size(INSTR4));
    int INSTR5[] = {0xc4 ,0xe3 ,0xfd ,0x01 ,0xd1 ,0x50};//    	vpermpd $0x50,%ymm1,%ymm2
    add(INSTR5, size(INSTR5));
    int INSTR6[] = {0xc4 ,0xe3 ,0xfd ,0x01 ,0xc8 ,0xfa};//    	vpermpd $0xfa,%ymm0,%ymm1
    add(INSTR6, size(INSTR6));
    int INSTR7[] = {0xc4 ,0xe3 ,0xfd ,0x01 ,0xc0 ,0x50};//    	vpermpd $0x50,%ymm0,%ymm0
    add(INSTR7, size(INSTR7));

    int INSTR8[] = {0xc5 ,0x7d ,0x6f ,0x05};// 	vmovdqa 0x0(%rip),%ymm8        # 0x8
    add(INSTR8, size(INSTR8));
    addConstant(shufcvt_64_128, true);
    int INSTR9[] = {0xc5 ,0x7d ,0x6f ,0x0d};// 	vmovdqa 0x0(%rip),%ymm9        # 0x10
    add(INSTR9, size(INSTR9));
    int INSTR10[] = {0xc4 ,0xc2 ,0x7d ,0x00 ,0xc0};//       	vpshufb %ymm8,%ymm0,%ymm0
    addConstant(shiftcvt_64_128, true);
    add(INSTR10, size(INSTR10));
    int INSTR11[] = {0xc4 ,0xc2 ,0x75 ,0x00 ,0xc8};//       	vpshufb %ymm8,%ymm1,%ymm1
    add(INSTR11, size(INSTR11));
    int INSTR12[] = {0xc4 ,0xc2 ,0x6d ,0x00 ,0xd0};//       	vpshufb %ymm8,%ymm2,%ymm2
    add(INSTR12, size(INSTR12));
    int INSTR13[] = {0xc4 ,0xc2 ,0x65 ,0x00 ,0xd8};//       	vpshufb %ymm8,%ymm3,%ymm3
    add(INSTR13, size(INSTR13));
    int INSTR14[] = {0xc4 ,0xc2 ,0x5d ,0x00 ,0xe0};//       	vpshufb %ymm8,%ymm4,%ymm4
    add(INSTR14, size(INSTR14));
    int INSTR15[] = {0xc4 ,0xc2 ,0x55 ,0x00 ,0xe8};//       	vpshufb %ymm8,%ymm5,%ymm5
    add(INSTR15, size(INSTR15));
    int INSTR16[] = {0xc4 ,0xc2 ,0x4d ,0x00 ,0xf0};//       	vpshufb %ymm8,%ymm6,%ymm6
    add(INSTR16, size(INSTR16));
    int INSTR17[] = {0xc4 ,0xc2 ,0x45 ,0x00 ,0xf8};//       	vpshufb %ymm8,%ymm7,%ymm7
    add(INSTR17, size(INSTR17));
    int INSTR18[] = {0xc4 ,0xc2 ,0x7d ,0x46 ,0xc1};//       	vpsravd %ymm9,%ymm0,%ymm0
    add(INSTR18, size(INSTR18));
    int INSTR19[] = {0xc4 ,0xc2 ,0x75 ,0x46 ,0xc9};//       	vpsravd %ymm9,%ymm1,%ymm1
    add(INSTR19, size(INSTR19));
    int INSTR20[] = {0xc4 ,0xc2 ,0x6d ,0x46 ,0xd1};//       	vpsravd %ymm9,%ymm2,%ymm2
    add(INSTR20, size(INSTR20));
    int INSTR21[] = {0xc4 ,0xc2 ,0x65 ,0x46 ,0xd9};//       	vpsravd %ymm9,%ymm3,%ymm3
    add(INSTR21, size(INSTR21));
    int INSTR22[] = {0xc4 ,0xc2 ,0x5d ,0x46 ,0xe1};//       	vpsravd %ymm9,%ymm4,%ymm4
    add(INSTR22, size(INSTR22));
    int INSTR23[] = {0xc4 ,0xc2 ,0x55 ,0x46 ,0xe9};//       	vpsravd %ymm9,%ymm5,%ymm5
    add(INSTR23, size(INSTR23));
    int INSTR24[] = {0xc4 ,0xc2 ,0x4d ,0x46 ,0xf1};//       	vpsravd %ymm9,%ymm6,%ymm6
    add(INSTR24, size(INSTR24));
    int INSTR25[] = {0xc4 ,0xc2 ,0x45 ,0x46 ,0xf9};//       	vpsravd %ymm9,%ymm7,%ymm7
    add(INSTR25, size(INSTR25));
}

void Codegen::cvt_signed_32_float(int bank, bool reload) {
    if (reload) {
        load_src32(bank);
    }
    int INSTR0[] = {0xc5 ,0xfc ,0x5b ,0xc0};//          	vcvtdq2ps %ymm0,%ymm0
    int INSTR1[] = {0xc5 ,0xfc ,0x5b ,0xc9};//          	vcvtdq2ps %ymm1,%ymm1
    add(INSTR0, size(INSTR0));
    add(INSTR1, size(INSTR1));
}

void Codegen::cvt_signed_32_double(int bank, bool reload) {
    if (reload) {
        load_src32(bank);
    }
    int INSTR0[] = {0xc5 ,0xf5 ,0xeb ,0xd1};//          	vpor   %ymm1,%ymm1,%ymm2
    int INSTR1[] = {0xc4 ,0xe3 ,0x7d ,0x06 ,0xc8 ,0x01};//    	vperm2f128 $0x1,%ymm0,%ymm0,%ymm1
    int INSTR2[] = {0xc4 ,0xe3 ,0x6d ,0x06 ,0xda ,0x01};//    	vperm2f128 $0x1,%ymm2,%ymm2,%ymm3
    int INSTR3[] = {0xc5 ,0xfe ,0xe6 ,0xc0};//          	vcvtdq2pd %xmm0,%ymm0
    int INSTR4[] = {0xc5 ,0xfe ,0xe6 ,0xc9};//          	vcvtdq2pd %xmm1,%ymm1
    int INSTR5[] = {0xc5 ,0xfe ,0xe6 ,0xd2};//          	vcvtdq2pd %xmm2,%ymm2
    int INSTR6[] = {0xc5 ,0xfe ,0xe6 ,0xdb};//          	vcvtdq2pd %xmm3,%ymm3
    add(INSTR0, size(INSTR0));
    add(INSTR1, size(INSTR1));
    add(INSTR2, size(INSTR2));
    add(INSTR3, size(INSTR3));
    add(INSTR4, size(INSTR4));
    add(INSTR5, size(INSTR5));
    add(INSTR6, size(INSTR6));
}

void Codegen::cvt_signed_64_double(int bank, bool reload) {
    if (!reload) {
        save(64,bank);
    }
    uint64_t offset = bank*256;
    int INSTR0[] = {0xc5 ,0xc0 ,0x57 ,0xff};//  vxorps %xmm7,%xmm7,%xmm7
    add(INSTR0, size(INSTR0));
    {
        int INSTR1[] = {0xc4 ,0xe1 ,0xc3 ,0x2a ,0x86};// 	vcvtsi2sdq 0x1234(%rsi),%xmm7,%xmm0
        add(INSTR1, size(INSTR1));
        addMemoryOffset(offset+0);
        int INSTR2[] = {0xc4 ,0xe1 ,0xc3 ,0x2a ,0x8e};// 	vcvtsi2sdq 0x1234(%rsi),%xmm7,%xmm1
        add(INSTR2, size(INSTR2));
        addMemoryOffset(offset+1*8);
        int INSTR3[] = {0xc4 ,0xe1 ,0xc3 ,0x2a ,0x96};// 	vcvtsi2sdq 0x1234(%rsi),%xmm7,%xmm2
        add(INSTR3, size(INSTR3));
        addMemoryOffset(offset+2*8);
        int INSTR4[] = {0xc4 ,0xe1 ,0xc3 ,0x2a ,0x9e};// 	vcvtsi2sdq 0x1234(%rsi),%xmm7,%xmm3
        add(INSTR4, size(INSTR4));
        addMemoryOffset(offset+3*8);
        int INSTR5[] = {0xc4 ,0xe1 ,0xc3 ,0x2a ,0xa6};// 	vcvtsi2sdq 0x1234(%rsi),%xmm7,%xmm4
        add(INSTR5, size(INSTR5));
        addMemoryOffset(offset+4*8);
        int INSTR6[] = {0xc4 ,0xe1 ,0xc3 ,0x2a ,0xae};// 	vcvtsi2sdq 0x1234(%rsi),%xmm7,%xmm5
        add(INSTR6, size(INSTR6));
        addMemoryOffset(offset+5*8);
        int INSTR7[] = {0xc4 ,0xe1 ,0xc3 ,0x2a ,0xb6};// 	vcvtsi2sdq 0x1234(%rsi),%xmm7,%xmm6
        add(INSTR7, size(INSTR7));
        addMemoryOffset(offset+6*8);
        int INSTR8[] = {0xc4 ,0xe1 ,0xc3 ,0x2a ,0xbe};// 	vcvtsi2sdq 0x1234(%rsi),%xmm7,%xmm7
        add(INSTR8, size(INSTR8));
        addMemoryOffset(offset+7*8);
        int INSTR9[] = {0xc5 ,0xfb ,0x11 ,0x86};// 	vmovsd %xmm0,0x1234(%rsi)
        add(INSTR9, size(INSTR9));
        addMemoryOffset(offset+0*8);
        int INSTR10[] = {0xc5 ,0xfb ,0x11 ,0x8e};// vmovsd %xmm1,0x1234(%rsi)
        add(INSTR10, size(INSTR10));
        addMemoryOffset(offset+1*8);
        int INSTR11[] = {0xc5 ,0xfb ,0x11 ,0x96};// vmovsd %xmm2,0x1234(%rsi)
        add(INSTR11, size(INSTR11));
        addMemoryOffset(offset+2*8);
        int INSTR12[] = {0xc5 ,0xfb ,0x11 ,0x9e};// vmovsd %xmm3,0x1234(%rsi)
        add(INSTR12, size(INSTR12));
        addMemoryOffset(offset+3*8);
        int INSTR13[] = {0xc5 ,0xfb ,0x11 ,0xa6};// vmovsd %xmm4,0x1234(%rsi)
        add(INSTR13, size(INSTR13));
        addMemoryOffset(offset+4*8);
        int INSTR14[] = {0xc5 ,0xfb ,0x11 ,0xae};// vmovsd %xmm5,0x1234(%rsi)
        add(INSTR14, size(INSTR14));
        addMemoryOffset(offset+5*8);
        int INSTR15[] = {0xc5 ,0xfb ,0x11 ,0xb6};// vmovsd %xmm6,0x1234(%rsi)
        add(INSTR15, size(INSTR15));
        addMemoryOffset(offset+6*8);
        int INSTR16[] = {0xc5 ,0xfb ,0x11 ,0xbe};// vmovsd %xmm7,0x1234(%rsi)
        add(INSTR16, size(INSTR16));
        addMemoryOffset(offset+7*8);
    }
    {
        int INSTR1[] = {0xc4 ,0xe1 ,0xc3 ,0x2a ,0x86};// 	vcvtsi2sdq 0x1234(%rsi),%xmm7,%xmm0
        add(INSTR1, size(INSTR1));
        addMemoryOffset(offset+8*8);
        int INSTR2[] = {0xc4 ,0xe1 ,0xc3 ,0x2a ,0x8e};// 	vcvtsi2sdq 0x1234(%rsi),%xmm7,%xmm1
        add(INSTR2, size(INSTR2));
        addMemoryOffset(offset+9*8);
        int INSTR3[] = {0xc4 ,0xe1 ,0xc3 ,0x2a ,0x96};// 	vcvtsi2sdq 0x1234(%rsi),%xmm7,%xmm2
        add(INSTR3, size(INSTR3));
        addMemoryOffset(offset+10*8);
        int INSTR4[] = {0xc4 ,0xe1 ,0xc3 ,0x2a ,0x9e};// 	vcvtsi2sdq 0x1234(%rsi),%xmm7,%xmm3
        add(INSTR4, size(INSTR4));
        addMemoryOffset(offset+11*8);
        int INSTR5[] = {0xc4 ,0xe1 ,0xc3 ,0x2a ,0xa6};// 	vcvtsi2sdq 0x1234(%rsi),%xmm7,%xmm4
        add(INSTR5, size(INSTR5));
        addMemoryOffset(offset+12*8);
        int INSTR6[] = {0xc4 ,0xe1 ,0xc3 ,0x2a ,0xae};// 	vcvtsi2sdq 0x1234(%rsi),%xmm7,%xmm5
        add(INSTR6, size(INSTR6));
        addMemoryOffset(offset+13*8);
        int INSTR7[] = {0xc4 ,0xe1 ,0xc3 ,0x2a ,0xb6};// 	vcvtsi2sdq 0x1234(%rsi),%xmm7,%xmm6
        add(INSTR7, size(INSTR7));
        addMemoryOffset(offset+14*8);
        int INSTR8[] = {0xc4 ,0xe1 ,0xc3 ,0x2a ,0xbe};// 	vcvtsi2sdl 0x1234(%rsi),%xmm7,%xmm7
        add(INSTR8, size(INSTR8));
        addMemoryOffset(offset+15*8);
        int INSTR9[] = {0xc5 ,0xfb ,0x11 ,0x86};// 	vmovsd %xmm0,0x1234(%rsi)
        add(INSTR9, size(INSTR9));
        addMemoryOffset(offset+8*8);
        int INSTR10[] = {0xc5 ,0xfb ,0x11 ,0x8e};// vmovsd %xmm1,0x1234(%rsi)
        add(INSTR10, size(INSTR10));
        addMemoryOffset(offset+9*8);
        int INSTR11[] = {0xc5 ,0xfb ,0x11 ,0x96};// vmovsd %xmm2,0x1234(%rsi)
        add(INSTR11, size(INSTR11));
        addMemoryOffset(offset+10*8);
        int INSTR12[] = {0xc5 ,0xfb ,0x11 ,0x9e};// vmovsd %xmm3,0x1234(%rsi)
        add(INSTR12, size(INSTR12));
        addMemoryOffset(offset+11*8);
        int INSTR13[] = {0xc5 ,0xfb ,0x11 ,0xa6};// vmovsd %xmm4,0x1234(%rsi)
        add(INSTR13, size(INSTR13));
        addMemoryOffset(offset+12*8);
        int INSTR14[] = {0xc5 ,0xfb ,0x11 ,0xae};// vmovsd %xmm5,0x1234(%rsi)
        add(INSTR14, size(INSTR14));
        addMemoryOffset(offset+13*8);
        int INSTR15[] = {0xc5 ,0xfb ,0x11 ,0xb6};// vmovsd %xmm6,0x1234(%rsi)
        add(INSTR15, size(INSTR15));
        addMemoryOffset(offset+14*8);
        int INSTR16[] = {0xc5 ,0xfb ,0x11 ,0xbe};// vmovsd %xmm7,0x1234(%rsi)
        add(INSTR16, size(INSTR16));
        addMemoryOffset(offset+15*8);
    }
}

void Codegen::cvt_float_double(int bank, bool reload) {
    if (reload) {
        load_src32(bank);
    }
    int INSTR0[] = {0xc5 ,0xf5 ,0xeb ,0xd1};//          	vpor   %ymm1,%ymm1,%ymm2
    int INSTR1[] = {0xc4 ,0xe3 ,0x7d ,0x06 ,0xc8 ,0x01};//    	vperm2f128 $0x1,%ymm0,%ymm0,%ymm1
    int INSTR2[] = {0xc4 ,0xe3 ,0x6d ,0x06 ,0xda ,0x01};//    	vperm2f128 $0x1,%ymm2,%ymm2,%ymm3
    int INSTR3[] = {0xc5 ,0xfc ,0x5a ,0xc0};//          	vcvtps2pd %xmm0,%ymm0
    int INSTR4[] = {0xc5 ,0xfc ,0x5a ,0xc9};//          	vcvtps2pd %xmm1,%ymm1
    int INSTR5[] = {0xc5 ,0xfc ,0x5a ,0xd2};//          	vcvtps2pd %xmm2,%ymm2
    int INSTR6[] = {0xc5 ,0xfc ,0x5a ,0xdb};//          	vcvtps2pd %xmm3,%ymm3
    add(INSTR0, size(INSTR0));
    add(INSTR1, size(INSTR1));
    add(INSTR2, size(INSTR2));
    add(INSTR3, size(INSTR3));
    add(INSTR4, size(INSTR4));
    add(INSTR5, size(INSTR5));
    add(INSTR6, size(INSTR6));
}

static double signed_128_double_C(const __int128_t *a) {
    return (double)*a;
}

// 210 cycles on zen 2
void Codegen::cvt_signed_128_double(int bank, bool reload) {
    if (!reload) {
        save(128,bank);
    }
    auto p = signed_128_double_C;
    int INSTR0[] = { 0x56, 0x57, 0x41, 0x54, 0x41, 0x55, 0x49, 0x89, 0xf4}; // push   %rsi; push   %rdi; push   %r12; push %r13, mov  %rsi,%r12
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {  0x48, 0xb8 };
    add(INSTR1, size(INSTR1));
    addMemoryOffsetL((uint64_t)p);
    int INSTR2[] = { 0x49 ,0x89 ,0xc5}; //  mov    %rax,%r13
    add(INSTR2, size(INSTR2));
    for (int i=0; i< 16; i++) {
        int INSTR3[] = { 0x49 ,0x8d ,0xbc ,0x24}; // lea    0x123(%r12),%rdi
        add(INSTR3, size(INSTR3));
        addMemoryOffset128(bank, i);
        int INSTR5[] = { 0x41 ,0xff ,0xd5};// call %rax13
        add(INSTR5, size(INSTR5));
        int INSTR4[] = {0xc4 ,0xc1 ,0x7b ,0x11 ,0x84 ,0x24};//         	vmovsd %xmm0,0x12456(%r12)
        add(INSTR4, size(INSTR4));
        addMemoryOffset64(bank, i);
    }
    int INSTR6[] = { 0x41, 0x5d, 0x41, 0x5c, 0x5f, 0x5e};   // pops
    add(INSTR6, size(INSTR6));
}

void Codegen::cvt_unsigned_32_64(int bank, bool reload) {
    if (reload) {
        load_src32(bank);
    }
    int INSTR0[] = {0xc5 ,0xf5 ,0xeb ,0xd1};//          	vpor   %ymm1,%ymm1,%ymm2
    int INSTR1[] = {0xc4 ,0xe3 ,0x7d ,0x46 ,0xc8 ,0x01};//  vperm2i128 $0x1,%ymm0,%ymm0,%ymm1
    int INSTR2[] = {0xc4 ,0xe3 ,0x6d ,0x46 ,0xda ,0x01};//  vperm2i128 $0x1,%ymm2,%ymm2,%ymm3
    int INSTR3[] = {0xc4 ,0xe2 ,0x7d ,0x35 ,0xc0};//       	vpmovzxdq %xmm0,%ymm0
    int INSTR4[] = {0xc4 ,0xe2 ,0x7d ,0x35 ,0xc9};//       	vpmovzxdq %xmm1,%ymm1
    int INSTR5[] = {0xc4 ,0xe2 ,0x7d ,0x35 ,0xd2};//       	vpmovzxdq %xmm2,%ymm2
    int INSTR6[] = {0xc4 ,0xe2 ,0x7d ,0x35 ,0xdb};//       	vpmovzxdq %xmm3,%ymm3=
    add(INSTR0, size(INSTR0));
    add(INSTR1, size(INSTR1));
    add(INSTR2, size(INSTR2));
    add(INSTR3, size(INSTR3));
    add(INSTR4, size(INSTR4));
    add(INSTR5, size(INSTR5));
    add(INSTR6, size(INSTR6));
}

void Codegen::cvt_signed_32_64(int bank, bool reload) {
    if (reload) {
        load_src32(bank);
    }
    int INSTR0[] = {0xc5 ,0xf5 ,0xeb ,0xd1};//          	vpor   %ymm1,%ymm1,%ymm2
    int INSTR1[] = {0xc4 ,0xe3 ,0x7d ,0x46 ,0xc8 ,0x01};//  vperm2i128 $0x1,%ymm0,%ymm0,%ymm1
    int INSTR2[] = {0xc4 ,0xe3 ,0x6d ,0x46 ,0xda ,0x01};//  vperm2i128 $0x1,%ymm2,%ymm2,%ymm3
    int INSTR3[] = {0xc4 ,0xe2 ,0x7d ,0x25 ,0xc0};//       	vpmovszxdq %xmm0,%ymm0
    int INSTR4[] = {0xc4 ,0xe2 ,0x7d ,0x25 ,0xc9};//       	vpmovsxdq %xmm1,%ymm1
    int INSTR5[] = {0xc4 ,0xe2 ,0x7d ,0x25 ,0xd2};//       	vpmovsxdq %xmm2,%ymm2
    int INSTR6[] = {0xc4 ,0xe2 ,0x7d ,0x25 ,0xdb};//       	vpmovsxdq %xmm3,%ymm3=
    add(INSTR0, size(INSTR0));
    add(INSTR1, size(INSTR1));
    add(INSTR2, size(INSTR2));
    add(INSTR3, size(INSTR3));
    add(INSTR4, size(INSTR4));
    add(INSTR5, size(INSTR5));
    add(INSTR6, size(INSTR6));
}

void Codegen::in32(int bank) {
    load_src32(bank);
    for (int i=0; i< inValues.size(); i++) {
        int INSTR0[] = {0x48 ,0xc7 ,0xc0 };// 	            mov    $0x12134,%rax
        add(INSTR0, size(INSTR0));
        addMemoryOffset(inValues.at(i));
        int INSTR1[] = {0xc4 ,0x61 ,0xf9 ,0x6e ,0xd0};//    vmovq  %rax,%xmm10
        add(INSTR1, size(INSTR1));
        int INSTR2[] = {0xc4 ,0x42 ,0x7d ,0x58 ,0xd2};//     vpbroadcastd %xmm10,%ymm10
        add(INSTR2, size(INSTR2));
        if (inValues.size() == 1) {
            int INSTR3[] = {0xc5 ,0xad ,0x76 ,0xc0};//         	vpcmpeqd %ymm0,%ymm10,%ymm0
            add(INSTR3, size(INSTR3));
            int INSTR4[] = {0xc5 ,0xad ,0x76 ,0xc9};//         	vpcmpeqd %ymm1,%ymm10,%ymm1
            add(INSTR4, size(INSTR4));
        } else {
            if (i == 0) {
                int INSTR3[] = {0xc5 ,0x2d ,0x76 ,0xc0};//         	vpcmpeqd %ymm0,%ymm10,%ymm8
                add(INSTR3, size(INSTR3));
                int INSTR4[] = {0xc5 ,0x2d ,0x76 ,0xc9};//         	vpcmpeqd %ymm1,%ymm10,%ymm9
                add(INSTR4, size(INSTR4));
            } else  {
                int INSTR3[] = {0xc5 ,0x2d ,0x76 ,0xd8};//         	vpcmpeqd %ymm0,%ymm10,%ymm11
                add(INSTR3, size(INSTR3));
                int INSTR4[] = {0xc5 ,0x2d ,0x76 ,0xe1};//         	vpcmpeqd %ymm1,%ymm10,%ymm12
                add(INSTR4, size(INSTR4));
                if (i < inValues.size()-1) {
                    int INSTR7[] = {0xc4 ,0x41 ,0x25 ,0xeb ,0xc0};//       	vpor   %ymm8,%ymm11,%ymm8
                    add(INSTR7, size(INSTR7));
                    int INSTR8[] = {0xc4 ,0x41 ,0x1d ,0xeb ,0xc9};//       	vpor   %ymm9,%ymm12,%ymm9
                    add(INSTR8, size(INSTR8));
                } else {
                    int INSTR9[] = {0xc4 ,0xc1 ,0x25 ,0xeb ,0xc0};//       	vpor   %ymm8,%ymm11,%ymm0
                    add(INSTR9, size(INSTR9));
                    int INSTR10[] = {0xc4 ,0xc1 ,0x1d ,0xeb ,0xc9};//       vpor   %ymm9,%ymm12,%ymm0
                    add(INSTR10, size(INSTR10));
                }
            }
        }
    }
    merge_cmp32();
    merge_null();
}

void Codegen::notin32(int bank) {
    if (inValues.empty()) {
        int INSTR11[] = { 0x31, 0xc0, 0xf7 ,0xd0}; //      xorl %eax,%eax  ; not    %eax -- 0xffff
        add(INSTR11, size(INSTR11));
    } else {
        load_src32(bank);
        for (int i=0; i< inValues.size(); i++) {
            int INSTR0[] = {0x48 ,0xc7 ,0xc0 };// 	            mov    $0x12134,%rax
            add(INSTR0, size(INSTR0));
            addMemoryOffset(inValues.at(i));
            int INSTR1[] = {0xc4 ,0x61 ,0xf9 ,0x6e ,0xd0};//    vmovq  %rax,%xmm10
            add(INSTR1, size(INSTR1));
            int INSTR2[] = {0xc4 ,0x42 ,0x7d ,0x58 ,0xd2};//     vpbroadcastd %xmm10,%ymm10
            add(INSTR2, size(INSTR2));
            if (inValues.size() == 1) {
                int INSTR3[] = {0xc5 ,0xad ,0x76 ,0xc0};//         	vpcmpeqd %ymm0,%ymm10,%ymm0
                add(INSTR3, size(INSTR3));
                int INSTR4[] = {0xc5 ,0xad ,0x76 ,0xc9};//         	vpcmpeqd %ymm1,%ymm10,%ymm1
                add(INSTR4, size(INSTR4));
            } else {
                if (i == 0) {
                    int INSTR3[] = {0xc5 ,0x2d ,0x76 ,0xc0};//         	vpcmpeqd %ymm0,%ymm10,%ymm8
                    add(INSTR3, size(INSTR3));
                    int INSTR4[] = {0xc5 ,0x2d ,0x76 ,0xc9};//         	vpcmpeqd %ymm1,%ymm10,%ymm9
                    add(INSTR4, size(INSTR4));
                } else  {
                    int INSTR3[] = {0xc5 ,0x2d ,0x76 ,0xd8};//         	vpcmpeqd %ymm0,%ymm10,%ymm11
                    add(INSTR3, size(INSTR3));
                    int INSTR4[] = {0xc5 ,0x2d ,0x76 ,0xe1};//         	vpcmpeqd %ymm1,%ymm10,%ymm12
                    add(INSTR4, size(INSTR4));
                    if (i < inValues.size()-1) {
                        int INSTR7[] = {0xc4 ,0x41 ,0x25 ,0xeb ,0xc0};//       	vpor   %ymm8,%ymm11,%ymm8
                        add(INSTR7, size(INSTR7));
                        int INSTR8[] = {0xc4 ,0x41 ,0x1d ,0xeb ,0xc9};//       	vpor   %ymm9,%ymm12,%ymm9
                        add(INSTR8, size(INSTR8));
                    } else {
                        int INSTR9[] = {0xc4 ,0xc1 ,0x25 ,0xeb ,0xc0};//       	vpor   %ymm8,%ymm11,%ymm0
                        add(INSTR9, size(INSTR9));
                        int INSTR10[] = {0xc4 ,0xc1 ,0x1d ,0xeb ,0xc9};//       vpor   %ymm9,%ymm12,%ymm0
                        add(INSTR10, size(INSTR10));
                    }
                }
            }
        }
        merge_cmp32();
        int INSTR11[] = { 0xf7 ,0xd0}; //             	not    %eax
        add(INSTR11, size(INSTR11));
    }
    merge_null();
}

void Codegen::in64(int bank) {
    load_src64(bank);
    for (int i = 0; i < inValues.size(); i++) {
        int INSTR0[] = {0x48, 0xb8};// 	            movabs    $0x12134,%rax
        add(INSTR0, size(INSTR0));
        addMemoryOffsetL(inValues.at(i));
        int INSTR1[] = {0xc4, 0x61, 0xf9, 0x6e, 0xc0};//    vmovq  %rax,%xmm8
        add(INSTR1, size(INSTR1));
        int INSTR2[] = {0xc4, 0x42, 0x7d, 0x59, 0xc0};//     vpbroadcastq %xmm8,%ymm8
        add(INSTR2, size(INSTR2));
        if (inValues.size() == 1) {
            int INSTR6[] = {0xc4, 0xe2, 0x3d, 0x29, 0xc0};//          	vpcmpeqq %ymm0,%ymm8,%ymm4
            add(INSTR6, size(INSTR6));
            int INSTR7[] = {0xc4, 0xe2, 0x3d, 0x29, 0xc9};//          	vpcmpeqq %ymm1,%ymm8,%ymm5
            add(INSTR7, size(INSTR7));
            int INSTR8[] = {0xc4, 0xe2, 0x3d, 0x29, 0xd2};//          	vpcmpeqq %ymm2,%ymm8,%ymm6
            add(INSTR8, size(INSTR8));
            int INSTR9[] = {0xc4, 0xe2, 0x3d, 0x29, 0xdb};//          	vpcmpeqq %ymm3,%ymm8,%ymm7
            add(INSTR9, size(INSTR9));
        } else {
            if (i == 0) {
                int INSTR6[] = {0xc4, 0xe2, 0x3d, 0x29, 0xe0};//          	vpcmpeqq %ymm0,%ymm8,%ymm4
                add(INSTR6, size(INSTR6));
                int INSTR7[] = {0xc4, 0xe2, 0x3d, 0x29, 0xe9};//          	vpcmpeqq %ymm1,%ymm8,%ymm5
                add(INSTR7, size(INSTR7));
                int INSTR8[] = {0xc4, 0xe2, 0x3d, 0x29, 0xf2};//          	vpcmpeqq %ymm2,%ymm8,%ymm6
                add(INSTR8, size(INSTR8));
                int INSTR9[] = {0xc4, 0xe2, 0x3d, 0x29, 0xfb};//          	vpcmpeqq %ymm3,%ymm8,%ymm7
                add(INSTR9, size(INSTR9));
            } else {
                int INSTR6[] = {0xc4, 0x62, 0x3d, 0x29, 0xc8};//       	vpcmpeqq %ymm0,%ymm8,%ymm9
                add(INSTR6, size(INSTR6));
                int INSTR7[] = {0xc4, 0x62, 0x3d, 0x29, 0xd1};//       	vpcmpeqq %ymm1,%ymm8,%ymm10
                add(INSTR7, size(INSTR7));
                int INSTR8[] = {0xc4, 0x62, 0x3d, 0x29, 0xda};//       	vpcmpeqq %ymm2,%ymm8,%ymm11
                add(INSTR8, size(INSTR8));
                int INSTR9[] = {0xc4, 0x62, 0x3d, 0x29, 0xe3};//       	vpcmpeqq %ymm3,%ymm8,%ymm12
                add(INSTR9, size(INSTR9));
                if (i < inValues.size() - 1) {
                    int INSTR10[] = {0xc5, 0xb5, 0xeb, 0xe4};//          	vpor   %ymm4,%ymm9,%ymm4
                    add(INSTR10, size(INSTR10));
                    int INSTR11[] = {0xc5, 0xad, 0xeb, 0xed};//          	vpor   %ymm5,%ymm10,%ymm5
                    add(INSTR11, size(INSTR11));
                    int INSTR12[] = {0xc5, 0xa5, 0xeb, 0xf6};//          	vpor   %ymm6,%ymm11,%ymm6
                    add(INSTR12, size(INSTR12));
                    int INSTR13[] = {0xc5, 0x9d, 0xeb, 0xff};//          	vpor   %ymm7,%ymm12,%ymm7
                    add(INSTR13, size(INSTR13));
                } else {
                    int INSTR10[] = {0xc5, 0xb5, 0xeb, 0xc4};//          	vpor   %ymm4,%ymm9,%ymm0
                    add(INSTR10, size(INSTR10));
                    int INSTR11[] = {0xc5, 0xad, 0xeb, 0xcd};//         	vpor   %ymm5,%ymm10,%ymm1
                    add(INSTR11, size(INSTR11));
                    int INSTR12[] = {0xc5, 0xa5, 0xeb, 0xd6};//          	vpor   %ymm6,%ymm11,%ymm2
                    add(INSTR12, size(INSTR12));
                    int INSTR13[] = {0xc5, 0x9d, 0xeb, 0xdf};//          	vpor   %ymm7,%ymm12,%ymm3
                    add(INSTR13, size(INSTR13));
                }
            }
        }
    }
    merge_cmp64();
    merge_null();
}

bool Codegen::addConvertor(int alen, int targetLen, bool isf, bool targetIsFloat, bool isSigned, int bank) {
    bool needReload = false;
    if (isf) {
        if (alen==32 && targetLen == 64) {
            cvt_float_double(bank,false);
        }
    } else {
        if (targetIsFloat) {
            if (alen==32 && targetLen == 32) {
                cvt_signed_32_float(bank,false);
            }
            if (alen==32 && targetLen == 64) {
                cvt_signed_32_double(bank,false);
            }
            if (alen==64 && targetLen == 64) {
                cvt_signed_64_double(bank,false);
                needReload = true;
            }
            if (alen==128 && targetLen == 64) {
                cvt_signed_128_double(bank,false);
                needReload = true;
            }
        } else {
            if (isSigned) {
                if (alen==32 && targetLen >= 64) {
                    cvt_signed_32_64(bank,false);
                }
                if (alen==32 && targetLen == 128) {
                    cvt_signed_64_128(bank,false);
                }
                if (alen==64 && targetLen == 128) {
                    cvt_signed_64_128(bank,false);
                }
            } else {
                if (alen==32 && targetLen == 64) {
                    cvt_unsigned_32_64(bank,false);
                }
                if (alen==32 && targetLen == 128) {
                    cvt_unsigned_64_128(bank,false);
                }
                if (alen==64 && targetLen == 128) {
                    cvt_unsigned_64_128(bank,false);
                }
            }
        }
    }
    return needReload;
}

void Codegen::scalef(float factor) {
    auto v = *(uint32_t*)&factor;
    int INSTR0[] = {0x48, 0xb8};// 	            movabs    $0x12134,%rax
    add(INSTR0, size(INSTR0));
    addMemoryOffsetL(v);
    int INSTR1[] = {0xc4, 0x61, 0xf9, 0x6e, 0xc0};//    vmovq  %rax,%xmm8
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc4 ,0x42 ,0x7d ,0x58 ,0xc0};//   	vpbroadcastd %xmm8,%ymm8
    add(INSTR2, size(INSTR2));
    int INSTR3[] = {0xc4 ,0xc1 ,0x7c ,0x59 ,0xc0};//  	vmulps %ymm8,%ymm0,%ymm0
    add(INSTR3, size(INSTR3));
    int INSTR4[] = {0xc4 ,0xc1 ,0x74 ,0x59 ,0xc8};//   	vmulps %ymm8,%ymm1,%ymm1
    add(INSTR4, size(INSTR4));
}

void Codegen::scaled(double factor) {
    auto v = *(uint64_t*)&factor;
    int INSTR0[] = {0x48, 0xb8};// 	            movabs    $0x12134,%rax
    add(INSTR0, size(INSTR0));
    addMemoryOffsetL(v);
    int INSTR1[] = {0xc4, 0x61, 0xf9, 0x6e, 0xc0};//    vmovq  %rax,%xmm8
    add(INSTR1, size(INSTR1));
    int INSTR2[] = {0xc4 ,0x42 ,0x7d ,0x59 ,0xc0};//   	vpbroadcastq %xmm8,%ymm8
    add(INSTR2, size(INSTR2));
    int INSTR3[] = {0xc4 ,0xc1 ,0x7d ,0x59 ,0xc0};//       	vmulpd %ymm8,%ymm0,%ymm0
    add(INSTR3, size(INSTR3));
    int INSTR4[] = {0xc4 ,0xc1 ,0x75 ,0x59 ,0xc8};//       	vmulpd %ymm8,%ymm1,%ymm1
    add(INSTR4, size(INSTR4));
    int INSTR5[] = {0xc4 ,0xc1 ,0x6d ,0x59 ,0xd0};//       	vmulpd %ymm8,%ymm2,%ymm2
    add(INSTR5, size(INSTR5));
    int INSTR6[] = {0xc4 ,0xc1 ,0x65 ,0x59 ,0xd8};//       	vmulpd %ymm8,%ymm3,%ymm3
    add(INSTR6, size(INSTR6));
}

uint64_t  Codegen::getSize() const {
    return nbInstructions;
}

void Codegen::between(int len, bool isFloat, bool isSigned, int bank1, int bank2, int bank3) {
    if (isFloat) {
        switch (len) {
            case 16:
            case 32:
                betweenf_mm(bank1, bank2,bank3);
                break;
            case 64:
                betweend_mm(bank1, bank2,bank3);
                break;
            default:
                break;
        }
    } else {
        if (len <= 32) {
            between32b_mm(bank1, bank2,bank3);
        } else if (len <= 64) {
            between64b_mm(bank1, bank2,bank3);
        } else {
            between128b_mm(bank1, bank2,bank3);
        }
    }
}

void Codegen::betweenf_mm(int bank1, int bank2, int bank3) {
    load_src32(bank1);
    int INSTR0[] = {0xc5, 0x7c, 0xc2, 0x86};//          	vcmple_oqps x(%rsi),%ymm0,%ymm8
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank3, 0);
    mem[cursor++] = 0x12;
    int INSTR1[] = {0xc5, 0x74, 0xc2, 0x8e};//          	vcmple_oqps x(%rsi),%ymm1,%ymm8
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank3, 1);
    mem[cursor++] = 0x12;
    int INSTR2[] = {0xc5, 0xfc, 0xc2, 0x86};//          	vcmpgt_oqps x(%rsi),%ymm0,%ymm0
    add(INSTR2, size(INSTR2));
    addMemoryOffset(bank2, 0);
    mem[cursor++] = 0x1d;
    int INSTR3[] = {0xc5, 0xf4, 0xc2, 0x8e};//          	vcmpgt_oqps x(%rsi),%ymm0,%ymm0
    add(INSTR3, size(INSTR3));
    addMemoryOffset(bank2, 1);
    mem[cursor++] = 0x1d;
    int INSTR4[] = {0xc4 ,0xc1 ,0x7c ,0x54 ,0xc0};//       	vandps %ymm8,%ymm0,%ymm0
    add(INSTR4, size(INSTR4));
    int INSTR5[] = {0xc4 ,0xc1 ,0x74 ,0x54 ,0xc9};//       	vandps %ymm9,%ymm1,%ymm1
    add(INSTR5, size(INSTR5));
    merge_cmp32();
}

void Codegen::betweend_mm(int bank1, int bank2, int bank3) {
    load_src64(bank1);
    int INSTR0[] = {0xc5 ,0x7d ,0xc2 ,0x86};// 	vcmple_oqpd 0x12345(%rsi),%ymm0,%ymm8
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank3, 0);
    mem[cursor++] = 0x12;
    int INSTR1[] = {0xc5 ,0x75 ,0xc2 ,0x8e};// 	vcmple_oqpd 0x12345(%rsi),%ymm1,%ymm9
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank3, 1);
    mem[cursor++] = 0x12;
    int INSTR2[] = {0xc5 ,0x6d ,0xc2 ,0x96};// 	vcmple_oqpd 0x12345(%rsi),%ymm2,%ymm10
    add(INSTR2, size(INSTR2));
    addMemoryOffset(bank3, 2);
    mem[cursor++] = 0x12;
    int INSTR3[] = {0xc5 ,0x65 ,0xc2 ,0x9e};// 	vcmple_oqpd 0x12345(%rsi),%ymm3,%ymm11
    add(INSTR3, size(INSTR3));
    addMemoryOffset(bank3, 3);
    mem[cursor++] = 0x12;
    int INSTR4[] = {0xc5 ,0xfd ,0xc2 ,0x86};// 	vcmpge_oqpd 0x12345(%rsi),%ymm0,%ymm0
    add(INSTR4, size(INSTR4));
    addMemoryOffset(bank2, 0);
    mem[cursor++] = 0x1d;
    int INSTR5[] = {0xc5 ,0xf5 ,0xc2 ,0x8e};// 	vcmpge_oqpd 0x12345(%rsi),%ymm1,%ymm1
    add(INSTR5, size(INSTR5));
    addMemoryOffset(bank2, 1);
    mem[cursor++] = 0x1d;
    int INSTR6[] = {0xc5 ,0xed ,0xc2 ,0x96};//	vcmpge_oqpd 0x12345(%rsi),%ymm2,%ymm2
    add(INSTR6, size(INSTR6));
    addMemoryOffset(bank2, 2);
    mem[cursor++] = 0x1d;
    int INSTR7[] = {0xc5 ,0xe5 ,0xc2 ,0x9e};// 	vcmpge_oqpd 0x12345(%rsi),%ymm3,%ymm3
    add(INSTR7, size(INSTR4));
    addMemoryOffset(bank2, 3);
    mem[cursor++] = 0x1d;
    int INSTR8[] = {0xc4 ,0xc1 ,0x7c ,0x54 ,0xc0};//  vandps %ymm8,%ymm0,%ymm0
    add(INSTR8, size(INSTR8));
    int INSTR9[] = {0xc4 ,0xc1 ,0x74 ,0x54 ,0xc9};//  vandps %ymm9,%ymm1,%ymm1
    add(INSTR9, size(INSTR9));
    int INSTR10[] = {0xc4 ,0xc1 ,0x6c ,0x54 ,0xd2};// vandps %ymm10,%ymm2,%ymm2
    add(INSTR10, size(INSTR10));
    int INSTR11[] = {0xc4 ,0xc1 ,0x64 ,0x54 ,0xdb};// vandps %ymm11,%ymm3,%ymm3
    add(INSTR11, size(INSTR11));
    merge_cmp64();
}

void Codegen::between32b_mm(int bank1, int bank2,int bank3) {
    load_src32(bank1);
    int INSTR0[] = {0xc5 ,0x7d ,0x66 ,0x86};// 	vpcmpgtd 0x12345(%rsi),%ymm0,%ymm8
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank2, 0);
    int INSTR1[] = {0xc5 ,0x75 ,0x66 ,0x8e};// 	vpcmpgtd 0x12345(%rsi),%ymm1,%ymm9
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank2, 1);
    int INSTR2[] = {0xc5 ,0x7d ,0x76 ,0xa6};//	vpcmpeqd 0x12345(%rsi),%ymm0,%ymm12
    add(INSTR2, size(INSTR2));
    addMemoryOffset(bank2, 0);
    int INSTR3[] = {0xc5 ,0x75 ,0x76 ,0xae};//	vpcmpeqd 0x12345(%rsi),%ymm1,%ymm13
    add(INSTR3, size(INSTR3));
    addMemoryOffset(bank2, 1);
    int INSTR4[] = {0xc4 ,0x41 ,0x1d ,0xeb ,0xc0};//vpor   %ymm8,%ymm12,%ymm8
    add(INSTR4, size(INSTR4));
    int INSTR5[] = {0xc4 ,0x41 ,0x15 ,0xeb ,0xc9};//vpor   %ymm9,%ymm13,%ymm9
    add(INSTR5, size(INSTR5));
    int INSTR6[] = {0xc5 ,0xfd ,0x66 ,0x86};// 	vpcmpgtd 0x12345(%rsi),%ymm0,%ymm0
    add(INSTR6, size(INSTR6));
    addMemoryOffset(bank3, 0);
    int INSTR7[] = {0xc5 ,0xf5 ,0x66 ,0x8e};// 	vpcmpgtd 0x12345(%rsi),%ymm1,%ymm1
    add(INSTR7, size(INSTR7));
    addMemoryOffset(bank3, 1);
    int INSTR8[] = {0xc4 ,0xc1 ,0x7d ,0xdf ,0xc0};//       	vpandn %ymm8,%ymm0,%ymm0
    add(INSTR8, size(INSTR8));
    int INSTR9[] = {0xc4 ,0xc1 ,0x75 ,0xdf ,0xc9};//       	vpandn %ymm9,%ymm1,%ymm1
    add(INSTR9, size(INSTR9));
    merge_cmp32();
}

void Codegen::between64b_mm(int bank1, int bank2,int bank3) {
    load_src64(bank1);
    int INSTR0[] = {0xc4 ,0x62 ,0x7d ,0x37 ,0x86};// 	vpcmpgtq 0x12345(%rsi),%ymm0,%ymm8
    add(INSTR0, size(INSTR0));
    addMemoryOffset(bank2, 0);
    int INSTR1[] = {0xc4 ,0x62 ,0x75 ,0x37 ,0x8e};// 	vpcmpgtq 0x12345(%rsi),%ymm1,%ymm9
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank2, 1);
    int INSTR2[] = {0xc4 ,0x62 ,0x6d ,0x37 ,0x96};//	vpcmpgtq 0x12345(%rsi),%ymm2,%ymm10
    add(INSTR2, size(INSTR2));
    addMemoryOffset(bank2, 2);
    int INSTR3[] = {0xc4 ,0x62 ,0x65 ,0x37 ,0x9e};// 	vpcmpgtq 0x12345(%rsi),%ymm3,%ymm11
    add(INSTR3, size(INSTR3));
    addMemoryOffset(bank2, 3);
    int INSTR4[] = {0xc4 ,0x62 ,0x7d ,0x29 ,0xa6};// 	vpcmpeqq 0x12345(%rsi),%ymm0,%ymm12
    add(INSTR4, size(INSTR4));
    addMemoryOffset(bank2, 0);
    int INSTR5[] = {0xc4 ,0x62 ,0x75 ,0x29 ,0xae};// 	vpcmpeqq 0x12345(%rsi),%ymm1,%ymm13
    add(INSTR5, size(INSTR5));
    addMemoryOffset(bank2, 1);
    int INSTR6[] = {0xc4 ,0x62 ,0x6d ,0x29 ,0xb6};// 	vpcmpeqq 0x12345(%rsi),%ymm2,%ymm14
    add(INSTR6, size(INSTR6));
    addMemoryOffset(bank2, 2);
    int INSTR7[] = {0xc4 ,0x62 ,0x65 ,0x29 ,0xbe};// 	vpcmpeqq 0x12345(%rsi),%ymm3,%ymm15
    add(INSTR7, size(INSTR7));
    addMemoryOffset(bank2, 3);
    int INSTR8[] = {0xc4 ,0x41 ,0x1d ,0xeb ,0xc0};//   	vpor   %ymm8,%ymm12,%ymm8
    add(INSTR8, size(INSTR8));
    int INSTR9[] = {0xc4 ,0x41 ,0x15 ,0xeb ,0xc9};//   	vpor   %ymm9,%ymm13,%ymm9
    add(INSTR9, size(INSTR9));
    int INSTR10[] = {0xc4 ,0x41 ,0x0d ,0xeb ,0xd2};//  	vpor   %ymm10,%ymm14,%ymm10
    add(INSTR10, size(INSTR10));
    int INSTR11[] = {0xc4 ,0x41 ,0x05 ,0xeb ,0xdb};// 	vpor   %ymm11,%ymm15,%ymm11
    add(INSTR11, size(INSTR11));
    int INSTR12[] = {0xc4 ,0xe2 ,0x7d ,0x37 ,0x86};// 	vpcmpgtq 0x12345(%rsi),%ymm0,%ymm0
    add(INSTR12, size(INSTR12));
    addMemoryOffset(bank3, 0);
    int INSTR13[] = {0xc4 ,0xe2 ,0x75 ,0x37 ,0x8e};// 	vpcmpgtq 0x12345(%rsi),%ymm1,%ymm1
    add(INSTR13, size(INSTR13));
    addMemoryOffset(bank3, 1);
    int INSTR14[] = {0xc4 ,0xe2 ,0x6d ,0x37 ,0x96};// 	vpcmpgtq 0x12345(%rsi),%ymm2,%ymm2
    add(INSTR14, size(INSTR14));
    addMemoryOffset(bank3, 2);
    int INSTR15[] = {0xc4 ,0xe2 ,0x65 ,0x37 ,0x9e};// 	vpcmpgtq 0x12345(%rsi),%ymm3,%ymm3
    add(INSTR15, size(INSTR15));
    addMemoryOffset(bank3, 3);
    int INSTR16[] = {0xc4 ,0xc1 ,0x7d ,0xdf ,0xc0};//  	vpandn %ymm8,%ymm0,%ymm0
    add(INSTR16, size(INSTR16));
    int INSTR17[] = {0xc4 ,0xc1 ,0x75 ,0xdf ,0xc9};//  	vpandn %ymm9,%ymm1,%ymm1
    add(INSTR17, size(INSTR17));
    int INSTR18[] = {0xc4 ,0xc1 ,0x6d ,0xdf ,0xd2};//  	vpandn %ymm10,%ymm2,%ymm2
    add(INSTR18, size(INSTR18));
    int INSTR19[] = {0xc4 ,0xc1 ,0x65 ,0xdf ,0xdb};//  	vpandn %ymm11,%ymm3,%ymm3
    add(INSTR19, size(INSTR19));
    merge_cmp64();
}

static uint32_t between128b_C(const __int128_t *a,const __int128_t *b,const __int128_t *c) {
    uint32_t ret = 0;
    for (int i=0; i< 16; i++) {
        bool cond = a[i] >= b[i] && a[i] <= c[i];
        ret |= (cond ? 3 : 0) << (2*i);
    }
    return ret;
}

void Codegen::between128b_mm(int bank1, int bank2,int bank3) {
    auto p = between128b_C;
    int INSTR0[] = { 0x56, 0x57, 0x41, 0x54, 0x41, 0x55}; // push   %rsi; push   %rdi; push   %r12; push %r13
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0x48 ,0x8d ,0x96};// 	lea    0x12345(%rsi),%rdx
    add(INSTR1, size(INSTR1));
    addMemoryOffset(bank3, 0);
    int INSTR2[] = {0x48 ,0x8d ,0xbe};// 	lea    0x12345(%rsi),%rdi
    add(INSTR2, size(INSTR2));
    addMemoryOffset(bank1, 0);
    int INSTR3[] = {0x48 ,0x8d ,0xb6};// 	lea    0x12345(%rsi),%rsi
    add(INSTR3, size(INSTR3));
    addMemoryOffset(bank2, 0);
    int INSTR4[] = {0x48, 0xb8};// 	         movabs    $0x12134,%rax
    add(INSTR4, size(INSTR4));
    addMemoryOffsetL((uint64_t)p);
    int INSTR5[] = { 0xff ,0xd0};//         call %rax
    add(INSTR5, size(INSTR5));
    int INSTR6[] = { 0x41, 0x5d, 0x41, 0x5c, 0x5f, 0x5e};   // pops
    add(INSTR6, size(INSTR6));
}

void Codegen::isNull() {
    nbInstructions = 0;
    cursor = 0;
    constexpr int INSTR_ENDBR64[] = {0xf3, 0x0f, 0x1e, 0xfa};
    add(INSTR_ENDBR64, size(INSTR_ENDBR64));
    int INSTR0[] = {0x48 ,0x8b ,0x0f};//             	mov    (%rdi),%rcx
    add(INSTR0, size(INSTR0));
    int INSTR1[] = {0x48 ,0x0f ,0xb7 ,0x01};//       	movzwq (%rcx),%rax
    add(INSTR1, size(INSTR1));
    //int INSTR2[] = {0x48 ,0xf7 ,0xd0};//             	not    %rax
    //add(INSTR2, size(INSTR2));
    //illegalInstruction();
}

}
