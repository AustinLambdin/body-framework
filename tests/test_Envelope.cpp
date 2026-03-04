// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "body/dsp/Envelope.hpp"

using namespace Catch::Matchers;

TEST_CASE("Envelope starts idle", "[Envelope]")
{
    body::dsp::Envelope env;
    env.prepare(44100.0);
    env.setParameters(0.01f, 0.1f, 0.5f, 0.2f);

    REQUIRE(env.getStage() == body::dsp::Envelope::Stage::Idle);
    REQUIRE(!env.isActive());
    REQUIRE_THAT(env.process(), WithinAbs(0.0f, 1e-6f));
}

TEST_CASE("Envelope noteOn triggers attack", "[Envelope]")
{
    body::dsp::Envelope env;
    env.prepare(44100.0);
    env.setParameters(0.01f, 0.1f, 0.5f, 0.2f);

    env.noteOn();
    REQUIRE(env.getStage() == body::dsp::Envelope::Stage::Attack);
    REQUIRE(env.isActive());

    float val = env.process();
    REQUIRE(val > 0.0f);
}

TEST_CASE("Envelope reaches peak and decays to sustain", "[Envelope]")
{
    body::dsp::Envelope env;
    env.prepare(44100.0);
    env.setParameters(0.001f, 0.01f, 0.5f, 0.2f); // Fast attack/decay

    env.noteOn();

    // Run through attack
    float maxVal = 0.0f;
    for (int i = 0; i < 44100; ++i) {
        float val = env.process();
        maxVal = std::max(maxVal, val);
    }

    REQUIRE_THAT(maxVal, WithinAbs(1.0f, 0.01f));

    // Should be at sustain by now
    REQUIRE(env.getStage() == body::dsp::Envelope::Stage::Sustain);
    REQUIRE_THAT(env.process(), WithinAbs(0.5f, 0.01f));
}

TEST_CASE("Envelope release goes to zero", "[Envelope]")
{
    body::dsp::Envelope env;
    env.prepare(44100.0);
    env.setParameters(0.001f, 0.001f, 0.5f, 0.01f); // Fast everything

    env.noteOn();

    // Run to sustain
    for (int i = 0; i < 4410; ++i)
        (void)env.process();

    env.noteOff();
    REQUIRE(env.getStage() == body::dsp::Envelope::Stage::Release);

    // Run through release
    for (int i = 0; i < 44100; ++i)
        (void)env.process();

    REQUIRE(env.getStage() == body::dsp::Envelope::Stage::Idle);
    REQUIRE_THAT(env.process(), WithinAbs(0.0f, 1e-5f));
}

TEST_CASE("Envelope output stays in [0, 1]", "[Envelope]")
{
    body::dsp::Envelope env;
    env.prepare(44100.0);
    env.setParameters(0.01f, 0.1f, 0.7f, 0.2f);

    env.noteOn();

    for (int i = 0; i < 44100; ++i) {
        float val = env.process();
        REQUIRE(val >= 0.0f);
        REQUIRE(val <= 1.0f);
    }

    env.noteOff();

    for (int i = 0; i < 44100; ++i) {
        float val = env.process();
        REQUIRE(val >= 0.0f);
        REQUIRE(val <= 1.0f);
    }
}

TEST_CASE("Envelope retriggering mid-release", "[Envelope]")
{
    body::dsp::Envelope env;
    env.prepare(44100.0);
    env.setParameters(0.001f, 0.01f, 0.5f, 0.1f);

    env.noteOn();
    for (int i = 0; i < 4410; ++i)
        (void)env.process();

    env.noteOff();
    // Let release run partially
    for (int i = 0; i < 1000; ++i)
        (void)env.process();

    float preRetrigger = env.process();
    REQUIRE(preRetrigger > 0.0f);
    REQUIRE(preRetrigger < 0.5f);

    // Retrigger
    env.noteOn();
    REQUIRE(env.getStage() == body::dsp::Envelope::Stage::Attack);

    // Should ramp back up to peak
    float peakVal = 0.0f;
    for (int i = 0; i < 4410; ++i) {
        float v = env.process();
        peakVal = std::max(peakVal, v);
    }

    REQUIRE(peakVal > 0.9f);
}

TEST_CASE("Envelope reset returns to idle", "[Envelope]")
{
    body::dsp::Envelope env;
    env.prepare(44100.0);
    env.setParameters(0.01f, 0.1f, 0.5f, 0.2f);

    env.noteOn();
    for (int i = 0; i < 100; ++i)
        (void)env.process();

    env.reset();
    REQUIRE(env.getStage() == body::dsp::Envelope::Stage::Idle);
    REQUIRE_THAT(env.process(), WithinAbs(0.0f, 1e-6f));
}

TEST_CASE("Envelope zero attack time reaches peak immediately", "[Envelope]")
{
    body::dsp::Envelope env;
    env.prepare(44100.0);
    env.setParameters(0.0f, 0.01f, 0.5f, 0.2f);

    env.noteOn();
    float val = env.process();

    // With zero attack, should reach 1.0 in first sample
    REQUIRE_THAT(val, WithinAbs(1.0f, 0.01f));
}
