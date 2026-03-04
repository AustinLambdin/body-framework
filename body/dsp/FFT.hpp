// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include <cstddef>
#include <memory>

namespace body::dsp {

/// @brief RAII FFT wrapper using pffft backend.
///
/// Move-only. Performs real-valued FFTs of power-of-2 sizes.
/// Not scaled: inverse(forward(x)) = N * x.
class FFT {
public:
    /// @brief Construct an FFT for the given size (must be a power of 2, >= 32).
    explicit FFT(int size);
    ~FFT();

    FFT(FFT&& other) noexcept;
    FFT& operator=(FFT&& other) noexcept;

    FFT(const FFT&) = delete;
    FFT& operator=(const FFT&) = delete;

    /// @brief Perform forward FFT (time domain -> frequency domain).
    /// @param input Time-domain samples (size floats).
    /// @param output Frequency-domain data (size floats, interleaved complex).
    void forward(const float* input, float* output) const;

    /// @brief Perform inverse FFT (frequency domain -> time domain).
    /// @param input Frequency-domain data (size floats).
    /// @param output Time-domain samples (size floats). NOT scaled by 1/N.
    void inverse(const float* input, float* output) const;

    /// @brief Get the FFT size.
    [[nodiscard]] int getSize() const noexcept { return size_; }

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    int size_ = 0;
};

} // namespace body::dsp
