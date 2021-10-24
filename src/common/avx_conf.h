#pragma once

#ifndef __AVX2__
#error "AVX2 required"
#else
#include <emmintrin.h>
#include <immintrin.h>
#endif

#ifndef __BMI2__
#error "BMI2 instruction support required"
#endif