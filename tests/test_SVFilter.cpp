// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "body/dsp/SVFilter.hpp"

#include <cmath>
#include <numbers>

using namespace Catch::Matchers;

TEST_CASE("SVFilter low-pass attenuates high frequency", "[SVFilter]")
{
    body::dsp::SVFilter filter;
    filter.prepare(44100.0);
    filter.setParameters(body::dsp::SVFilter::Type::LowPass, 1000.0f, 0.707f);

    float peakOut = 0.0f;
    for (int i = 0; i < 1000; ++i) {
        float sample = std::sin(2.0f * std::numbers::pi_v<float> * 10000.0f * static_cast<float>(i) / 44100.0f);
        float out = filter.process(sample);
        if (i > 100)
            peakOut = std::max(peakOut, std::abs(out));
    }

    REQUIRE(peakOut < 0.15f);
}

TEST_CASE("SVFilter low-pass passes low frequency", "[SVFilter]")
{
    body::dsp::SVFilter filter;
    filter.prepare(44100.0);
    filter.setParameters(body::dsp::SVFilter::Type::LowPass, 5000.0f, 0.707f);

    float peakOut = 0.0f;
    for (int i = 0; i < 2000; ++i) {
        float sample = std::sin(2.0f * std::numbers::pi_v<float> * 100.0f * static_cast<float>(i) / 44100.0f);
        float out = filter.process(sample);
        if (i > 500)
            peakOut = std::max(peakOut, std::abs(out));
    }

    REQUIRE(peakOut > 0.9f);
}

TEST_CASE("SVFilter high-pass attenuates low frequency", "[SVFilter]")
{
    body::dsp::SVFilter filter;
    filter.prepare(44100.0);
    filter.setParameters(body::dsp::SVFilter::Type::HighPass, 5000.0f, 0.707f);

    float peakOut = 0.0f;
    for (int i = 0; i < 2000; ++i) {
        float sample = std::sin(2.0f * std::numbers::pi_v<float> * 100.0f * static_cast<float>(i) / 44100.0f);
        float out = filter.process(sample);
        if (i > 500)
            peakOut = std::max(peakOut, std::abs(out));
    }

    REQUIRE(peakOut < 0.05f);
}

TEST_CASE("SVFilter band-pass passes center frequency", "[SVFilter]")
{
    body::dsp::SVFilter filter;
    filter.prepare(44100.0);
    filter.setParameters(body::dsp::SVFilter::Type::BandPass, 1000.0f, 1.0f);

    float peakOut = 0.0f;
    for (int i = 0; i < 4000; ++i) {
        float sample = std::sin(2.0f * std::numbers::pi_v<float> * 1000.0f * static_cast<float>(i) / 44100.0f);
        float out = filter.process(sample);
        if (i > 1000)
            peakOut = std::max(peakOut, std::abs(out));
    }

    REQUIRE(peakOut > 0.5f);
}

TEST_CASE("SVFilter notch attenuates center frequency", "[SVFilter]")
{
    body::dsp::SVFilter filter;
    filter.prepare(44100.0);
    filter.setParameters(body::dsp::SVFilter::Type::Notch, 1000.0f, 10.0f);

    float peakOut = 0.0f;
    for (int i = 0; i < 4000; ++i) {
        float sample = std::sin(2.0f * std::numbers::pi_v<float> * 1000.0f * static_cast<float>(i) / 44100.0f);
        float out = filter.process(sample);
        if (i > 1000)
            peakOut = std::max(peakOut, std::abs(out));
    }

    REQUIRE(peakOut < 0.05f);
}

TEST_CASE("SVFilter reset clears state", "[SVFilter]")
{
    body::dsp::SVFilter filter;
    filter.prepare(44100.0);
    filter.setParameters(body::dsp::SVFilter::Type::LowPass, 1000.0f, 0.707f);

    for (int i = 0; i < 100; ++i)
        (void)filter.process(1.0f);

    filter.reset();
    REQUIRE_THAT(filter.process(0.0f), WithinAbs(0.0f, 1e-6f));
}

TEST_CASE("SVFilter peak boosts center frequency", "[SVFilter]")
{
    body::dsp::SVFilter filter;
    filter.prepare(44100.0);
    filter.setParameters(body::dsp::SVFilter::Type::Peak, 1000.0f, 1.0f, 12.0f);

    float peakOut = 0.0f;
    for (int i = 0; i < 4000; ++i) {
        float sample = 0.25f * std::sin(2.0f * std::numbers::pi_v<float> * 1000.0f * static_cast<float>(i) / 44100.0f);
        float out = filter.process(sample);
        if (i > 1000)
            peakOut = std::max(peakOut, std::abs(out));
    }

    REQUIRE(peakOut > 0.8f);
}
