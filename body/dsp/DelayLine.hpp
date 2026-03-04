// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include <cstddef>
#include <vector>

namespace body::dsp {

/// @brief Circular-buffer delay line with fractional delay via linear interpolation.
///
/// Call prepare() once to allocate, then push()/read()/readAt() are real-time safe.
class DelayLine {
public:
    DelayLine() = default;

    /// @brief Allocate internal buffer for up to maxDelaySamples of delay.
    void prepare(std::size_t maxDelaySamples);

    /// @brief Push a new sample into the delay line.
    void push(float sample) noexcept;

    /// @brief Read the oldest sample (at maximum delay).
    [[nodiscard]] float read() const noexcept;

    /// @brief Read at a fractional delay position using linear interpolation.
    /// @param delaySamples Delay in samples (clamped to [0, maxDelay]).
    [[nodiscard]] float readAt(float delaySamples) const noexcept;

    /// @brief Reset the delay line to silence.
    void reset() noexcept;

    /// @brief Get the maximum delay in samples.
    [[nodiscard]] std::size_t getMaxDelay() const noexcept { return maxDelay_; }

private:
    std::vector<float> buffer_;
    std::size_t writePos_ = 0;
    std::size_t maxDelay_ = 0;
};

} // namespace body::dsp
