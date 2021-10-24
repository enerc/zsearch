#pragma once

#include <cstdint>
#include <vector>
#include "../../common/avx_conf.h"
#include "kernels.hpp"

namespace devices {

constexpr uint64_t MAX_CODE_SIZE = 256*4096;
constexpr uint64_t MAX_DATA_SIZE = 4096*32;
constexpr uint64_t MAX_BANKS = 1024;

class Codegen {
public:
    Codegen();
    ~Codegen();
    void flush();
    void illegalInstruction();
    uint16_t exec(void *data, void *banks) const;
    void fetchVariable(int len,bool isFloat, bool isSigned, int targetLen, bool targetisFloat, int srcIndex, int bank,double scale);
    void loadConstant(__uint128_t &cst,int len, bool isFloat,int bank, bool isInClause);
    void cmpeq(int len, bool isFloat, bool isSigned, int bank1, int bank2);
    void cmpne(int len, bool isFloat, bool isSigned, int bank1, int bank2);
    void cmplt(int len, bool isFloat, bool isSigned, int bank1, int bank2);
    void cmple(int len, bool isFloat, bool isSigned, int bank1, int bank2);
    void cmpgt(int len, bool isFloat, bool isSigned, int bank1, int bank2);
    void cmpge(int len, bool isFloat, bool isSigned, int bank1, int bank2);
    void between(int len, bool isFloat, bool isSigned, int bank1, int bank2, int bank3);
    void cmp(OperatorType T, int len, bool isFloat, bool isSigned, int bank1, int bank2);
    void add(int len, bool isFloat, int bank1, int bank2);
    void sub(int len, bool isFloat, int bank1, int bank2);
    void mul(int len, bool isFloat, int bank1, int bank2);
    void div(int len, bool isFloat, int bank1, int bank2);
    void xor_mm(int len, bool isFloat,int bank1, int bank2);
    void min(int len, bool isFloat,int bank1, int bank2);
    void max(int len, bool isFloat,int bank1, int bank2);
    void distanceSphere(int bank1, int targetLen);
    void isNull();
    static void dumpFloat(int bank, void *banks);
    [[nodiscard]] uint64_t  getSize() const;

    void in32(int bank);
    void in64(int bank);
    void notin32(int bank);

private:
    void add(const int *,int size);
    void loadPointer();
    void addConstant(const __m256i &,bool doCopy);
    void addSignedShift32(int s);
    void addSignedShift64(int s);
    void unpackGen5(bool isSigned,int p, const __m256i &mask);
    void unpackGen9(bool isSigned,int p);
    void unpackGen13(bool isSigned,int p);
    void unpackGen17(bool isSigned,int p);
    void unpackGen25(bool isSigned,int p);
    void unpackGen33(bool isSigned,int p);
    void unpackGen41(bool isSigned,int p);
    void unpackGen49(bool isSigned,int p);
    void unpackGen57(bool isSigned,int p);
    void unpackGen65(bool isSigned,int p);
    void unpackGen73(bool isSigned,int p);
    void unpackGen81(bool isSigned,int p);
    void unpackGen89(bool isSigned,int p);
    void unpackGen97(bool isSigned,int p);
    void unpackGen105(bool isSigned,int p);
    void unpackGen113(bool isSigned,int p);
    void unpackGen121(bool isSigned,int p);

