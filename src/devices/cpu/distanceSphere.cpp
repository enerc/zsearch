#include <cmath>
#include "../../common/float16.hpp"
#include "../../common/log.hpp"
#include "kernels.hpp"

/*
   AVX implementation of sin, cos, sincos, exp and log
   Based on "sse_mathfun.h", by Julien Pommier
   http://gruntthepeon.free.fr/ssemath/
   Copyright (C) 2012 Giovanni Garberoglio
   Interdisciplinary Laboratory for Computational Science (LISC)
   Fondazione Bruno Kessler and University of Trento
   via Sommarive, 18
   I-38123 Trento (Italy)
  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.
  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:
  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
  (this is the zlib license)
*/
static const __m256i _ps256_sign_mask = _mm256_set_epi32(0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000, 0x80000000);
static const __m256i _ps256_inv_sign_mask = _mm256_set_epi32(0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff, 0x7fffffff);
static const __m256  _ps256_cephes_FOPI = _mm256_set_ps(1.27323954473516f, 1.27323954473516f, 1.27323954473516f, 1.27323954473516f, 1.27323954473516f, 1.27323954473516f,
                                                       1.27323954473516f, 1.27323954473516f);
static const __m256i _pi32_256_1 = _mm256_set_epi32(1, 1, 1, 1, 1, 1, 1, 1);
static const __m256i _pi32_256_inv1 = _mm256_set_epi32(~1, ~1, ~1, ~1, ~1, ~1, ~1, ~1);
static const __m256i _pi32_256_4 = _mm256_set_epi32(4, 4, 4, 4, 4, 4, 4, 4);
static const __m256i _pi32_256_2 = _mm256_set_epi32(2, 2, 2, 2, 2, 2, 2, 2);
static const __m256i _pi32_256_0 = _mm256_set_epi32(0, 0, 0, 0, 0, 0, 0, 0);
static const __m256 _ps256_minus_cephes_DP1 = _mm256_set_ps(-0.78515625, -0.78515625, -0.78515625, -0.78515625, -0.78515625, -0.78515625, -0.78515625, -0.78515625);
static const __m256 _ps256_minus_cephes_DP2 = _mm256_set_ps(-2.4187564849853515625e-4, -2.4187564849853515625e-4, -2.4187564849853515625e-4, -2.4187564849853515625e-4,
                                                            -2.4187564849853515625e-4, -2.4187564849853515625e-4, -2.4187564849853515625e-4, -2.4187564849853515625e-4);
static const __m256 _ps256_minus_cephes_DP3 = _mm256_set_ps(-3.77489497744594108e-8, -3.77489497744594108e-8, -3.77489497744594108e-8, -3.77489497744594108e-8,
                                                            -3.77489497744594108e-8, -3.77489497744594108e-8, -3.77489497744594108e-8, -3.77489497744594108e-8);
static const __m256 _ps256_sincof_p0 = _mm256_set_ps(-1.9515295891E-4, -1.9515295891E-4, -1.9515295891E-4, -1.9515295891E-4, -1.9515295891E-4, -1.9515295891E-4, -1.9515295891E-4,
                                                     -1.9515295891E-4);
static const __m256 _ps256_sincof_p1 = _mm256_set_ps(8.3321608736E-3, 8.3321608736E-3, 8.3321608736E-3, 8.3321608736E-3, 8.3321608736E-3, 8.3321608736E-3, 8.3321608736E-3,
                                                     8.3321608736E-3);
static const __m256 _ps256_sincof_p2 = _mm256_set_ps(-1.6666654611E-1, -1.6666654611E-1, -1.6666654611E-1, -1.6666654611E-1, -1.6666654611E-1, -1.6666654611E-1, -1.6666654611E-1,
                                                     -1.6666654611E-1);
static const __m256 _ps256_coscof_p0 = _mm256_set_ps(2.443315711809948E-005, 2.443315711809948E-005, 2.443315711809948E-005, 2.443315711809948E-005, 2.443315711809948E-005,
                                                     2.443315711809948E-005, 2.443315711809948E-005, 2.443315711809948E-005);
static const __m256 _ps256_coscof_p1 = _mm256_set_ps(-1.388731625493765E-003, -1.388731625493765E-003, -1.388731625493765E-003, -1.388731625493765E-003, -1.388731625493765E-003,
                                                     -1.388731625493765E-003, -1.388731625493765E-003, -1.388731625493765E-003);
static const __m256 _ps256_coscof_p2 = _mm256_set_ps(4.166664568298827E-002, 4.166664568298827E-002, 4.166664568298827E-002, 4.166664568298827E-002, 4.166664568298827E-002,
                                                     4.166664568298827E-002, 4.166664568298827E-002, 4.166664568298827E-002);
static const __m256 _ps256_0p5 = _mm256_set_ps(0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5);
static const __m256 _ps256_1 = _mm256_set_ps(1, 1, 1, 1, 1, 1, 1, 1);
static const __m256 _ps256_0625 = _mm256_set_ps(0.625, 0.625, 0.625, 0.625, 0.625, 0.625, 0.625, 0.625);

