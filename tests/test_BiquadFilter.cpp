// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "body/dsp/BiquadFilter.hpp"

#include <array>
#include <cmath>
#include <numbers>

using namespace Catch::Matchers;

TEST_CASE("BiquadFilter default passes signal", "[BiquadFilter]")
{
    body::dsp::BiquadFilter filter;
    filter.prepare(44100.0);

    // Default coefficients are pass-through (b0=1, rest=0)
    REQUIRE_THAT(filter.process(1.0f), WithinAbs(1.0f, 1e-6f));
    REQUIRE_THAT(filter.process(0.5f), WithinAbs(0.5f, 1e-6f));
}

TEST_CASE("BiquadFilter low-pass attenuates high frequency", "[BiquadFilter]")
{
    body::dsp::BiquadFilter filter;
    filter.prepare(44100.0);
    filter.setLowPass(1000.0f, 0.707f);

    // Generate a high-frequency signal (10kHz) and check it's attenuated
    float peakOut = 0.0f;
    for (int i = 0; i < 1000; ++i) {
        float sample = std::sin(2.0f * std::numbers::pi_v<float> * 10000.0f * static_cast<float>(i) / 44100.0f);
        float out = filter.process(sample);
        if (i > 100) // skip transient
            peakOut = std::max(peakOut, std::abs(out));
    }

    // Should be significantly attenuated (10kHz well above 1kHz cutoff)
    REQUIRE(peakOut < 0.15f);
}

TEST_CASE("BiquadFilter low-pass passes low frequency", "[BiquadFilter]")
{
    body::dsp::BiquadFilter filter;
    filter.prepare(44100.0);
    filter.setLowPass(5000.0f, 0.707f);

    // Generate a low-frequency signal (100Hz)
    float peakOut = 0.0f;
    for (int i = 0; i < 2000; ++i) {
        float sample = std::sin(2.0f * std::numbers::pi_v<float> * 100.0f * static_cast<float>(i) / 44100.0f);
        float out = filter.process(sample);
        if (i > 500)
            peakOut = std::max(peakOut, std::abs(out));
    }

    REQUIRE(peakOut > 0.9f);
}

TEST_CASE("BiquadFilter high-pass attenuates low frequency", "[BiquadFilter]")
{
    body::dsp::BiquadFilter filter;
    filter.prepare(44100.0);
    filter.setHighPass(5000.0f, 0.707f);

    float peakOut = 0.0f;
    for (int i = 0; i < 2000; ++i) {
        float sample = std::sin(2.0f * std::numbers::pi_v<float> * 100.0f * static_cast<float>(i) / 44100.0f);
        float out = filter.process(sample);
        if (i > 500)
            peakOut = std::max(peakOut, std::abs(out));
    }

    REQUIRE(peakOut < 0.05f);
}

TEST_CASE("BiquadFilter processBlock matches per-sample", "[BiquadFilter]")
{
    body::dsp::BiquadFilter filter1;
    body::dsp::BiquadFilter filter2;
    filter1.prepare(44100.0);
    filter2.prepare(44100.0);
    filter1.setLowPass(2000.0f, 1.0f);
    filter2.setLowPass(2000.0f, 1.0f);

    std::array<float, 64> block{};
    std::array<float, 64> reference{};
    for (int i = 0; i < 64; ++i) {
        block[i] = std::sin(2.0f * std::numbers::pi_v<float> * 440.0f * static_cast<float>(i) / 44100.0f);
        reference[i] = block[i];
    }

    // Process block
    filter1.processBlock(block.data(), block.size());

    // Process per-sample
    for (int i = 0; i < 64; ++i)
        reference[i] = filter2.process(reference[i]);

    for (int i = 0; i < 64; ++i) {
        REQUIRE_THAT(block[i], WithinAbs(reference[i], 1e-6f));
    }
}

TEST_CASE("BiquadFilter reset clears state", "[BiquadFilter]")
{
    body::dsp::BiquadFilter filter;
    filter.prepare(44100.0);
    filter.setLowPass(1000.0f, 0.707f);

    // Push some signal through
    for (int i = 0; i < 100; ++i)
        (void)filter.process(1.0f);

    filter.reset();

    // After reset, filter state should be clear
    // Processing zero should yield zero
    REQUIRE_THAT(filter.process(0.0f), WithinAbs(0.0f, 1e-6f));
}

TEST_CASE("BiquadFilter notch attenuates center frequency", "[BiquadFilter]")
{
    body::dsp::BiquadFilter filter;
    filter.prepare(44100.0);
    filter.setNotch(1000.0f, 10.0f);

    float peakOut = 0.0f;
    for (int i = 0; i < 4000; ++i) {
        float sample = std::sin(2.0f * std::numbers::pi_v<float> * 1000.0f * static_cast<float>(i) / 44100.0f);
        float out = filter.process(sample);
        if (i > 1000)
            peakOut = std::max(peakOut, std::abs(out));
    }

    REQUIRE(peakOut < 0.05f);
}

TEST_CASE("BiquadFilter peak boosts center frequency", "[BiquadFilter]")
{
    body::dsp::BiquadFilter filter;
    filter.prepare(44100.0);
    filter.setPeak(1000.0f, 1.0f, 12.0f);

    float peakOut = 0.0f;
    for (int i = 0; i < 4000; ++i) {
        float sample = 0.25f * std::sin(2.0f * std::numbers::pi_v<float> * 1000.0f * static_cast<float>(i) / 44100.0f);
        float out = filter.process(sample);
        if (i > 1000)
            peakOut = std::max(peakOut, std::abs(out));
    }

    // 12dB boost should amplify significantly (gain ~ 4x)
    REQUIRE(peakOut > 0.8f);
}
