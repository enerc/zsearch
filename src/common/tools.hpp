#pragma once

#include <cstdint>
#include <sys/time.h>
#include <cassert>
#include <iostream>
#include <cstring>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include "log.hpp"
#include "avx_conf.h"

inline uint64_t now() {
	struct timeval tv{};
	gettimeofday(&tv, nullptr);
	return (uint64_t) tv.tv_sec * 1000 * 1000 + tv.tv_usec;
}

inline std::string timeSpent(uint64_t oldClock) {
	uint64_t n = now();
	return std::to_string(n-oldClock) + "µS";
}

inline void fastMemcpy(void *pvDest, void *pvSrc, size_t nBytes) {
  assert(nBytes % 32 == 0);
  assert((intptr_t(pvDest) & 31) == 0);
  assert((intptr_t(pvSrc) & 31) == 0);
  const auto *pSrc = reinterpret_cast<const __m256i*>(pvSrc);
  auto *pDest = reinterpret_cast<__m256i*>(pvDest);
  auto nVects = (int64_t)(nBytes / sizeof(*pSrc));
  for (; nVects > 0; nVects--, pSrc++, pDest++) {
    const __m256i loaded = _mm256_stream_load_si256(pSrc);
    _mm256_stream_si256(pDest, loaded);
  }
  _mm_sfence();
}

inline void fastMemcpy4(void *pvDest, void *pvSrc, size_t nBytes) {
  const auto *pSrc = reinterpret_cast<const __m256i*>(pvSrc);
  auto *pDest = reinterpret_cast<__m256i*>(pvDest);
  auto nVects = (int64_t)(nBytes / sizeof(*pSrc) / 4);
  for (; nVects > 0; nVects--, pSrc+=4, pDest+=4) {
    const __m256i l0 = _mm256_stream_load_si256(pSrc);
    const __m256i l1 = _mm256_stream_load_si256(pSrc+1);
    const __m256i l2 = _mm256_stream_load_si256(pSrc+2);
    const __m256i l3 = _mm256_stream_load_si256(pSrc+3);
    _mm256_stream_si256(pDest, l0);
    _mm256_stream_si256(pDest+1, l1);
    _mm256_stream_si256(pDest+2, l2);
    _mm256_stream_si256(pDest+3, l3);
  }
  _mm_sfence();
}

inline void fastZeroed4(void *pvDest, size_t nBytes) {
  auto *pDest = reinterpret_cast<__m256i*>(pvDest);
  auto nVects = (int64_t)(nBytes / sizeof(*pDest) / 4);
  const __m256i l0 = _mm256_setzero_si256();
  for (; nVects > 0; nVects--,  pDest+=4) {
    _mm256_stream_si256(pDest, l0);
    _mm256_stream_si256(pDest+1, l0);
    _mm256_stream_si256(pDest+2, l0);
    _mm256_stream_si256(pDest+3, l0);
  }
  _mm_sfence();
}
void parZeroed4(void *pvDest, size_t nBytes);

inline void fastOne4(void *pvDest, size_t nBytes) {
    auto *pDest = reinterpret_cast<__m256i*>(pvDest);
    auto nVects = (int64_t)(nBytes / sizeof(*pDest) / 4);
    const __m256i l0 = _mm256_set_epi64x(-1L,-1L,-1L,-1L);
    for (; nVects > 0; nVects--,  pDest+=4) {
        _mm256_stream_si256(pDest, l0);
        _mm256_stream_si256(pDest+1, l0);
        _mm256_stream_si256(pDest+2, l0);
        _mm256_stream_si256(pDest+3, l0);
    }
    _mm_sfence();
}