static inline void sin256_ps(__m256 xa, __m256 xb, __m256 &ya, __m256 &yb) {
    __m256 xmm1, xmm2, xmm3, sign_bit;
    __m256i imm0, imm2;

    sign_bit = xa;
    // take the absolute value
    xa = _mm256_and_ps(xa, *(__m256 *) &_ps256_inv_sign_mask);
    /* extract the sign bit (upper one) */
    sign_bit = _mm256_and_ps(sign_bit, *(__m256 *) &_ps256_sign_mask);

    /* scale by 4/Pi */
    ya = _mm256_mul_ps(xa, _ps256_cephes_FOPI);

    /* store the integer part of y in mm0 */
    imm2 = _mm256_cvttps_epi32(ya);
    /* j=(j+1) & (~1) (see the cephes sources) */
    // another two AVX2 instruction
    imm2 = _mm256_add_epi32(imm2, _pi32_256_1);
    imm2 = _mm256_and_si256(imm2, _pi32_256_inv1);
    ya = _mm256_cvtepi32_ps(imm2);

    /* get the swap sign flag */
    imm0 = _mm256_and_si256(imm2, _pi32_256_4);
    imm0 = _mm256_slli_epi32(imm0, 29);
    /* get the polynom selection mask
       there is one polynom for 0 <= x <= Pi/4
       and another one for Pi/4<x<=Pi/2
       Both branches will be computed.
    */
    imm2 = _mm256_and_si256(imm2, _pi32_256_2);
    imm2 = _mm256_cmpeq_epi32(imm2, _pi32_256_0);

    __m256 swap_sign_bit = _mm256_castsi256_ps(imm0);
    __m256 poly_mask = _mm256_castsi256_ps(imm2);
    sign_bit = _mm256_xor_ps(sign_bit, swap_sign_bit);

    /* The magic pass: "Extended precision modular arithmetic"
       x = ((x - y * DP1) - y * DP2) - y * DP3; */
    xmm1 = _mm256_mul_ps(ya, _ps256_minus_cephes_DP1);
    xmm2 = _mm256_mul_ps(ya, _ps256_minus_cephes_DP2);
    xmm3 = _mm256_mul_ps(ya, _ps256_minus_cephes_DP3);
    xa = _mm256_add_ps(xa, xmm1);
    xa = _mm256_add_ps(xa, xmm2);
    xa = _mm256_add_ps(xa, xmm3);

    /* Evaluate the first polynom  (0 <= x <= Pi/4) */
    ya = _ps256_coscof_p0;
    __m256 z = _mm256_mul_ps(xa, xa);

    ya = _mm256_mul_ps(ya, z);
    ya = _mm256_add_ps(ya, _ps256_coscof_p1);
    ya = _mm256_mul_ps(ya, z);
    ya = _mm256_add_ps(ya, _ps256_coscof_p2);
    ya = _mm256_mul_ps(ya, z);
    ya = _mm256_mul_ps(ya, z);
    __m256 tmp = _mm256_mul_ps(z, _ps256_0p5);
    ya = _mm256_sub_ps(ya, tmp);
    ya = _mm256_add_ps(ya, _ps256_1);

    // Evaluate the second polynom  (Pi/4 <= x <= 0)
    __m256 y2 = _ps256_sincof_p0;
    y2 = _mm256_mul_ps(y2, z);
    y2 = _mm256_add_ps(y2, _ps256_sincof_p1);
    y2 = _mm256_mul_ps(y2, z);
    y2 = _mm256_add_ps(y2, _ps256_sincof_p2);
    y2 = _mm256_mul_ps(y2, z);
    y2 = _mm256_mul_ps(y2, xa);
    y2 = _mm256_add_ps(y2, xa);

    // select the correct result from the two polynoms
    xmm3 = poly_mask;
    y2 = _mm256_and_ps(xmm3, y2); //, xmm3);
    ya = _mm256_andnot_ps(xmm3, ya);
    ya = _mm256_add_ps(ya, y2);
    // update the sign
    ya = _mm256_xor_ps(ya, sign_bit);


    sign_bit = xb;
    // take the absolute value
    xb = _mm256_and_ps(xb, *(__m256 *) &_ps256_inv_sign_mask);
    // extract the sign bit (upper one)
    sign_bit = _mm256_and_ps(sign_bit, *(__m256 *) &_ps256_sign_mask);

    // scale by 4/Pi
    yb = _mm256_mul_ps(xb, _ps256_cephes_FOPI);

    // store the integer part of y in mm0
    imm2 = _mm256_cvttps_epi32(yb);
    // j=(j+1) & (~1) (see the cephes sources)
    // another two AVX2 instruction
    imm2 = _mm256_add_epi32(imm2, _pi32_256_1);
    imm2 = _mm256_and_si256(imm2, _pi32_256_inv1);
    yb = _mm256_cvtepi32_ps(imm2);

    // get the swap sign flag
    imm0 = _mm256_and_si256(imm2, _pi32_256_4);
    imm0 = _mm256_slli_epi32(imm0, 29);
    /* get the polynom selection mask
       there is one polynom for 0 <= x <= Pi/4
       and another one for Pi/4<x<=Pi/2
       Both branches will be computed.
    */
    imm2 = _mm256_and_si256(imm2, _pi32_256_2);
    imm2 = _mm256_cmpeq_epi32(imm2, _pi32_256_0);

    swap_sign_bit = _mm256_castsi256_ps(imm0);
    poly_mask = _mm256_castsi256_ps(imm2);
    sign_bit = _mm256_xor_ps(sign_bit, swap_sign_bit);

    /* The magic pass: "Extended precision modular arithmetic"
       x = ((x - y * DP1) - y * DP2) - y * DP3; */
    xmm1 = _mm256_mul_ps(yb, _ps256_minus_cephes_DP1);
    xmm2 = _mm256_mul_ps(yb, _ps256_minus_cephes_DP2);
    xmm3 = _mm256_mul_ps(yb, _ps256_minus_cephes_DP3);
    xb = _mm256_add_ps(xb, xmm1);
    xb = _mm256_add_ps(xb, xmm2);
    xb = _mm256_add_ps(xb, xmm3);

    // Evaluate the first polynom  (0 <= x <= Pi/4)
    yb = _ps256_coscof_p0;
    z = _mm256_mul_ps(xb, xb);

    yb = _mm256_mul_ps(yb, z);
    yb = _mm256_add_ps(yb, _ps256_coscof_p1);
    yb = _mm256_mul_ps(yb, z);
    yb = _mm256_add_ps(yb, _ps256_coscof_p2);
    yb = _mm256_mul_ps(yb, z);
    yb = _mm256_mul_ps(yb, z);
    tmp = _mm256_mul_ps(z, _ps256_0p5);
    yb = _mm256_sub_ps(yb, tmp);
    yb = _mm256_add_ps(yb, _ps256_1);

    // Evaluate the second polynom  (Pi/4 <= x <= 0)

    y2 = *(__m256 *) &_ps256_sincof_p0;
    y2 = _mm256_mul_ps(y2, z);
    y2 = _mm256_add_ps(y2, _ps256_sincof_p1);
    y2 = _mm256_mul_ps(y2, z);
    y2 = _mm256_add_ps(y2, _ps256_sincof_p2);
    y2 = _mm256_mul_ps(y2, z);
    y2 = _mm256_mul_ps(y2, xb);
    y2 = _mm256_add_ps(y2, xb);

    // select the correct result from the two polynoms
    xmm3 = poly_mask;
    y2 = _mm256_and_ps(xmm3, y2); //, xmm3);
    yb = _mm256_andnot_ps(xmm3, yb);
    yb = _mm256_add_ps(yb, y2);
    // update the sign
    yb = _mm256_xor_ps(yb, sign_bit);
}

