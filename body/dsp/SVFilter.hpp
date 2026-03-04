// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

namespace body::dsp {

/// @brief State-variable filter using trapezoidal integration (Cytomic/Simper).
///
/// Better behaved at high frequencies than biquad. Produces low-pass, high-pass,
/// band-pass, notch, peak, low-shelf, and high-shelf simultaneously.
class SVFilter {
public:
    enum class Type {
        LowPass,
        HighPass,
        BandPass,
        Notch,
        Peak,
        LowShelf,
        HighShelf
    };

    SVFilter() = default;

    /// @brief Set sample rate. Call before setParameters().
    void prepare(double sampleRate) noexcept;

    /// @brief Set all filter parameters at once.
    /// @param type Filter type.
    /// @param frequency Cutoff/center frequency in Hz.
    /// @param q Resonance (Q factor).
    /// @param gainDb Gain in dB (only used for Peak, LowShelf, HighShelf).
    void setParameters(Type type, float frequency, float q, float gainDb = 0.0f) noexcept;

    /// @brief Process a single sample.
    [[nodiscard]] float process(float sample) noexcept;

    /// @brief Reset filter state.
    void reset() noexcept;

private:
    void updateCoefficients() noexcept;

    double sampleRate_ = 44100.0;
    Type type_ = Type::LowPass;
    float frequency_ = 1000.0f;
    float q_ = 0.707f;
    float gainDb_ = 0.0f;

    // Coefficients
    float g_ = 0.0f;   // tan(pi * fc / fs)
    float k_ = 0.0f;   // 1/Q
    float a1_ = 0.0f;
    float a2_ = 0.0f;
    float a3_ = 0.0f;
    float m0_ = 0.0f, m1_ = 0.0f, m2_ = 0.0f;

    // State
    float ic1eq_ = 0.0f;
    float ic2eq_ = 0.0f;
};

} // namespace body::dsp
