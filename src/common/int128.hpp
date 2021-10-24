#pragma once

#include <cstdint>
#include <string_view>
#include <cstring>
#include <chrono>
#include <ctime>
#include <cmath>
#include "../common/float16.hpp"
#include "../mapping/mapping_types.hpp"
#include "../mapping/indexDefinition.hpp"
#include "date.hpp"
#include "tools.hpp"
#include "../floaxie/ftoa.hpp"

template<class T> bool parse_digit(const uint8_t c, T &i) {
	const auto digit = static_cast<uint8_t>(c - '0');
	if (digit > 9) {
		return false;
	}
	i = 10 * i + digit;
	return true;
}

inline int hex2int(char ch) {
	if (ch >= '0' && ch <= '9')
		return ch - '0';
	if (ch >= 'A' && ch <= 'F')
		return ch - 'A' + 10;
	if (ch >= 'a' && ch <= 'f')
		return ch - 'a' + 10;
	Log::error("invalid character in hex number "+std::to_string(ch));
	return -1;
}

template<class T> bool parse_hexdigit(const uint8_t c, T &i) {
	const int digit = hex2int((char)c);
	if (digit == -1) {
		return false;
	}
	i = 16 * i + digit;
	return true;
}

inline __uint128_t parseUnsigned128(const std::string_view &src, uint32_t length) {
	if (src.size() > 1 && src.at(0) == '0' && (src.at(1) == 'x' || src.at(1) == 'X')) {
		int p = 2;
		__uint128_t ret = 0;
		while (p < src.size() && parse_hexdigit<__uint128_t>(src.at(p), ret)) { p++; }
		return ret;
	} else {
		bool negative = (src.at(0) == '-');	// should not be there, but in case....
		int p = negative ? 1 : 0;

		__uint128_t ret = 0;
		while (p < src.size() && parse_digit<__uint128_t>(src.at(p), ret)) { p++; }

        const __uint128_t h = (((__uint128_t)1) << length)-1;
        return length < 128 ? std::min(ret, h) : ret;
	}
}

inline __int128_t parseSigned128(const std::string_view &src, uint32_t length) {
	if (src.size() > 1 && src.at(0) == '0' && (src.at(1) == 'x' || src.at(1) == 'X')) {
		int p = 2;
		__int128_t ret = 0;
		while (p < src.size() && parse_hexdigit<__int128_t>(src.at(p), ret)) { p++; }
		return ret;
	} else {
		bool negative = (src.at(0) == '-');
		int p = negative ? 1 : 0;

		__int128_t ret = 0;
		while (p < src.size() && parse_digit<__int128_t>(src.at(p), ret)) { p++; }

		const __uint128_t h = length == 128 ? -1 : (((__uint128_t)1) << length)-1UL;
        const auto o = (__int128_t)((((__uint128_t)1) << (length-1))-1L);
        return negative ? (__int128_t)((-std::min(ret, o))&h) : std::min(ret, o);
	}
}

inline __int128_t convertTo128iSigned(int64_t v) {
	return (__int128_t)v;
}

inline __uint128_t convertTo128iUnsigned(uint64_t v ) {
	return (__uint128_t)v;
}

[[maybe_unused]] inline __uint128_t convertTo128f(float v ) {
	__uint128_t ret;
	memset(&ret,0,sizeof(ret));
	auto *a = (float*)&ret;
	a[0] = v;
	return ret;
}

inline __uint128_t convertTo128d(double v ) {
	__uint128_t ret;
	memset(&ret,0,sizeof(ret));
	auto *a = (double*)&ret;
	a[0] = v;
	return ret;
}

inline float from128f(const __uint128_t &v) {
    return *(float*)&v;
}

inline double from128d(const __uint128_t &v) {
    return *(double*)&v;
}

inline void initConvertedResult(char *buffer, int scale) {
    if (scale==0) {
        return;
    }
    while (scale > 0) {
        *buffer = '0';
        buffer --;
        scale --;
    }
    *buffer = '.';
    buffer--;
    *buffer = '0';
}

inline int printInt128(__int128_t x, int scale,char *buffer) {
    bool neg = x < 0;
    x = neg ? -x : x;
    char tmp[256];
    initConvertedResult(tmp+255,scale);
    int l = 255;
    while (x > 9) {
        tmp[l] = (char)((x % 10) + '0');
        l --;
        if (l == 255-scale) {
            tmp[l] = '.';
            l --;
        }
        x /= 10;
    }
    tmp[l] = (char)(x+ '0');
    int k = std::max(256-l,scale>0 ? scale+2 : 0);
    if (neg) {
        k++;
        tmp[256-k] = '-';
    }
    memcpy(buffer,tmp+256-k,k);
    return k;
}

inline int doPrintUint128(__uint128_t x, int scale,char *buffer) {
    char tmp[256];
    initConvertedResult(tmp+255,scale);
    int l = 255;
    while (x > 9) {
        tmp[l] = (char)((x % 10) + '0');
        l --;
        if (l == 255-scale) {
            tmp[l] = '.';
            l --;
        }
        x /= 10;
    }
    tmp[l] = (char)(x+ '0');
    int k = std::max(256-l,scale>0 ? scale+2 : 0);
    memcpy(buffer,tmp+256-k,k);
    return k;
}

inline int doPrintUint64(uint64_t x, int scale,char *buffer) {
    char tmp[256];
    initConvertedResult(tmp+255,scale);
    int l = 255;
    while (x > 9) {
        tmp[l] = (char)((x % 10) + '0');
        l --;
        if (l == 255-scale) {
            tmp[l] = '.';
            l --;
        }
        x /= 10;
    }
    tmp[l] = (char)(x+ '0');
    int k = std::max(256-l,scale>0 ? scale+2 : 0);
    memcpy(buffer,tmp+256-k,k);
    return k;
}