static inline void cos256_ps(__m256 xa, __m256 xb, __m256 &ya, __m256 &yb) {
    __m256 xmm1, xmm2, xmm3;
    __m256i imm0, imm2;

    // take the absolute value
    xa = _mm256_and_ps(xa, *(__m256 *) &_ps256_inv_sign_mask);

    /* scale by 4/Pi */
    ya = _mm256_mul_ps(xa, _ps256_cephes_FOPI);

    /* store the integer part of y in mm0 */
    imm2 = _mm256_cvttps_epi32(ya);
    /* j=(j+1) & (~1) (see the cephes sources) */
    // another two AVX2 instruction
    imm2 = _mm256_add_epi32(imm2, _pi32_256_1);
    imm2 = _mm256_and_si256(imm2, _pi32_256_inv1);
    ya = _mm256_cvtepi32_ps(imm2);
    imm2 = _mm256_sub_epi32(imm2, _pi32_256_2);

    /* get the swap sign flag */
    imm0 = _mm256_andnot_si256(imm2, _pi32_256_4);
    imm0 = _mm256_slli_epi32(imm0, 29);
    /* get the polynom selection mask
       there is one polynom for 0 <= x <= Pi/4
       and another one for Pi/4<x<=Pi/2
       Both branches will be computed.
    */
    imm2 = _mm256_and_si256(imm2, _pi32_256_2);
    imm2 = _mm256_cmpeq_epi32(imm2,_pi32_256_0);

    __m256 sign_bit = _mm256_castsi256_ps(imm0);
    __m256 poly_mask = _mm256_castsi256_ps(imm2);

    /* The magic pass: "Extended precision modular arithmetic"
       x = ((x - y * DP1) - y * DP2) - y * DP3; */
    xmm1 = _mm256_mul_ps(ya, _ps256_minus_cephes_DP1);
    xmm2 = _mm256_mul_ps(ya, _ps256_minus_cephes_DP2);
    xmm3 = _mm256_mul_ps(ya, _ps256_minus_cephes_DP3);
    xa = _mm256_add_ps(xa, xmm1);
    xa = _mm256_add_ps(xa, xmm2);
    xa = _mm256_add_ps(xa, xmm3);

    /* Evaluate the first polynom  (0 <= x <= Pi/4) */
    ya = _ps256_coscof_p0;
    __m256 z = _mm256_mul_ps(xa, xa);

    ya = _mm256_mul_ps(ya, z);
    ya = _mm256_add_ps(ya, _ps256_coscof_p1);
    ya = _mm256_mul_ps(ya, z);
    ya = _mm256_add_ps(ya, _ps256_coscof_p2);
    ya = _mm256_mul_ps(ya, z);
    ya = _mm256_mul_ps(ya, z);
    __m256 tmp = _mm256_mul_ps(z, _ps256_0p5);
    ya = _mm256_sub_ps(ya, tmp);
    ya = _mm256_add_ps(ya, *(__m256 *) &_ps256_1);

    /* Evaluate the second polynom  (Pi/4 <= x <= 0) */
    __m256 y2 = _ps256_sincof_p0;
    y2 = _mm256_mul_ps(y2, z);
    y2 = _mm256_add_ps(y2, _ps256_sincof_p1);
    y2 = _mm256_mul_ps(y2, z);
    y2 = _mm256_add_ps(y2, _ps256_sincof_p2);
    y2 = _mm256_mul_ps(y2, z);
    y2 = _mm256_mul_ps(y2, xa);
    y2 = _mm256_add_ps(y2, xa);

    // select the correct result from the two polynoms
    xmm3 = poly_mask;
    y2 = _mm256_and_ps(xmm3, y2); //, xmm3);
    ya = _mm256_andnot_ps(xmm3, ya);
    ya = _mm256_add_ps(ya, y2);
    // update the sign
    ya = _mm256_xor_ps(ya, sign_bit);


    // take the absolute value
    xb = _mm256_and_ps(xb, *(__m256 *)&_ps256_inv_sign_mask);

    // scale by 4/Pi
    yb = _mm256_mul_ps(xb, _ps256_cephes_FOPI);

    // store the integer part of y in mm0
    imm2 = _mm256_cvttps_epi32(yb);
    // j=(j+1) & (~1) (see the cephes sources)
    // another two AVX2 instruction
    imm2 = _mm256_add_epi32(imm2, _pi32_256_1);
    imm2 = _mm256_and_si256(imm2, _pi32_256_inv1);
    yb = _mm256_cvtepi32_ps(imm2);
    imm2 = _mm256_sub_epi32(imm2, _pi32_256_2);

    // get the swap sign flag
    imm0 = _mm256_andnot_si256(imm2, _pi32_256_4);
    imm0 = _mm256_slli_epi32(imm0, 29);
    /* get the polynom selection mask
       there is one polynom for 0 <= x <= Pi/4
       and another one for Pi/4<x<=Pi/2
       Both branches will be computed.
    */
    imm2 = _mm256_and_si256(imm2, _pi32_256_2);
    imm2 = _mm256_cmpeq_epi32(imm2, _pi32_256_0);

    sign_bit = _mm256_castsi256_ps(imm0);
    poly_mask = _mm256_castsi256_ps(imm2);

    /* The magic pass: "Extended precision modular arithmetic"
       x = ((x - y * DP1) - y * DP2) - y * DP3; */
    xmm1 = _mm256_mul_ps(yb, _ps256_minus_cephes_DP1);
    xmm2 = _mm256_mul_ps(yb, _ps256_minus_cephes_DP2);
    xmm3 = _mm256_mul_ps(yb, _ps256_minus_cephes_DP3);
    xb = _mm256_add_ps(xb, xmm1);
    xb = _mm256_add_ps(xb, xmm2);
    xb = _mm256_add_ps(xb, xmm3);

    // Evaluate the first polynom  (0 <= x <= Pi/4)
    yb = *(__m256 *) &_ps256_coscof_p0;
    z = _mm256_mul_ps(xb, xb);

    yb = _mm256_mul_ps(yb, z);
    yb = _mm256_add_ps(yb, _ps256_coscof_p1);
    yb = _mm256_mul_ps(yb, z);
    yb = _mm256_add_ps(yb, _ps256_coscof_p2);
    yb = _mm256_mul_ps(yb, z);
    yb = _mm256_mul_ps(yb, z);
    tmp = _mm256_mul_ps(z, _ps256_0p5);
    yb = _mm256_sub_ps(yb, tmp);
    yb = _mm256_add_ps(yb, _ps256_1);

    // Evaluate the second polynom  (Pi/4 <= x <= 0)
    y2 = *(__m256 *) &_ps256_sincof_p0;
    y2 = _mm256_mul_ps(y2, z);
    y2 = _mm256_add_ps(y2, _ps256_sincof_p1);
    y2 = _mm256_mul_ps(y2, z);
    y2 = _mm256_add_ps(y2, _ps256_sincof_p2);
    y2 = _mm256_mul_ps(y2, z);
    y2 = _mm256_mul_ps(y2, xb);
    y2 = _mm256_add_ps(y2, xb);

    // select the correct result from the two polynoms
    xmm3 = poly_mask;
    y2 = _mm256_and_ps(xmm3, y2); //, xmm3);
    yb = _mm256_andnot_ps(xmm3, yb);
    yb = _mm256_add_ps(yb, y2);
    // update the sign
    yb = _mm256_xor_ps(yb, sign_bit);
}

