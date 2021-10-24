#pragma once

#include <cstdint>
#include "../common/int128.hpp"

uint32_t getCrc32c(uint32_t crc, const char *buf, size_t len);

[[maybe_unused]] __m128i  get128bitsHash(const char *buf, size_t len);
