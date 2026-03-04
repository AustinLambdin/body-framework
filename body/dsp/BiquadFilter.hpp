// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include <cstddef>

namespace body::dsp {

/// @brief Biquad filter using Direct Form II Transposed.
///
/// Coefficient calculations follow the Audio EQ Cookbook by Robert Bristow-Johnson.
/// Supports LPF, HPF, BPF, Notch, Peak, LowShelf, HighShelf.
class BiquadFilter {
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

    BiquadFilter() = default;

    /// @brief Set the sample rate. Call before setting filter parameters.
    void prepare(double sampleRate) noexcept;

    /// @brief Configure as low-pass filter.
    void setLowPass(float frequency, float q) noexcept;

    /// @brief Configure as high-pass filter.
    void setHighPass(float frequency, float q) noexcept;

    /// @brief Configure as band-pass filter.
    void setBandPass(float frequency, float q) noexcept;

    /// @brief Configure as notch filter.
    void setNotch(float frequency, float q) noexcept;

    /// @brief Configure as peaking EQ filter.
    void setPeak(float frequency, float q, float gainDb) noexcept;

    /// @brief Configure as low-shelf filter.
    void setLowShelf(float frequency, float q, float gainDb) noexcept;

    /// @brief Configure as high-shelf filter.
    void setHighShelf(float frequency, float q, float gainDb) noexcept;

    /// @brief Process a single sample.
    [[nodiscard]] float process(float sample) noexcept;

    /// @brief Process a block of samples in-place.
    void processBlock(float* data, std::size_t count) noexcept;

    /// @brief Reset filter state (zeros delay elements).
    void reset() noexcept;

private:
    void setCoefficients(float b0, float b1, float b2, float a0, float a1, float a2) noexcept;

    double sampleRate_ = 44100.0;

    // Normalized coefficients
    float b0_ = 1.0f, b1_ = 0.0f, b2_ = 0.0f;
    float a1_ = 0.0f, a2_ = 0.0f;

    // State (Direct Form II Transposed)
    float z1_ = 0.0f, z2_ = 0.0f;
};

} // namespace body::dsp