/*
Cephes Math Library Release 2.1:  December, 1988
Copyright 1984, 1987, 1988 by Stephen L. Moshier
Direct inquiries to 30 Frost Street, Cambridge, MA 02140
*/
static const __m256 AsinR0 = _mm256_set_ps(2.967721961301243206100E-3, 2.967721961301243206100E-3, 2.967721961301243206100E-3, 2.967721961301243206100E-3,
                                           2.967721961301243206100E-3, 2.967721961301243206100E-3, 2.967721961301243206100E-3, 2.967721961301243206100E-3);
static const __m256 AsinR1 = _mm256_set_ps(-5.634242780008963776856E-1, -5.634242780008963776856E-1, -5.634242780008963776856E-1, -5.634242780008963776856E-1,
                                           -5.634242780008963776856E-1, -5.634242780008963776856E-1, -5.634242780008963776856E-1, -5.634242780008963776856E-1);
static const __m256 AsinR2 = _mm256_set_ps(6.968710824104713396794E0, 6.968710824104713396794E0, 6.968710824104713396794E0, 6.968710824104713396794E0, 6.968710824104713396794E0,
                                           6.968710824104713396794E0, 6.968710824104713396794E0, 6.968710824104713396794E0);
static const __m256 AsinR3 = _mm256_set_ps(-2.556901049652824852289E1, -2.556901049652824852289E1, -2.556901049652824852289E1, -2.556901049652824852289E1,
                                           -2.556901049652824852289E1, -2.556901049652824852289E1, -2.556901049652824852289E1, -2.556901049652824852289E1);
