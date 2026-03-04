// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

namespace body::dsp {

/// @brief ADSR envelope generator with exponential curves.
///
/// Uses one-pole targeting for smooth exponential attack/decay/release curves.
/// Supports retriggering mid-envelope. Output range is [0, 1].
class Envelope {
public:
    enum class Stage {
        Idle,
        Attack,
        Decay,
        Sustain,
        Release
    };

    Envelope() = default;

    /// @brief Set the sample rate.
    void prepare(double sampleRate) noexcept;

    /// @brief Set ADSR parameters.
    /// @param attackSeconds Attack time in seconds.
    /// @param decaySeconds Decay time in seconds.
    /// @param sustainLevel Sustain level [0, 1].
    /// @param releaseSeconds Release time in seconds.
    void setParameters(float attackSeconds, float decaySeconds,
                       float sustainLevel, float releaseSeconds) noexcept;

    /// @brief Trigger the envelope (start attack phase).
    void noteOn() noexcept;

    /// @brief Release the envelope (start release phase).
    void noteOff() noexcept;

    /// @brief Generate the next envelope sample.
    /// @return Envelope value in [0, 1].
    [[nodiscard]] float process() noexcept;

    /// @brief Reset envelope to idle.
    void reset() noexcept;

    /// @brief Get current stage.
    [[nodiscard]] Stage getStage() const noexcept { return stage_; }

    /// @brief Check if envelope is active (not idle).
    [[nodiscard]] bool isActive() const noexcept { return stage_ != Stage::Idle; }

private:
    [[nodiscard]] float calculateCoefficient(float timeSeconds) const noexcept;

    double sampleRate_ = 44100.0;

    float attackCoeff_ = 0.0f;
    float decayCoeff_ = 0.0f;
    float releaseCoeff_ = 0.0f;
    float sustainLevel_ = 1.0f;

    float output_ = 0.0f;
    Stage stage_ = Stage::Idle;
};

} // namespace body::dsp
