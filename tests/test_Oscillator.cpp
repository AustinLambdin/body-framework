// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "body/dsp/Oscillator.hpp"

using namespace Catch::Matchers;

TEST_CASE("Oscillator default state", "[Oscillator]")
{
    body::dsp::Oscillator osc;
    osc.prepare(44100.0);

    REQUIRE_THAT(osc.getFrequency(), WithinAbs(440.0f, 1e-6f));
    REQUIRE(osc.getWaveform() == body::dsp::Oscillator::Waveform::Sine);
}

TEST_CASE("Oscillator sine output in [-1, 1]", "[Oscillator]")
{
    body::dsp::Oscillator osc;
    osc.prepare(44100.0);
    osc.setFrequency(440.0f);
    osc.setWaveform(body::dsp::Oscillator::Waveform::Sine);

    for (int i = 0; i < 44100; ++i) {
        float val = osc.process();
        REQUIRE(val >= -1.0f);
        REQUIRE(val <= 1.0f);
    }
}

TEST_CASE("Oscillator sine starts at zero", "[Oscillator]")
{
    body::dsp::Oscillator osc;
    osc.prepare(44100.0);
    osc.setFrequency(440.0f);
    osc.setWaveform(body::dsp::Oscillator::Waveform::Sine);

    float first = osc.process();
    REQUIRE_THAT(first, WithinAbs(0.0f, 0.1f));
}

TEST_CASE("Oscillator sine correct frequency", "[Oscillator]")
{
    body::dsp::Oscillator osc;
    osc.prepare(44100.0);
    osc.setFrequency(100.0f);
    osc.setWaveform(body::dsp::Oscillator::Waveform::Sine);

    // Count zero crossings over 1 second
    int zeroCrossings = 0;
    float prev = osc.process();
    for (int i = 1; i < 44100; ++i) {
        float val = osc.process();
        if ((prev >= 0.0f && val < 0.0f) || (prev < 0.0f && val >= 0.0f))
            ++zeroCrossings;
        prev = val;
    }

    // 100Hz = 200 zero crossings per second
    REQUIRE(zeroCrossings >= 198);
    REQUIRE(zeroCrossings <= 202);
}

TEST_CASE("Oscillator saw output in [-1, 1]", "[Oscillator]")
{
    body::dsp::Oscillator osc;
    osc.prepare(44100.0);
    osc.setFrequency(440.0f);
    osc.setWaveform(body::dsp::Oscillator::Waveform::Saw);

    for (int i = 0; i < 44100; ++i) {
        float val = osc.process();
        REQUIRE(val >= -1.1f); // Small tolerance for PolyBLEP
        REQUIRE(val <= 1.1f);
    }
}

TEST_CASE("Oscillator square output near +/-1", "[Oscillator]")
{
    body::dsp::Oscillator osc;
    osc.prepare(44100.0);
    osc.setFrequency(100.0f);
    osc.setWaveform(body::dsp::Oscillator::Waveform::Square);

    int highCount = 0;
    int lowCount = 0;
    for (int i = 0; i < 44100; ++i) {
        float val = osc.process();
        if (val > 0.5f) ++highCount;
        if (val < -0.5f) ++lowCount;
    }

    // Should spend roughly equal time high and low
    REQUIRE(highCount > 20000);
    REQUIRE(lowCount > 20000);
}

TEST_CASE("Oscillator triangle output in [-1, 1]", "[Oscillator]")
{
    body::dsp::Oscillator osc;
    osc.prepare(44100.0);
    osc.setFrequency(440.0f);
    osc.setWaveform(body::dsp::Oscillator::Waveform::Triangle);

    for (int i = 0; i < 44100; ++i) {
        float val = osc.process();
        REQUIRE(val >= -1.01f);
        REQUIRE(val <= 1.01f);
    }
}

TEST_CASE("Oscillator reset restores phase", "[Oscillator]")
{
    body::dsp::Oscillator osc;
    osc.prepare(44100.0);
    osc.setFrequency(440.0f);
    osc.setWaveform(body::dsp::Oscillator::Waveform::Sine);

    float first = osc.process();

    for (int i = 0; i < 1000; ++i)
        (void)osc.process();

    osc.reset();
    float afterReset = osc.process();

    REQUIRE_THAT(afterReset, WithinAbs(first, 1e-6f));
}

TEST_CASE("Oscillator setFrequency updates correctly", "[Oscillator]")
{
    body::dsp::Oscillator osc;
    osc.prepare(44100.0);
    osc.setFrequency(1000.0f);
    osc.setWaveform(body::dsp::Oscillator::Waveform::Sine);

    REQUIRE_THAT(osc.getFrequency(), WithinAbs(1000.0f, 1e-6f));

    // Count zero crossings for 1000Hz over 0.1 seconds
    int zeroCrossings = 0;
    float prev = osc.process();
    for (int i = 1; i < 4410; ++i) {
        float val = osc.process();
        if ((prev >= 0.0f && val < 0.0f) || (prev < 0.0f && val >= 0.0f))
            ++zeroCrossings;
        prev = val;
    }

    // 1000Hz for 0.1s = 200 zero crossings
    REQUIRE(zeroCrossings >= 198);
    REQUIRE(zeroCrossings <= 202);
}