static const __m256 AsinR4 = _mm256_set_ps(2.853665548261061424989E1, 2.853665548261061424989E1, 2.853665548261061424989E1, 2.853665548261061424989E1, 2.853665548261061424989E1,
                                           2.853665548261061424989E1, 2.853665548261061424989E1, 2.853665548261061424989E1);

static const __m256 AsinS1 = _mm256_set_ps(-2.194779531642920639778E1, -2.194779531642920639778E1, -2.194779531642920639778E1, -2.194779531642920639778E1,
                                           -2.194779531642920639778E1, -2.194779531642920639778E1, -2.194779531642920639778E1, -2.194779531642920639778E1);
static const __m256 AsinS2 = _mm256_set_ps(1.470656354026814941758E2, 1.470656354026814941758E2, 1.470656354026814941758E2, 1.470656354026814941758E2, 1.470656354026814941758E2,
                                           1.470656354026814941758E2, 1.470656354026814941758E2, 1.470656354026814941758E2);
static const __m256 AsinS3 = _mm256_set_ps(-3.838770957603691357202E2, -3.838770957603691357202E2, -3.838770957603691357202E2, -3.838770957603691357202E2,
                                           -3.838770957603691357202E2, -3.838770957603691357202E2, -3.838770957603691357202E2, -3.838770957603691357202E2);
static const __m256 AsinS4 = _mm256_set_ps(3.424398657913078477438E2, 3.424398657913078477438E2, 3.424398657913078477438E2, 3.424398657913078477438E2, 3.424398657913078477438E2,
                                           3.424398657913078477438E2, 3.424398657913078477438E2, 3.424398657913078477438E2);
static const __m256 AsinPIO4 = _mm256_set_ps(7.85398163397448309616E-1, 7.85398163397448309616E-1, 7.85398163397448309616E-1, 7.85398163397448309616E-1, 7.85398163397448309616E-1,
                                             7.85398163397448309616E-1, 7.85398163397448309616E-1, 7.85398163397448309616E-1);
static const __m256 AsinMoreBits = _mm256_set_ps(6.123233995736765886130E-17, 6.123233995736765886130E-17, 6.123233995736765886130E-17, 6.123233995736765886130E-17,
                                                 6.123233995736765886130E-17, 6.123233995736765886130E-17, 6.123233995736765886130E-17, 6.123233995736765886130E-17);

static const __m256 AsinP0 = _mm256_set_ps(4.253011369004428248960E-3, 4.253011369004428248960E-3, 4.253011369004428248960E-3, 4.253011369004428248960E-3,
                                           4.253011369004428248960E-3, 4.253011369004428248960E-3, 4.253011369004428248960E-3, 4.253011369004428248960E-3);
static const __m256 AsinP1 = _mm256_set_ps(-6.019598008014123785661E-1, -6.019598008014123785661E-1, -6.019598008014123785661E-1, -6.019598008014123785661E-1,
                                           -6.019598008014123785661E-1, -6.019598008014123785661E-1, -6.019598008014123785661E-1, -6.019598008014123785661E-1);
static const __m256 AsinP2 = _mm256_set_ps(5.444622390564711410273E0, 5.444622390564711410273E0, 5.444622390564711410273E0, 5.444622390564711410273E0, 5.444622390564711410273E0,
                                           5.444622390564711410273E0, 5.444622390564711410273E0, 5.444622390564711410273E0);
static const __m256 AsinP3 = _mm256_set_ps(-1.626247967210700244449E1, -1.626247967210700244449E1, -1.626247967210700244449E1, -1.626247967210700244449E1,
                                           -1.626247967210700244449E1, -1.626247967210700244449E1, -1.626247967210700244449E1, -1.626247967210700244449E1);
static const __m256 AsinP4 = _mm256_set_ps(1.956261983317594739197E1, 1.956261983317594739197E1, 1.956261983317594739197E1, 1.956261983317594739197E1, 1.956261983317594739197E1,
                                           1.956261983317594739197E1, 1.956261983317594739197E1, 1.956261983317594739197E1);
static const __m256 AsinP5 = _mm256_set_ps(-8.198089802484824371615E0, -8.198089802484824371615E0, -8.198089802484824371615E0, -8.198089802484824371615E0,
                                           -8.198089802484824371615E0, -8.198089802484824371615E0, -8.198089802484824371615E0, -8.198089802484824371615E0);

