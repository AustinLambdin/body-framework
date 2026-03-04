// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "body/dsp/BiquadFilter.hpp"

#include <cmath>
#include <numbers>

namespace body::dsp {

void BiquadFilter::prepare(double sampleRate) noexcept
{
    sampleRate_ = sampleRate;
    reset();
}

void BiquadFilter::setLowPass(float frequency, float q) noexcept
{
    float w0 = 2.0f * std::numbers::pi_v<float> * frequency / static_cast<float>(sampleRate_);
    float cosW0 = std::cos(w0);
    float sinW0 = std::sin(w0);
    float alpha = sinW0 / (2.0f * q);

    float b0 = (1.0f - cosW0) / 2.0f;
    float b1 = 1.0f - cosW0;
    float b2 = (1.0f - cosW0) / 2.0f;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cosW0;
    float a2 = 1.0f - alpha;

    setCoefficients(b0, b1, b2, a0, a1, a2);
}

void BiquadFilter::setHighPass(float frequency, float q) noexcept
{
    float w0 = 2.0f * std::numbers::pi_v<float> * frequency / static_cast<float>(sampleRate_);
    float cosW0 = std::cos(w0);
    float sinW0 = std::sin(w0);
    float alpha = sinW0 / (2.0f * q);

    float b0 = (1.0f + cosW0) / 2.0f;
    float b1 = -(1.0f + cosW0);
    float b2 = (1.0f + cosW0) / 2.0f;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cosW0;
    float a2 = 1.0f - alpha;

    setCoefficients(b0, b1, b2, a0, a1, a2);
}

void BiquadFilter::setBandPass(float frequency, float q) noexcept
{
    float w0 = 2.0f * std::numbers::pi_v<float> * frequency / static_cast<float>(sampleRate_);
    float cosW0 = std::cos(w0);
    float sinW0 = std::sin(w0);
    float alpha = sinW0 / (2.0f * q);

    float b0 = alpha;
    float b1 = 0.0f;
    float b2 = -alpha;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cosW0;
    float a2 = 1.0f - alpha;

    setCoefficients(b0, b1, b2, a0, a1, a2);
}

void BiquadFilter::setNotch(float frequency, float q) noexcept
{
    float w0 = 2.0f * std::numbers::pi_v<float> * frequency / static_cast<float>(sampleRate_);
    float cosW0 = std::cos(w0);
    float sinW0 = std::sin(w0);
    float alpha = sinW0 / (2.0f * q);

    float b0 = 1.0f;
    float b1 = -2.0f * cosW0;
    float b2 = 1.0f;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cosW0;
    float a2 = 1.0f - alpha;

    setCoefficients(b0, b1, b2, a0, a1, a2);
}

void BiquadFilter::setPeak(float frequency, float q, float gainDb) noexcept
{
    float A = std::pow(10.0f, gainDb / 40.0f);
    float w0 = 2.0f * std::numbers::pi_v<float> * frequency / static_cast<float>(sampleRate_);
    float cosW0 = std::cos(w0);
    float sinW0 = std::sin(w0);
    float alpha = sinW0 / (2.0f * q);

    float b0 = 1.0f + alpha * A;
    float b1 = -2.0f * cosW0;
    float b2 = 1.0f - alpha * A;
    float a0 = 1.0f + alpha / A;
    float a1 = -2.0f * cosW0;
    float a2 = 1.0f - alpha / A;

    setCoefficients(b0, b1, b2, a0, a1, a2);
}

void BiquadFilter::setLowShelf(float frequency, float q, float gainDb) noexcept
{
    float A = std::pow(10.0f, gainDb / 40.0f);
    float w0 = 2.0f * std::numbers::pi_v<float> * frequency / static_cast<float>(sampleRate_);
    float cosW0 = std::cos(w0);
    float sinW0 = std::sin(w0);
    float alpha = sinW0 / (2.0f * q);
    float twoSqrtAAlpha = 2.0f * std::sqrt(A) * alpha;

    float b0 = A * ((A + 1.0f) - (A - 1.0f) * cosW0 + twoSqrtAAlpha);
    float b1 = 2.0f * A * ((A - 1.0f) - (A + 1.0f) * cosW0);
    float b2 = A * ((A + 1.0f) - (A - 1.0f) * cosW0 - twoSqrtAAlpha);
    float a0 = (A + 1.0f) + (A - 1.0f) * cosW0 + twoSqrtAAlpha;
    float a1 = -2.0f * ((A - 1.0f) + (A + 1.0f) * cosW0);
    float a2 = (A + 1.0f) + (A - 1.0f) * cosW0 - twoSqrtAAlpha;

    setCoefficients(b0, b1, b2, a0, a1, a2);
}

void BiquadFilter::setHighShelf(float frequency, float q, float gainDb) noexcept
{
    float A = std::pow(10.0f, gainDb / 40.0f);
    float w0 = 2.0f * std::numbers::pi_v<float> * frequency / static_cast<float>(sampleRate_);
    float cosW0 = std::cos(w0);
    float sinW0 = std::sin(w0);
    float alpha = sinW0 / (2.0f * q);
    float twoSqrtAAlpha = 2.0f * std::sqrt(A) * alpha;

    float b0 = A * ((A + 1.0f) + (A - 1.0f) * cosW0 + twoSqrtAAlpha);
    float b1 = -2.0f * A * ((A - 1.0f) + (A + 1.0f) * cosW0);
    float b2 = A * ((A + 1.0f) + (A - 1.0f) * cosW0 - twoSqrtAAlpha);
    float a0 = (A + 1.0f) - (A - 1.0f) * cosW0 + twoSqrtAAlpha;
    float a1 = 2.0f * ((A - 1.0f) - (A + 1.0f) * cosW0);
    float a2 = (A + 1.0f) - (A - 1.0f) * cosW0 - twoSqrtAAlpha;

    setCoefficients(b0, b1, b2, a0, a1, a2);
}

float BiquadFilter::process(float sample) noexcept
{
    float out = b0_ * sample + z1_;
    z1_ = b1_ * sample - a1_ * out + z2_;
    z2_ = b2_ * sample - a2_ * out;
    return out;
}

void BiquadFilter::processBlock(float* data, std::size_t count) noexcept
{
    for (std::size_t i = 0; i < count; ++i)
        data[i] = process(data[i]);
}

void BiquadFilter::reset() noexcept
{
    z1_ = 0.0f;
    z2_ = 0.0f;
}

void BiquadFilter::setCoefficients(float b0, float b1, float b2, float a0, float a1, float a2) noexcept
{
    float invA0 = 1.0f / a0;
    b0_ = b0 * invA0;
    b1_ = b1 * invA0;
    b2_ = b2 * invA0;
    a1_ = a1 * invA0;
    a2_ = a2 * invA0;
}

} // namespace body::dsp
