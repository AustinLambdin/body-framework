// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include <cstddef>

// Platform detection
#if defined(__SSE2__) || (defined(_MSC_VER) && (defined(_M_X64) || defined(_M_AMD64)))
    #define BODY_SIMD_SSE2 1
    #include <immintrin.h>
#elif defined(__ARM_NEON) || defined(__ARM_NEON__)
    #define BODY_SIMD_NEON 1
    #include <arm_neon.h>
#else
    #define BODY_SIMD_SCALAR 1
#endif

namespace body::dsp::simd {

/// @brief Add src to dst element-wise: dst[i] += src[i]
inline void add(float* dst, const float* src, std::size_t count) noexcept
{
    std::size_t i = 0;

#if defined(BODY_SIMD_SSE2)
    for (; i + 4 <= count; i += 4) {
        __m128 a = _mm_loadu_ps(dst + i);
        __m128 b = _mm_loadu_ps(src + i);
        _mm_storeu_ps(dst + i, _mm_add_ps(a, b));
    }
#elif defined(BODY_SIMD_NEON)
    for (; i + 4 <= count; i += 4) {
        float32x4_t a = vld1q_f32(dst + i);
        float32x4_t b = vld1q_f32(src + i);
        vst1q_f32(dst + i, vaddq_f32(a, b));
    }
#endif

    for (; i < count; ++i)
        dst[i] += src[i];
}

/// @brief Multiply dst by src element-wise: dst[i] *= src[i]
inline void multiply(float* dst, const float* src, std::size_t count) noexcept
{
    std::size_t i = 0;

#if defined(BODY_SIMD_SSE2)
    for (; i + 4 <= count; i += 4) {
        __m128 a = _mm_loadu_ps(dst + i);
        __m128 b = _mm_loadu_ps(src + i);
        _mm_storeu_ps(dst + i, _mm_mul_ps(a, b));
    }
#elif defined(BODY_SIMD_NEON)
    for (; i + 4 <= count; i += 4) {
        float32x4_t a = vld1q_f32(dst + i);
        float32x4_t b = vld1q_f32(src + i);
        vst1q_f32(dst + i, vmulq_f32(a, b));
    }
#endif

    for (; i < count; ++i)
        dst[i] *= src[i];
}

/// @brief Apply scalar gain to buffer: dst[i] *= gain
inline void applyGain(float* dst, float gain, std::size_t count) noexcept
{
    std::size_t i = 0;

#if defined(BODY_SIMD_SSE2)
    __m128 g = _mm_set1_ps(gain);
    for (; i + 4 <= count; i += 4) {
        __m128 a = _mm_loadu_ps(dst + i);
        _mm_storeu_ps(dst + i, _mm_mul_ps(a, g));
    }
#elif defined(BODY_SIMD_NEON)
    float32x4_t g = vdupq_n_f32(gain);
    for (; i + 4 <= count; i += 4) {
        float32x4_t a = vld1q_f32(dst + i);
        vst1q_f32(dst + i, vmulq_f32(a, g));
    }
#endif

    for (; i < count; ++i)
        dst[i] *= gain;
}

} // namespace body::dsp::simd