static const __m256 AsinQ1 = _mm256_set_ps(-1.474091372988853791896E1, -1.474091372988853791896E1, -1.474091372988853791896E1, -1.474091372988853791896E1,
                                           -1.474091372988853791896E1, -1.474091372988853791896E1, -1.474091372988853791896E1, -1.474091372988853791896E1);
static const __m256 AsinQ2 = _mm256_set_ps(7.049610280856842141659E1, 7.049610280856842141659E1, 7.049610280856842141659E1, 7.049610280856842141659E1, 7.049610280856842141659E1,
                                           7.049610280856842141659E1, 7.049610280856842141659E1, 7.049610280856842141659E1);
static const __m256 AsinQ3 = _mm256_set_ps(-1.471791292232726029859E2, -1.471791292232726029859E2, -1.471791292232726029859E2, -1.471791292232726029859E2,
                                           -1.471791292232726029859E2, -1.471791292232726029859E2, -1.471791292232726029859E2, -1.471791292232726029859E2);
static const __m256 AsinQ4 = _mm256_set_ps(1.395105614657485689735E2, 1.395105614657485689735E2, 1.395105614657485689735E2, 1.395105614657485689735E2, 1.395105614657485689735E2,
                                           1.395105614657485689735E2, 1.395105614657485689735E2, 1.395105614657485689735E2);
static const __m256 AsinQ5 = _mm256_set_ps(-4.918853881490881290097E1, -4.918853881490881290097E1, -4.918853881490881290097E1, -4.918853881490881290097E1,
                                           -4.918853881490881290097E1, -4.918853881490881290097E1, -4.918853881490881290097E1, -4.918853881490881290097E1);

//extern __m256 mmDebug0[2];


static inline void asin256_ps(const __m256 &x, __m256 &y) {
    __m256 a = x;
    __m256 sign_bit = a;
    // take the absolute value
    a = _mm256_and_ps(a, *(__m256 *) &_ps256_inv_sign_mask);
    sign_bit = _mm256_and_ps(sign_bit, *(__m256 *) &_ps256_sign_mask);

    // x should be < 11
    // if ( a > 0.625 )
    auto branch = _mm256_cmp_ps(a, _ps256_0625, _CMP_GT_OS);
    // arcsin(1-x) = pi/2 - sqrt(2x)(1+R(x))
    // zz = 1.0 - a;
    __m256 zz = _mm256_sub_ps(_ps256_1, a);

    // polevl( zz, R, 4)
    __m256 ans = _mm256_mul_ps(zz, AsinR0);
    ans = _mm256_add_ps(ans, AsinR1);
    ans = _mm256_mul_ps(zz, ans);
    ans = _mm256_add_ps(ans, AsinR2);
    ans = _mm256_mul_ps(zz, ans);
    ans = _mm256_add_ps(ans, AsinR3);
    ans = _mm256_mul_ps(zz, ans);
    ans = _mm256_add_ps(ans, AsinR4);
    // p1evl( zz, S, 4)
    __m256 ans2 = _mm256_add_ps(zz, AsinS1);
    ans2 = _mm256_mul_ps(zz, ans2);
    ans2 = _mm256_add_ps(ans2, AsinS2);
    ans2 = _mm256_mul_ps(zz, ans2);
    ans2 = _mm256_add_ps(ans2, AsinS3);
    ans2 = _mm256_mul_ps(zz, ans2);
    ans2 = _mm256_add_ps(ans2, AsinS4);
    //  p = zz * polevl( zz, R, 4)/p1evl( zz, S, 4);
    __m256 p = _mm256_div_ps(ans, ans2);
    p = _mm256_mul_ps(zz, p);
    // zz = sqrt(zz+zz);
    zz = _mm256_add_ps(zz, zz);
    zz = _mm256_sqrt_ps(zz);
    // z = PIO4 - zz;
    __m256 z = _mm256_sub_ps(AsinPIO4, zz);
    // zz = zz * p - MOREBITS;
    zz = _mm256_mul_ps(zz, p);
    zz = _mm256_sub_ps(zz, AsinMoreBits);
    // z = z - zz;
    z = _mm256_sub_ps(z, zz);
    // z = z + PIO4;
    z = _mm256_add_ps(z, AsinPIO4);

    // zz = a * a;
    __m256 zz2 = _mm256_mul_ps(a, a);
    // polevl( zz, P, 5)
    ans = _mm256_mul_ps(zz2, AsinP0);
    ans = _mm256_add_ps(ans, AsinP1);
    ans = _mm256_mul_ps(zz2, ans);
    ans = _mm256_add_ps(ans, AsinP2);
    ans = _mm256_mul_ps(zz2, ans);
    ans = _mm256_add_ps(ans, AsinP3);
    ans = _mm256_mul_ps(zz2, ans);
    ans = _mm256_add_ps(ans, AsinP4);
    ans = _mm256_mul_ps(zz2, ans);
    ans = _mm256_add_ps(ans, AsinP5);
    // p1evl( zz, Q, 5)
    ans2 = _mm256_add_ps(zz2, AsinQ1);
    ans2 = _mm256_mul_ps(zz2, ans2);
    ans2 = _mm256_add_ps(ans2, AsinQ2);
    ans2 = _mm256_mul_ps(zz2, ans2);
    ans2 = _mm256_add_ps(ans2, AsinQ3);
    ans2 = _mm256_mul_ps(zz2, ans2);
    ans2 = _mm256_add_ps(ans2, AsinQ4);
    ans2 = _mm256_mul_ps(zz2, ans2);
    ans2 = _mm256_add_ps(ans2, AsinQ5);
    // z = zz * polevl( zz, P, 5)/p1evl( zz, Q, 5);
    __m256 p2 = _mm256_div_ps(ans, ans2);
    p2 = _mm256_mul_ps(zz2, p2);
    // z = a * z + a;
    __m256 z2 = _mm256_mul_ps(a, p2);
    z2 = _mm256_add_ps(z2, a);

    p = _mm256_and_ps(branch, z);
    p2 = _mm256_andnot_ps(branch, z2);
    y = _mm256_add_ps(p, p2);
    // update the sign
    y = _mm256_xor_ps(y, sign_bit);
}

