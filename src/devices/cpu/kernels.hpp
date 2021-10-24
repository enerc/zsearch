#pragma once
#include "../../common/float16.hpp"

typedef bool (*TestOper)(const __uint128_t&,const __uint128_t&);
inline bool OperEqual(const __uint128_t&a,const __uint128_t&b) { return a == b;}
inline bool OperEqualF16(const __uint128_t&a,const __uint128_t&b) { return (halfFloatToFloat(*(uint16_t*)&a) == halfFloatToFloat(*(uint16_t*)&b)) ; }
inline bool OperEqualF32(const __uint128_t&a,const __uint128_t&b) {return *(float*)&a == *(float*)&b; }
inline bool OperEqualF64(const __uint128_t&a,const __uint128_t&b) {return *(double*)&a == *(double*)&b;}
inline bool OperNotEqual(const __uint128_t&a,const __uint128_t&b) { return a != b;}
inline bool OperNotEqualF16(const __uint128_t&a,const __uint128_t&b) {return (halfFloatToFloat(*(uint16_t*)&a) != halfFloatToFloat(*(uint16_t*)&b)); }
inline bool OperNotEqualF32(const __uint128_t&a,const __uint128_t&b) {return *(float*)&a != *(float*)&b; }
inline bool OperNotEqualF64(const __uint128_t&a,const __uint128_t&b) {return *(double*)&a != *(double*)&b;}
inline bool OperLTS(const __uint128_t&a,const __uint128_t&b) {return (__int128_t)a < (__int128_t)b; }
inline bool OperLTU(const __uint128_t&a,const __uint128_t&b) {return a < b; }
inline bool OperGTS(const __uint128_t&a,const __uint128_t&b) {return (__int128_t)a > (__int128_t)b; }
inline bool OperGTU(const __uint128_t&a,const __uint128_t&b) {return a > b; }
inline bool OperLES(const __uint128_t&a,const __uint128_t&b) {return (__int128_t)a <= (__int128_t)b; }
inline bool OperLEU(const __uint128_t&a,const __uint128_t&b) {return a <= b; }
inline bool OperGES(const __uint128_t&a,const __uint128_t&b) {return (__int128_t)a >= (__int128_t)b; }
inline bool OperGEU(const __uint128_t&a,const __uint128_t&b) {return a >= b; }
inline bool OperLTF16(const __uint128_t&a,const __uint128_t&b) {return (halfFloatToFloat(*(uint16_t*)&a) < halfFloatToFloat(*(uint16_t*)&b)) ; }
inline bool OperLTF32(const __uint128_t&a,const __uint128_t&b) {return *(float*)&a < *(float*)&b; }
inline bool OperLTF64(const __uint128_t&a,const __uint128_t&b) {return *(double*)&a < *(double*)&b; }
inline bool OperGTF16(const __uint128_t&a,const __uint128_t&b) {return (halfFloatToFloat(*(uint16_t*)&a) > halfFloatToFloat(*(uint16_t*)&b)) ; }
inline bool OperGTF32(const __uint128_t&a,const __uint128_t&b) {return *(float*)&a > *(float*)&b; }
inline bool OperGTF64(const __uint128_t&a,const __uint128_t&b) {return *(double*)&a > *(double*)&b; }
inline bool OperLEF16(const __uint128_t&a,const __uint128_t&b) {return (halfFloatToFloat(*(uint16_t*)&a) <= halfFloatToFloat(*(uint16_t*)&b)) ; }
inline bool OperLEF32(const __uint128_t&a,const __uint128_t&b) {return *(float*)&a <= *(float*)&b; }
inline bool OperLEF64(const __uint128_t&a,const __uint128_t&b) {return *(double*)&a <= *(double*)&b; }
inline bool OperGEF16(const __uint128_t&a,const __uint128_t&b) {return (halfFloatToFloat(*(uint16_t*)&a) >= halfFloatToFloat(*(uint16_t*)&b)) ; }
inline bool OperGEF32(const __uint128_t&a,const __uint128_t&b) {return *(float*)&a >= *(float*)&b; }
inline bool OperGEF64(const __uint128_t&a,const __uint128_t&b) {return *(double*)&a >= *(double*)&b; }

enum OperatorType {
    OperEqualType,
    OperNotEqualType,
    OperLTType,
    OperGTType,
    OperLEType,
    OperGEType,
} ;

typedef void (*MergeOper)();
inline void MergeNone() {;}
inline void MergeOr() {;}
inline void MergeAnd() {;}


extern "C" {
float distance_to_center(int64_t v, int64_t center);
void unpackDebug();
}
template <OperatorType>uint16_t st_distance_sphere(__m256i x0,__m256i x1,__m256d positionSphereCenter256,__m256 cosPositionSphereOrigin256,__m256 distanceSphere256);
__m256 cal_distance_sphere(__m256i x0,__m256i x1, __m256d positionSphereCenter256, __m256 cosPositionSphereOrigin256);