inline void fastAnd(void *pvDest,const void *pvSrc1, const void *pvSrc2, size_t nBytes) {
    auto *pDest = reinterpret_cast<__m256i *>(pvDest);
    auto *pSrc1 = reinterpret_cast<const __m256i *>(pvSrc1);
    auto *pSrc2 = reinterpret_cast<const __m256i *>(pvSrc2);
    auto nVects = (int64_t)(nBytes / sizeof(*pDest) / 4);
    for (; nVects > 0; nVects--, pDest += 4,pSrc1 +=4,pSrc2+=4)  {
        const __m256i l0 = _mm256_stream_load_si256(pSrc1);
        const __m256i l1 = _mm256_stream_load_si256(pSrc1 + 1);
        const __m256i l2 = _mm256_stream_load_si256(pSrc1 + 2);
        const __m256i l3 = _mm256_stream_load_si256(pSrc1 + 3);
        const __m256i n0 = _mm256_stream_load_si256(pSrc2);
        const __m256i n1 = _mm256_stream_load_si256(pSrc2 + 1);
        const __m256i n2 = _mm256_stream_load_si256(pSrc2 + 2);
        const __m256i n3 = _mm256_stream_load_si256(pSrc2 + 3);
        const __m256i r0 = _mm256_and_si256(l0, n0);
        const __m256i r1 = _mm256_and_si256(l1, n1);
        const __m256i r2 = _mm256_and_si256(l2, n2);
        const __m256i r3 = _mm256_and_si256(l3, n3);
        _mm256_stream_si256(pDest, r0);
        _mm256_stream_si256(pDest+1, r1);
        _mm256_stream_si256(pDest+2, r2);
        _mm256_stream_si256(pDest+3, r3);
    }
}

inline void fastAndNot(void *pvDest,const void *pvSrc1, const void *pvSrc2, size_t nBytes) {
    auto *pDest = reinterpret_cast<__m256i *>(pvDest);
    auto *pSrc1 = reinterpret_cast<const __m256i *>(pvSrc1);
    auto *pSrc2 = reinterpret_cast<const __m256i *>(pvSrc2);
    auto nVects = (int64_t)(nBytes / sizeof(*pDest) / 4);
    for (; nVects > 0; nVects--, pDest += 4,pSrc1 +=4,pSrc2+=4)  {
        const __m256i l0 = _mm256_stream_load_si256(pSrc1);
        const __m256i l1 = _mm256_stream_load_si256(pSrc1 + 1);
        const __m256i l2 = _mm256_stream_load_si256(pSrc1 + 2);
        const __m256i l3 = _mm256_stream_load_si256(pSrc1 + 3);
        const __m256i n0 = _mm256_stream_load_si256(pSrc2);
        const __m256i n1 = _mm256_stream_load_si256(pSrc2 + 1);
        const __m256i n2 = _mm256_stream_load_si256(pSrc2 + 2);
        const __m256i n3 = _mm256_stream_load_si256(pSrc2 + 3);
        const __m256i r0 = _mm256_andnot_si256(n0, l0);
        const __m256i r1 = _mm256_andnot_si256(n1, l1);
        const __m256i r2 = _mm256_andnot_si256(n2, l2);
        const __m256i r3 = _mm256_andnot_si256(n3, l3);
        _mm256_stream_si256(pDest, r0);
        _mm256_stream_si256(pDest+1, r1);
        _mm256_stream_si256(pDest+2, r2);
        _mm256_stream_si256(pDest+3, r3);
    }
}

inline void fastOr(void *pvDest,const void *pvSrc1,const  void *pvSrc2, size_t nBytes) {
    auto *pDest = reinterpret_cast<__m256i *>(pvDest);
    auto *pSrc1 = reinterpret_cast<const __m256i *>(pvSrc1);
    auto *pSrc2 = reinterpret_cast<const __m256i *>(pvSrc2);
    auto nVects = (int64_t)(nBytes / sizeof(*pDest) / 4);
    for (; nVects > 0; nVects--, pDest += 4,pSrc1 +=4,pSrc2+=4)  {
        const __m256i l0 = _mm256_stream_load_si256(pSrc1);
        const __m256i l1 = _mm256_stream_load_si256(pSrc1 + 1);
        const __m256i l2 = _mm256_stream_load_si256(pSrc1 + 2);
        const __m256i l3 = _mm256_stream_load_si256(pSrc1 + 3);
        const __m256i n0 = _mm256_stream_load_si256(pSrc2);
        const __m256i n1 = _mm256_stream_load_si256(pSrc2 + 1);
        const __m256i n2 = _mm256_stream_load_si256(pSrc2 + 2);
        const __m256i n3 = _mm256_stream_load_si256(pSrc2 + 3);
        const __m256i r0 = _mm256_or_si256(n0, l0);
        const __m256i r1 = _mm256_or_si256(n1, l1);
        const __m256i r2 = _mm256_or_si256(n2, l2);
        const __m256i r3 = _mm256_or_si256(n3, l3);
        _mm256_stream_si256(pDest, r0);
        _mm256_stream_si256(pDest+1, r1);
        _mm256_stream_si256(pDest+2, r2);
        _mm256_stream_si256(pDest+3, r3);
    }
}