constexpr double GeometryScalingRadianLat = (1.0/GEOMETRY_SCALING_LAT)*M_PI / 180.;
constexpr double GeometryScalingRadianLon = (1.0/GEOMETRY_SCALING_LON)*M_PI / 180.;
static const __m256d GeometryScalingRadian256d = _mm256_set_pd(GeometryScalingRadianLat,GeometryScalingRadianLon,GeometryScalingRadianLat,GeometryScalingRadianLon);
static const __m256 GeometryScalingRadian256 = _mm256_set_ps(GeometryScalingRadianLat,GeometryScalingRadianLon,GeometryScalingRadianLat,GeometryScalingRadianLon,GeometryScalingRadianLat,GeometryScalingRadianLon,GeometryScalingRadianLat,GeometryScalingRadianLon);
static const __m256 sincosMask256 = _mm256_set_ps(1,0,1,0,1,0,1,0);
static const __m256 earthConstant = _mm256_set_ps(6371000.0 * 2,6371000.0 * 2,6371000.0 * 2,6371000.0 * 2,6371000.0 * 2,6371000.0 * 2,6371000.0 * 2,6371000.0 * 2);

__m256 cal_distance_sphere(__m256i x0,__m256i x1, __m256d positionSphereCenter256, __m256 cosPositionSphereOrigin256) {
    // to double, then substract the center, then back to float
    __m256d tmp0 = _mm256_cvtepi32_pd(_mm256_castsi256_si128(x0));
    __m256d tmp1 = _mm256_cvtepi32_pd(_mm256_castsi256_si128(x1));
    __m256d tmp2 = _mm256_cvtepi32_pd(_mm256_castsi256_si128(_mm256_permute2x128_si256(x0,x0,0x11)));
    __m256d tmp3 = _mm256_cvtepi32_pd(_mm256_castsi256_si128(_mm256_permute2x128_si256(x1,x1,0x11)));
    // int to radian
    tmp0 = _mm256_mul_pd(tmp0,GeometryScalingRadian256d);
    tmp1 = _mm256_mul_pd(tmp1,GeometryScalingRadian256d);
    tmp2 = _mm256_mul_pd(tmp2,GeometryScalingRadian256d);
    tmp3 = _mm256_mul_pd(tmp3,GeometryScalingRadian256d);
    // substract center position of search
    tmp0 = _mm256_sub_pd(positionSphereCenter256,tmp0);
    tmp1 = _mm256_sub_pd(positionSphereCenter256,tmp1);
    tmp2 = _mm256_sub_pd(positionSphereCenter256,tmp2);
    tmp3 = _mm256_sub_pd(positionSphereCenter256,tmp3);
    // back to float
    __m128 i0 = _mm256_cvtpd_ps(tmp0);
    __m128 i1 = _mm256_cvtpd_ps(tmp1);
    __m128 i2 = _mm256_cvtpd_ps(tmp2);
    __m128 i3 = _mm256_cvtpd_ps(tmp3);

    auto a0 =  _mm256_permute2f128_ps(_mm256_castps128_ps256(i0),_mm256_castps128_ps256(i2),0x20);
    auto a1 =  _mm256_permute2f128_ps(_mm256_castps128_ps256(i1),_mm256_castps128_ps256(i3),0x20);
    // *0.5
    a0 = _mm256_mul_ps(a0,_ps256_0p5);
    a1 = _mm256_mul_ps(a1,_ps256_0p5);

    // float A = sin((pt2[1] - pt1[1]) * M_PI / 180. / 2.);
    // float B = sin((pt2[0] - pt1[0]) * M_PI / 180. / 2);
    __m256 sca0,sca1;
    sin256_ps(a0,a1,sca0,sca1);
    sca0 = _mm256_mul_ps(sca0,sca0);
    sca1 = _mm256_mul_ps(sca1,sca1);
    sca0 = _mm256_mul_ps(sca0,cosPositionSphereOrigin256);
    sca1 = _mm256_mul_ps(sca1,cosPositionSphereOrigin256);

    // cos(pt1[1] * M_PI / 180.)
    a0 = _mm256_cvtepi32_ps(x0);
    a1 = _mm256_cvtepi32_ps(x1);
    a0 = _mm256_mul_ps(a0,GeometryScalingRadian256);
    a1 = _mm256_mul_ps(a1,GeometryScalingRadian256);
    // cos 0 = 1
    a0 = _mm256_mul_ps(a0,sincosMask256);
    a1 = _mm256_mul_ps(a1,sincosMask256);
    __m256 ca0,ca1;
    cos256_ps(a0,a1,ca0,ca1);

    ca0 = _mm256_shuffle_ps(ca0,ca0,0b10110001);
    ca1 = _mm256_shuffle_ps(ca1,ca1,0b10110001);

    // A*A + cos(pt1[1] * M_PI / 180.) * cos(pt2[1] * M_PI / 180.) * B*B
    sca0 = _mm256_mul_ps(sca0,ca0);
    sca1 = _mm256_mul_ps(sca1,ca1);
    sca0 = _mm256_add_ps(sca0,_mm256_permute_ps(sca0,0b10110001));
    sca1 = _mm256_add_ps(sca1,_mm256_permute_ps(sca1,0b10110001));

    // reorganise to one vector
    sca0 = _mm256_castsi256_ps(_mm256_shuffle_epi32(_mm256_castps_si256(sca0),0b1000));
    sca1 = _mm256_castsi256_ps(_mm256_shuffle_epi32(_mm256_castps_si256(sca1),0b1000));
    sca0 = _mm256_castpd_ps(_mm256_permute4x64_pd(_mm256_castps_pd(sca0),0b1000));
    sca1 = _mm256_castpd_ps(_mm256_permute4x64_pd(_mm256_castps_pd(sca1),0b1000));
    sca0 = _mm256_permute2f128_ps(sca0,sca1,0x20);

    // sqrt(A*A + cos(pt1[1] * M_PI / 180.) * cos(pt2[1] * M_PI / 180.) * B*B)
    sca0 = _mm256_sqrt_ps(sca0);
    // asin
    asin256_ps(sca0,sca1);
    //     return 6371000.0 * 2 * asin(sqrt(A*A + cos(pt1[1] * M_PI / 180.) * cos(pt2[1] * M_PI / 180.) * B*B));
    return _mm256_mul_ps(sca1,earthConstant);
}

