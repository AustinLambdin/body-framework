// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

namespace body::dsp {

/// @brief Band-limited oscillator with PolyBLEP anti-aliasing.
///
/// Supports sine, saw, square, and triangle waveforms. Output range is [-1, 1].
/// Saw, square, and triangle use PolyBLEP to reduce aliasing at high frequencies.
class Oscillator {
public:
    enum class Waveform {
        Sine,
        Saw,
        Square,
        Triangle
    };

    Oscillator() = default;

    /// @brief Set sample rate.
    void prepare(double sampleRate) noexcept;

    /// @brief Set oscillator frequency in Hz.
    void setFrequency(float hz) noexcept;

    /// @brief Set waveform type.
    void setWaveform(Waveform waveform) noexcept;

    /// @brief Generate the next sample.
    /// @return Sample value in [-1, 1].
    [[nodiscard]] float process() noexcept;

    /// @brief Reset phase to zero.
    void reset() noexcept;

    /// @brief Get current frequency.
    [[nodiscard]] float getFrequency() const noexcept { return frequency_; }

    /// @brief Get current waveform.
    [[nodiscard]] Waveform getWaveform() const noexcept { return waveform_; }

private:
    [[nodiscard]] float polyBlep(float t) const noexcept;

    double sampleRate_ = 44100.0;
    Waveform waveform_ = Waveform::Sine;
    float frequency_ = 440.0f;
    float phase_ = 0.0f;
    float phaseIncrement_ = 0.0f;
};

} // namespace body::dsp
