// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "body/dsp/FFT.hpp"

#include <array>
#include <cmath>
#include <numbers>

using namespace Catch::Matchers;

TEST_CASE("FFT construction with valid size", "[FFT]")
{
    body::dsp::FFT fft(64);
    REQUIRE(fft.getSize() == 64);
}

TEST_CASE("FFT construction with power-of-2 size succeeds", "[FFT]")
{
    body::dsp::FFT fft(128);
    REQUIRE(fft.getSize() == 128);
}

TEST_CASE("FFT move construction", "[FFT]")
{
    body::dsp::FFT fft1(64);
    body::dsp::FFT fft2(std::move(fft1));

    REQUIRE(fft2.getSize() == 64);
    REQUIRE(fft1.getSize() == 0); // NOLINT(bugprone-use-after-move)
}

TEST_CASE("FFT forward then inverse recovers signal", "[FFT]")
{
    constexpr int N = 64;
    body::dsp::FFT fft(N);

    // Create a simple signal: DC + sine
    std::array<float, N> input{};
    for (int i = 0; i < N; ++i) {
        input[i] = 1.0f + std::sin(2.0f * std::numbers::pi_v<float> * 4.0f * static_cast<float>(i) / static_cast<float>(N));
    }

    std::array<float, N> freq{};
    std::array<float, N> output{};

    fft.forward(input.data(), freq.data());
    fft.inverse(freq.data(), output.data());

    // inverse(forward(x)) = N * x, so divide by N
    for (int i = 0; i < N; ++i) {
        REQUIRE_THAT(output[i] / static_cast<float>(N), WithinAbs(input[i], 1e-4f));
    }
}

TEST_CASE("FFT DC signal has energy at bin 0", "[FFT]")
{
    constexpr int N = 64;
    body::dsp::FFT fft(N);

    std::array<float, N> input{};
    for (auto& s : input)
        s = 1.0f;

    std::array<float, N> freq{};
    fft.forward(input.data(), freq.data());

    // For a real FFT with ordered output, bin 0 is the DC component
    REQUIRE_THAT(freq[0], WithinAbs(static_cast<float>(N), 0.1f));

    // Other bins should be near zero
    for (int i = 2; i < N; ++i) {
        REQUIRE_THAT(freq[i], WithinAbs(0.0f, 1e-4f));
    }
}

TEST_CASE("FFT sine signal has energy at correct bin", "[FFT]")
{
    constexpr int N = 256;
    body::dsp::FFT fft(N);

    // Sine at bin 8 (8 cycles in N samples)
    std::array<float, N> input{};
    for (int i = 0; i < N; ++i) {
        input[i] = std::sin(2.0f * std::numbers::pi_v<float> * 8.0f * static_cast<float>(i) / static_cast<float>(N));
    }

    std::array<float, N> freq{};
    fft.forward(input.data(), freq.data());

    // For ordered real FFT output, interleaved as [re0, re_N/2, re1, im1, re2, im2, ...]
    // Bin 8 should have the dominant energy
    // The pffft ordered format for real: freq[0] = DC, freq[1] = Nyquist
    // Then freq[2*k], freq[2*k+1] = re, im of bin k for k=1..N/2-1
    float binMag = std::sqrt(freq[16] * freq[16] + freq[17] * freq[17]);
    REQUIRE(binMag > static_cast<float>(N) * 0.4f);
}

TEST_CASE("FFT various power-of-2 sizes", "[FFT]")
{
    for (int size : {32, 64, 128, 256, 512, 1024}) {
        body::dsp::FFT fft(size);
        REQUIRE(fft.getSize() == size);
    }
}