template <OperatorType oper>uint16_t st_distance_sphere(__m256i x0,__m256i x1, __m256d positionSphereCenter256, __m256 cosPositionSphereOrigin256,__m256 distanceSphere256) {
    __m256 sca0 = cal_distance_sphere(x0,x1,positionSphereCenter256, cosPositionSphereOrigin256);
    if (oper == OperEqualType) {
        sca0 = _mm256_cmp_ps(sca0, distanceSphere256, _CMP_EQ_OQ);
    } else if (oper == OperNotEqualType) {
        sca0 = _mm256_cmp_ps(sca0, distanceSphere256, _CMP_NEQ_OQ);
    } else if (oper == OperLTType) {
     sca0 = _mm256_cmp_ps(sca0,distanceSphere256,_CMP_LT_OQ);
    } else if (oper == OperLEType) {
        sca0 = _mm256_cmp_ps(sca0,distanceSphere256,_CMP_LE_OQ);
    } else if (oper == OperGTType) {
        sca0 = _mm256_cmp_ps(sca0,distanceSphere256,_CMP_GT_OQ);
    } else if (oper == OperGEType) {
        sca0 = _mm256_cmp_ps(sca0, distanceSphere256, _CMP_GE_OQ);
    } else {
        Log::error("Unsupported template operator for distance comparison");
    }
    __m256i ret = _mm256_packs_epi32(_mm256_castps_si256(sca0),_mm256_castps_si256(sca0));
    ret = _mm256_permute4x64_epi64(ret,0x88);
    ret = _mm256_packs_epi16(ret,ret);
    return _mm256_movemask_epi8(ret) & 0xff;
}

template uint16_t st_distance_sphere<OperEqualType>(__m256i x0,__m256i x1,__m256d positionSphereCenter256, __m256 cosPositionSphereOrigin256,__m256 distanceSphere256);
template uint16_t st_distance_sphere<OperNotEqualType>(__m256i x0,__m256i x1,__m256d positionSphereCenter256, __m256 cosPositionSphereOrigin256,__m256 distanceSphere256);
template uint16_t st_distance_sphere<OperLTType>(__m256i x0,__m256i x1,__m256d positionSphereCenter256, __m256 cosPositionSphereOrigin256,__m256 distanceSphere256);
template uint16_t st_distance_sphere<OperGTType>(__m256i x0,__m256i x1,__m256d positionSphereCenter256, __m256 cosPositionSphereOrigin256,__m256 distanceSphere256);
template uint16_t st_distance_sphere<OperLEType>(__m256i x0,__m256i x1,__m256d positionSphereCenter256, __m256 cosPositionSphereOrigin256,__m256 distanceSphere256);
template uint16_t st_distance_sphere<OperGEType>(__m256i x0,__m256i x1,__m256d positionSphereCenter256, __m256 cosPositionSphereOrigin256,__m256 distanceSphere256);


float distance_to_center(int64_t v, int64_t center) {
    float pt0, pt1;
    Uint64ToLocation(v,pt0,pt1);
    float CenterX, CenterY;
    Uint64ToLocation(center,CenterX,CenterY);

    double A = sin(((CenterY - pt1)* M_PI / 180.) / 2.);
    double B = sin(((CenterX - pt0) * M_PI / 180.) / 2);
    double d = 6371000.0 * 2 * asin(sqrt(A*A + cos(pt1 * M_PI / 180.) * cos(CenterY*M_PI/180.0) * B*B));
    return (float)d;
}