inline void fastXor(void *pvDest,const void *pvSrc1,const  void *pvSrc2, size_t nBytes) {
    auto *pDest = reinterpret_cast<__m256i *>(pvDest);
    auto *pSrc1 = reinterpret_cast<const __m256i *>(pvSrc1);
    auto *pSrc2 = reinterpret_cast<const __m256i *>(pvSrc2);
    auto nVects = (int64_t)(nBytes / sizeof(*pDest) / 4);
    for (; nVects > 0; nVects--, pDest += 4,pSrc1 +=4,pSrc2+=4)  {
        const __m256i l0 = _mm256_stream_load_si256(pSrc1);
        const __m256i l1 = _mm256_stream_load_si256(pSrc1 + 1);
        const __m256i l2 = _mm256_stream_load_si256(pSrc1 + 2);
        const __m256i l3 = _mm256_stream_load_si256(pSrc1 + 3);
        const __m256i n0 = _mm256_stream_load_si256(pSrc2);
        const __m256i n1 = _mm256_stream_load_si256(pSrc2 + 1);
        const __m256i n2 = _mm256_stream_load_si256(pSrc2 + 2);
        const __m256i n3 = _mm256_stream_load_si256(pSrc2 + 3);
        const __m256i r0 = _mm256_xor_si256(n0, l0);
        const __m256i r1 = _mm256_xor_si256(n1, l1);
        const __m256i r2 = _mm256_xor_si256(n2, l2);
        const __m256i r3 = _mm256_xor_si256(n3, l3);
        _mm256_stream_si256(pDest, r0);
        _mm256_stream_si256(pDest+1, r1);
        _mm256_stream_si256(pDest+2, r2);
        _mm256_stream_si256(pDest+3, r3);
    }
}

inline void fastNot(void *pvDest,const void *pvSrc, size_t nBytes) {
    auto *pDest = reinterpret_cast<__m256i *>(pvDest);
    auto *pSrc = reinterpret_cast<const __m256i *>(pvSrc);
    const __m256i l0 = _mm256_set_epi64x(-1L,-1L,-1L,-1L);
    auto nVects = (int64_t)(nBytes / sizeof(*pDest) / 4);
    for (; nVects > 0; nVects--, pDest += 4, pSrc+=4) {
        const __m256i n0 = _mm256_stream_load_si256(pSrc);
        const __m256i n1 = _mm256_stream_load_si256(pSrc + 1);
        const __m256i n2 = _mm256_stream_load_si256(pSrc + 2);
        const __m256i n3 = _mm256_stream_load_si256(pSrc + 3);
        const __m256i r0 = _mm256_xor_si256(n0, l0);
        const __m256i r1 = _mm256_xor_si256(n1, l0);
        const __m256i r2 = _mm256_xor_si256(n2, l0);
        const __m256i r3 = _mm256_xor_si256(n3, l0);
        _mm256_stream_si256(pDest, r0);
        _mm256_stream_si256(pDest+1, r1);
        _mm256_stream_si256(pDest+2, r2);
        _mm256_stream_si256(pDest+3, r3);
    }
}

inline uint64_t fastCount(const uint64_t *p, uint32_t size) {
    uint64_t  r = 0;
    for (int i=0 ; i < size/sizeof(uint64_t); i+=8, p+=8) {
        __builtin_prefetch(p+64);
        r += __builtin_popcountl(p[0]);
        r += __builtin_popcountl(p[1]);
        r += __builtin_popcountl(p[2]);
        r += __builtin_popcountl(p[3]);
        r += __builtin_popcountl(p[4]);
        r += __builtin_popcountl(p[5]);
        r += __builtin_popcountl(p[6]);
        r += __builtin_popcountl(p[7]);
    }
    return r;
}

constexpr __uint128_t ONELL = ~((__uint128_t)0);
inline __int128_t makeSigned(__uint128_t v, int length) {
    if (length == 128) return (__int128_t)v;
    bool b = (v >> (length - 1)) == 1;
    return b ? (__int128_t)((ONELL << length) | v) : (__int128_t)v;
}