inline int printUint128(const __uint128_t &x, int scale,char *buffer) {
    if ((x>>64)==0) {
        return doPrintUint64(x,scale,buffer);
    } else {
        return doPrintUint128(x,scale,buffer);
    }
}


inline int convertToString(const __uint128_t &v,const mapping::mapping_entry &me,char *buffer){
    switch (me.t) {
        case mapping::index_type_boolean:
            if (v == 0) {
                memcpy(buffer,"false",5);
                return 5;
            } else {
                memcpy(buffer,"true",4);
                return 4;
            }
        case mapping::index_type_signed: {
            __int128_t w = SignExtend(v,me.length);
            return printInt128(w, (int)me.scale, buffer);
        }
        case mapping::index_type_unsigned:
            return printUint128(v,(int)me.scale,buffer);
        case mapping::index_type_float16: {
            float test_val(halfFloatToFloat(v));
            return floaxie::ftoa(test_val, buffer);
        }
        case mapping::index_type_float:{
            float test_val(uint32ToFloat(v));
            if (me.scale != 1) test_val *= (float)pow(10,-(float)me.scale);
            return floaxie::ftoa(test_val, buffer);
        }
        case mapping::index_type_double: {
            double test_val(uint64ToDouble(v));
            if (me.scale != 1) test_val *= pow(10,-(float)me.scale);
            return floaxie::ftoa(test_val, buffer);
        }
        case mapping::index_type_geopoint: {
            float lati, longi;
            Uint64ToLocation((int64_t)v,lati,longi);
            buffer[0] = '[';
            int o = 1;
            o += floaxie::ftoa(lati, buffer+o);
            buffer[o] = ',';
            o++;
            o += floaxie::ftoa(longi, buffer+o);
            buffer[o] = ']';
            o++;
            return  o;
        }
        case mapping::index_type_date: {
            std::chrono::duration<uint64_t, std::ratio<1, 1>> a(v);
            std::chrono::time_point <std::chrono::system_clock,std::chrono::duration<uint64_t, std::ratio<1, 1>>> tpu(a);
            std::string s= date::format(me.format, tpu);
            buffer[0] = '"';
            memcpy(buffer+1,s.c_str(),s.size());
            buffer[1+s.size()] = '"';
            return (int)s.size()+2;
        }
        case mapping::index_type_date_nano: {
            std::chrono::duration<uint64_t, std::ratio<1, 1000000000>> a(v);
            std::chrono::time_point <std::chrono::system_clock,std::chrono::duration<uint64_t, std::ratio<1, 1000000000>>> tpu(a);
            std::string s= date::format(me.format, tpu);
            buffer[0] = '"';
            memcpy(buffer+1,s.c_str(),s.size());
            buffer[1+s.size()] = '"';
            return (int)s.size()+2;
        }
        default:
            return {};
    }
}

inline float floatConvert(mapping::storage_types s,const std::string &v) {
	switch (s) {
	case mapping::storage_type_float16:
	case mapping::storage_type_float:
	case mapping::storage_type_geopoint: {
        float d;
        int o = sscanf(v.c_str(), "%f", &d);
        if (o != 1) {
            Log::error("Incorrect floating point number:" + v + " assuming 0");
            d = 0;
        }
        return d;
    }
    case mapping::storage_type_null:
    case mapping::storage_type_signed:
    case mapping::storage_type_unsigned:
    case mapping::storage_type_double:
    case mapping::storage_type_enum:
    case mapping::storage_type_signed_fixed_point:
    case mapping::storage_type_unsigned_fixed_point:
    case mapping::storage_type_unknown:
            break;
    }
	return 0;
}

inline double doubleConvert(mapping::storage_types s,const std::string &v) {
	switch (s) {
	case mapping::storage_type_double: {
        double d;
        int o = sscanf(v.c_str(), "%lf", &d);
        if (o != 1) {
            Log::error("Incorrect floating point number:" + v + " assuming 0");
            d = 0;
        }
        return d;
    }
	default:
	    break;
	}
	return 0;
}

// float/float16 stored as double
inline __uint128_t scale128Query(mapping::storage_types s,double scale,const __uint128_t &v, bool isFloat) {
    double f = isFloat ? from128d(v) : (float)(int64_t)v;
    switch (s) {
        case mapping::storage_type_signed:
        case mapping::storage_type_unsigned:
        case mapping::storage_type_geopoint:
        case mapping::storage_type_unknown:
            return v;
        case mapping::storage_type_signed_fixed_point:
            return (__int128_t)(scale * f);
        case mapping::storage_type_unsigned_fixed_point:
            return (__uint128_t)(scale * f);
        case mapping::storage_type_float:
            return floatToUint32((float)f);
        case mapping::storage_type_double:
            return doubleToUint64(f);
        case mapping::storage_type_float16:
            return floatToHalfFloat((float)(f * scale));
        case mapping::storage_type_null:
            return 0;
        case mapping::storage_type_enum:
            return v;
    }
    return 0;
}

inline __int128_t signExtend(const __int128_t &a,uint32_t len ) {
    __int128_t c = a;
    if (len < 128) {
        c = (a << (128-len)) >> (128-len);
    }
    return c;
}

[[maybe_unused]] inline int getBitLength(const __uint128_t &a) {
    int ret = 128;
    int k = 127;
    while (((a>>k)&1) ==0) {
        k --;
        ret --;
    }
    return ret;
}