    void unpack1(bool isSigned);
    void unpack2(bool isSigned);
    void unpack3(bool isSigned);
    void unpack4(bool isSigned);
    void unpack5(bool isSigned);
    void unpack6(bool isSigned);
    void unpack7(bool isSigned);
    void unpack8(bool isSigned);
    void unpack9(bool isSigned);
    void unpack10(bool isSigned);
    void unpack11(bool isSigned);
    void unpack12(bool isSigned);
    void unpack13(bool isSigned);
    void unpack14(bool isSigned);
    void unpack15(bool isSigned);
    void unpack16(bool isSigned);
    void unpack17(bool isSigned);
    void unpack18(bool isSigned);
    void unpack19(bool isSigned);
    void unpack20(bool isSigned);
    void unpack21(bool isSigned);
    void unpack22(bool isSigned);
    void unpack23(bool isSigned);
    void unpack24(bool isSigned);
    void unpack25(bool isSigned);
    void unpack26(bool isSigned);
    void unpack27(bool isSigned);
    void unpack28(bool isSigned);
    void unpack29(bool isSigned);
    void unpack30(bool isSigned);
    void unpack31(bool isSigned);
    void unpack32(bool isSigned);
    void unpack33(bool isSigned);
    void unpack34(bool isSigned);
    void unpack35(bool isSigned);
    void unpack36(bool isSigned);
    void unpack37(bool isSigned);
    void unpack38(bool isSigned);
    void unpack39(bool isSigned);
    void unpack40(bool isSigned);
    void unpack41(bool isSigned);
    void unpack42(bool isSigned);
    void unpack43(bool isSigned);
    void unpack44(bool isSigned);
    void unpack45(bool isSigned);
    void unpack46(bool isSigned);
    void unpack47(bool isSigned);
    void unpack48(bool isSigned);
    void unpack49(bool isSigned);
    void unpack50(bool isSigned);
    void unpack51(bool isSigned);
    void unpack52(bool isSigned);
    void unpack53(bool isSigned);
    void unpack54(bool isSigned);
    void unpack55(bool isSigned);
    void unpack56(bool isSigned);
    void unpack57(bool isSigned);
    void unpack58(bool isSigned);
    void unpack59(bool isSigned);
    void unpack60(bool isSigned);
    void unpack61(bool isSigned);
    void unpack62(bool isSigned);
    void unpack63(bool isSigned);
    void unpack64(bool isSigned);
    void unpack65(bool isSigned);
    void unpack66(bool isSigned);
    void unpack67(bool isSigned);
    void unpack68(bool isSigned);
    void unpack69(bool isSigned);
    void unpack70(bool isSigned);
    void unpack71(bool isSigned);
    void unpack72(bool isSigned);
    void unpack73(bool isSigned);
    void unpack74(bool isSigned);
    void unpack75(bool isSigned);
    void unpack76(bool isSigned);
    void unpack77(bool isSigned);
    void unpack78(bool isSigned);
    void unpack79(bool isSigned);
    void unpack80(bool isSigned);
    void unpack81(bool isSigned);
    void unpack82(bool isSigned);
    void unpack83(bool isSigned);
    void unpack84(bool isSigned);
    void unpack85(bool isSigned);
    void unpack86(bool isSigned);
    void unpack87(bool isSigned);
    void unpack88(bool isSigned);
    void unpack89(bool isSigned);
    void unpack90(bool isSigned);
    void unpack91(bool isSigned);
    void unpack92(bool isSigned);
    void unpack93(bool isSigned);
    void unpack94(bool isSigned);
    void unpack95(bool isSigned);
    void unpack96(bool isSigned);
    void unpack97(bool isSigned);
    void unpack98(bool isSigned);
    void unpack99(bool isSigned);
    void unpack100(bool isSigned);
    void unpack101(bool isSigned);
    void unpack102(bool isSigned);
    void unpack103(bool isSigned);
    void unpack104(bool isSigned);
    void unpack105(bool isSigned);
    void unpack106(bool isSigned);
    void unpack107(bool isSigned);
    void unpack108(bool isSigned);
    void unpack109(bool isSigned);
    void unpack110(bool isSigned);
    void unpack111(bool isSigned);
    void unpack112(bool isSigned);
    void unpack113(bool isSigned);
    void unpack114(bool isSigned);
    void unpack115(bool isSigned);
    void unpack116(bool isSigned);
    void unpack117(bool isSigned);
    void unpack118(bool isSigned);
    void unpack119(bool isSigned);
    void unpack120(bool isSigned);
    void unpack121(bool isSigned);
    void unpack122(bool isSigned);
    void unpack123(bool isSigned);
    void unpack124(bool isSigned);
    void unpack125(bool isSigned);
    void unpack126(bool isSigned);
    void unpack127(bool isSigned);
    void unpack128(bool isSigned);

    void unpack(int len, bool isSigned);
    void save(int len,int bank);

    void loadConstant32(const __uint128_t &cst,int bank);
    void loadConstant64(const __uint128_t &cst,int bank);
    void loadConstant128(const __uint128_t &cst,int bank);
    void loadConstantF16(const __uint128_t &cst,int bank);
    void addMemoryOffset(uint64_t offset);
    void addMemoryOffsetL(uint64_t offset);
    void addMemoryOffset(int bank,int regIndex);
    void addMemoryOffset32(int bank,int regIndex);
    void addMemoryOffset64(int bank,int regIndex);
    void addMemoryOffset128(int bank,int regIndex);

    void mulf_mm(int bank1, int bank2);
    void muld_mm(int bank1, int bank2);
    void mul32b_mm(int bank1, int bank2);
    void mul64b_mm(int bank1, int bank2);
    void mul128b_mm(int bank1, int bank2);

    void divf_mm(int bank1, int bank2);
    void divd_mm(int bank1, int bank2);
    void div32b_mm(int bank1, int bank2);
    void div64b_mm(int bank1, int bank2);
    void div128b_mm(int bank1, int bank2);

