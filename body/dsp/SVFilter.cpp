// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "body/dsp/SVFilter.hpp"

#include <cmath>
#include <numbers>

namespace body::dsp {

void SVFilter::prepare(double sampleRate) noexcept
{
    sampleRate_ = sampleRate;
    reset();
    updateCoefficients();
}

void SVFilter::setParameters(Type type, float frequency, float q, float gainDb) noexcept
{
    type_ = type;
    frequency_ = frequency;
    q_ = q;
    gainDb_ = gainDb;
    updateCoefficients();
}

float SVFilter::process(float sample) noexcept
{
    // Tick the SVF (trapezoidal integration)
    float v3 = sample - ic2eq_;
    float v1 = a1_ * ic1eq_ + a2_ * v3;
    float v2 = ic2eq_ + a2_ * ic1eq_ + a3_ * v3;
    ic1eq_ = 2.0f * v1 - ic1eq_;
    ic2eq_ = 2.0f * v2 - ic2eq_;

    return m0_ * sample + m1_ * v1 + m2_ * v2;
}

void SVFilter::reset() noexcept
{
    ic1eq_ = 0.0f;
    ic2eq_ = 0.0f;
}

void SVFilter::updateCoefficients() noexcept
{
    g_ = std::tan(std::numbers::pi_v<float> * frequency_ / static_cast<float>(sampleRate_));
    k_ = 1.0f / q_;

    a1_ = 1.0f / (1.0f + g_ * (g_ + k_));
    a2_ = g_ * a1_;
    a3_ = g_ * a2_;

    float A = std::pow(10.0f, gainDb_ / 40.0f);

    switch (type_) {
        case Type::LowPass:
            m0_ = 0.0f; m1_ = 0.0f; m2_ = 1.0f;
            break;
        case Type::HighPass:
            m0_ = 1.0f; m1_ = -k_; m2_ = -1.0f;
            break;
        case Type::BandPass:
            m0_ = 0.0f; m1_ = 1.0f; m2_ = 0.0f;
            break;
        case Type::Notch:
            m0_ = 1.0f; m1_ = -k_; m2_ = 0.0f;
            break;
        case Type::Peak:
            // Peaking EQ: adjust k for gain
            k_ = 1.0f / (q_ * A);
            a1_ = 1.0f / (1.0f + g_ * (g_ + k_));
            a2_ = g_ * a1_;
            a3_ = g_ * a2_;
            m0_ = 1.0f; m1_ = k_ * (A * A - 1.0f); m2_ = 0.0f;
            break;
        case Type::LowShelf: {
            g_ = std::tan(std::numbers::pi_v<float> * frequency_ / static_cast<float>(sampleRate_)) / std::sqrt(A);
            k_ = 1.0f / q_;
            a1_ = 1.0f / (1.0f + g_ * (g_ + k_));
            a2_ = g_ * a1_;
            a3_ = g_ * a2_;
            m0_ = 1.0f; m1_ = k_ * (A - 1.0f); m2_ = A * A - 1.0f;
            break;
        }
        case Type::HighShelf: {
            g_ = std::tan(std::numbers::pi_v<float> * frequency_ / static_cast<float>(sampleRate_)) * std::sqrt(A);
            k_ = 1.0f / q_;
            a1_ = 1.0f / (1.0f + g_ * (g_ + k_));
            a2_ = g_ * a1_;
            a3_ = g_ * a2_;
            m0_ = A * A; m1_ = k_ * (1.0f - A) * A; m2_ = 1.0f - A * A;
            break;
        }
    }
}

} // namespace body::dsp
