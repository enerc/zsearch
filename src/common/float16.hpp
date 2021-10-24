#pragma once

#include <cstdint>
#include <iostream>
#include <cmath>
#include "avx_conf.h"


#define GEOMETRY_SCALING_LAT 16777216
#define GEOMETRY_SCALING_LON 8388608

inline uint16_t floatToHalfFloat(float floatValue ) {
	__m128 a = { floatValue, 0, 0, 0,};
	return _mm_extract_epi16(_mm_cvtps_ph(a,_MM_FROUND_NO_EXC),0); // Round to nearest even
}

inline float halfFloatToFloat( uint16_t halfFloat ) {
	float ret;
	__m128i a= _mm_set_epi16( 0, 0, 0, 0, 0, 0, 0,halfFloat);
	__m128 b = _mm_cvtph_ps(a);
	_mm_store_ss(&ret,b);
	return ret;
}

inline uint32_t floatToUint32(float floatValue) {
	return *(uint32_t*)&floatValue;
}

inline float uint32ToFloat(uint32_t v) {
	return *(float*)&v;
}

inline uint64_t doubleToUint64(double doubleValue) {
	return *(uint64_t*)&doubleValue;
}

inline double uint64ToDouble(uint64_t v) {
	return *(double*)&v;
}

inline uint64_t locationToUint64(float x, float y) {
	int32_t a[2] = { (int32_t)std::round(x*GEOMETRY_SCALING_LON),(int32_t)std::round(y*GEOMETRY_SCALING_LAT) };
	return *(uint64_t*)&a;
}

inline void Uint64ToLocation(int64_t v, float &x, float &y) {
    x = ((float)((int32_t)v))/GEOMETRY_SCALING_LON;
    y = ((float)(v>>32))/GEOMETRY_SCALING_LAT;
}