    void addf_mm(int bank1, int bank2);
    void addd_mm(int bank1, int bank2);
    void add32b_mm(int bank1, int bank2);
    void add64b_mm(int bank1, int bank2);
    void add128b_mm(int bank1, int bank2);

    void subf_mm(int bank1, int bank2);
    void subd_mm(int bank1, int bank2);
    void sub32b_mm(int bank1, int bank2);
    void sub64b_mm(int bank1, int bank2);
    void sub128b_mm(int bank1, int bank2);

    void xor32b_mm(int bank1, int bank2);
    void xor64b_mm(int bank1, int bank2);
    void xor128b_mm(int bank1, int bank2);

    void min32b_mm(int bank1, int bank2);
    void min64b_mm(int bank1, int bank2);
    void min128b_mm(int bank1, int bank2);
    void minf_mm(int bank1, int bank2);
    void mind_mm(int bank1, int bank2);

    void max32b_mm(int bank1, int bank2);
    void max64b_mm(int bank1, int bank2);
    void max128b_mm(int bank1, int bank2);
    void maxf_mm(int bank1, int bank2);
    void maxd_mm(int bank1, int bank2);


    void cmpeqf_mm(int bank1, int bank2);
    void cmpeqd_mm(int bank1, int bank2);
    void cmpeq32b_mm(int bank1, int bank2);
    void cmpeq64b_mm(int bank1, int bank2);
    void cmpeq128b_mm(int bank1, int bank2);

    void cmpnef_mm(int bank1, int bank2);
    void cmpned_mm(int bank1, int bank2);
    void cmpne32b_mm(int bank1, int bank2);
    void cmpne64b_mm(int bank1, int bank2);
    void cmpne128b_mm(int bank1, int bank2);

    void cmplef_mm(int bank1, int bank2);
    void cmpled_mm(int bank1, int bank2);
    void cmple32b_mm(int bank1, int bank2,int len,bool isSigned);
    void cmple64b_mm(int bank1, int bank2,int len,bool isSigned);
    void cmple128b_mm(int bank1, int bank2);

    void cmpltf_mm(int bank1, int bank2);
    void cmpltd_mm(int bank1, int bank2);
    void cmplt32b_mm(int bank1, int bank2,int len, bool isSigned);
    void cmplt64b_mm(int bank1, int bank2,int len, bool isSigned);
    void cmplt128b_mm(int bank1, int bank2);

    void cmpgtf_mm(int bank1, int bank2);
    void cmpgtd_mm(int bank1, int bank2);
    void cmpgt32b_mm(int bank1, int bank2,int len,bool isSigned);
    void cmpgt64b_mm(int bank1, int bank2,int len,bool isSigned);
    void cmpgt128b_mm(int bank1, int bank2);

    void cmpgef_mm(int bank1, int bank2);
    void cmpged_mm(int bank1, int bank2);
    void cmpge32b_mm(int bank1, int bank2,int len,bool isSigned);
    void cmpge64b_mm(int bank1, int bank2,int len,bool isSigned);
    void cmpge128b_mm(int bank1, int bank2);

    void betweenf_mm(int bank1, int bank2, int bank3);
    void betweend_mm(int bank1, int bank2, int bank3);
    void between32b_mm(int bank1, int bank2,int bank3);
    void between64b_mm(int bank1, int bank2,int bank3);
    void between128b_mm(int bank1, int bank2,int bank3);

    void cvt_unsigned_64_128(int bank, bool reload);
    void cvt_signed_64_128(int bank, bool reload);
    void cvt_signed_32_float(int bank, bool reload);
    void cvt_signed_32_double(int bank, bool reload);
    void cvt_signed_64_double(int bank, bool reload);
    void cvt_signed_128_double(int bank, bool reload);
    void cvt_float_double(int bank, bool reload);
    void cvt_unsigned_32_64(int bank, bool reload);
    void cvt_signed_32_64(int bank, bool reload);

    void merge_null();
    void merge_cmp32();
    void merge_cmp64();
    void load_src32(int bank1);
    void load_src64(int bank1);
    void load_src128(int bank1);
    void f16Tof32(int srcIndex, int bank);
    void addConvertor(int len, int targetLen, bool isaFloat, bool targetIsFloat,bool isSigned, int bank);
    void scalef(float factor);
    void scaled(double factor);
    void loadCenter(uint64_t);
    void ret();


    uint8_t *mem;
    uint64_t cursor;
    uint64_t nbInstructions{};
    int      sourceIndex;
    int      constantIndex;
    std::vector<__uint128_t> inValues;

};

}