// search above end of string, but that's ok
// like %hello%
// assume k > 1
template<size_t k> inline bool avx2_strstr(const __m256i avx2_search_needle_first_byte, const __m256i avx2_search_needle_last_byte, const char *s, size_t n, const char *needle) {
    for (size_t i = 0; i < n; i += 32) {
        const __m256i block_first = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(s + i));
        const __m256i block_last = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(s + i + k - 1));

        const __m256i eq_first = _mm256_cmpeq_epi8(avx2_search_needle_first_byte, block_first);
        const __m256i eq_last = _mm256_cmpeq_epi8(avx2_search_needle_last_byte, block_last);

        uint32_t mask = _mm256_movemask_epi8(_mm256_and_si256(eq_first, eq_last));
        if (n - i < 32) {
            mask &= (1 << (n - i)) - 1;
        }

        while (mask != 0) {
            const auto bitpos = __builtin_ctz(mask);
            if (memcmp(s + i + bitpos + 1, needle + 1, k - 2) == 0) {
                return (i + bitpos + k) <= n;
            }
            mask = mask & (mask - 1);
        }
    }
    return false;
}

inline bool avx2_strstr_any(const __m256i avx2_search_needle_first_byte, const __m256i avx2_search_needle_last_byte, const char *s, size_t n, const char *needle,size_t k) {
    for (size_t i = 0; i < n; i += 32) {
        const __m256i block_first = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(s + i));
        const __m256i block_last = _mm256_loadu_si256(reinterpret_cast<const __m256i *>(s + i + k - 1));

        const __m256i eq_first = _mm256_cmpeq_epi8(avx2_search_needle_first_byte, block_first);
        const __m256i eq_last = _mm256_cmpeq_epi8(avx2_search_needle_last_byte, block_last);

        uint32_t mask = _mm256_movemask_epi8(_mm256_and_si256(eq_first, eq_last));
        if (n - i < 32) {
            mask &= (1 << (n - i)) - 1;
        }

        while (mask != 0) {
            const auto bitpos = __builtin_ctz(mask);
            if (memcmp(s + i + bitpos + 1, needle + 1, k - 2) == 0) {
                return (i + bitpos + k) <= n;
            }
            mask = mask & (mask - 1);
        }
    }
    return false;
}

#if 0
// like hello%
inline bool avx2_strStartsWidth(const char* s, size_t n, const char* needle, size_t k) {
    if (n<k) {
        return false;
    }
    return (memcmp(s , needle , k ) == 0);
}

// like %hello
inline bool avx2_strEndsWith(const char* s, size_t n, const char* needle, size_t k) {
    if (n<k) {
        return false;
    }
    return memcmp(s+n-k,needle,k) == 0;
}
#endif
inline bool endsWith (const std::string &fullString, const std::string &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

inline size_t getMapSize(int fd) {
    lseek(fd, (size_t)0, SEEK_CUR);
    return lseek(fd, (size_t)0, SEEK_END) -1;
}

inline char *extendMapping(int fd, size_t extendSize, char *mapping) {
    size_t currentSize = getMapSize(fd);
    size_t newSize = currentSize + extendSize;
    msync(mapping,currentSize,MS_SYNC);
    lseek(fd,(off_t)newSize,SEEK_SET);
    if (write(fd,"\0",1) < 0) {
        Log::error("Failed to extend mmap file to "+ std::to_string(newSize));
        return nullptr;
    }
    munmap(mapping, currentSize);
    char *ret = (char*)mmap(nullptr, newSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if (ret == MAP_FAILED) {
        Log::error("Failed to extend mmap from "+std::to_string(currentSize) + " to " + std::to_string(newSize));
        return nullptr;
    }
    return ret;
}

inline __int128_t SignExtend(const __uint128_t&a, uint32_t len) {
    auto b = (__int128_t)(a << (128-len));
    return b >> (128-len);
}

inline int getKernelLength(const __uint128_t&a) {
    __uint128_t p = 1;
    __uint128_t b = (__int128_t)a < 0 ? -(__int128_t)a : a;
    if (b < p<<32) {
        return 32;
    } else if (b < p<<64) {
        return 64;
    } else {
        return 128;
    }
}

inline int getKernelLengthForColumn(uint32_t a) {
    if (a <= 32) {
        return 32;
    } else if (a <= 64) {
        return 64;
    } else {
        return 128;
    }
}