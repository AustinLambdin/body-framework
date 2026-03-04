// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "body/dsp/DelayLine.hpp"

using namespace Catch::Matchers;

TEST_CASE("DelayLine prepare and initial state", "[DelayLine]")
{
    body::dsp::DelayLine dl;
    dl.prepare(10);

    REQUIRE(dl.getMaxDelay() == 10);
    REQUIRE_THAT(dl.read(), WithinAbs(0.0f, 1e-6f));
}

TEST_CASE("DelayLine push and read at max delay", "[DelayLine]")
{
    body::dsp::DelayLine dl;
    dl.prepare(4);

    // Push 5 samples (fills entire buffer of size 5)
    for (int i = 1; i <= 5; ++i)
        dl.push(static_cast<float>(i));

    // read() returns the oldest sample (delay of 4)
    REQUIRE_THAT(dl.read(), WithinAbs(1.0f, 1e-6f));
}

TEST_CASE("DelayLine readAt integer delay", "[DelayLine]")
{
    body::dsp::DelayLine dl;
    dl.prepare(4);

    dl.push(1.0f);
    dl.push(2.0f);
    dl.push(3.0f);
    dl.push(4.0f);
    dl.push(5.0f);

    // Most recent sample is at delay 0
    REQUIRE_THAT(dl.readAt(0.0f), WithinAbs(5.0f, 1e-6f));
    REQUIRE_THAT(dl.readAt(1.0f), WithinAbs(4.0f, 1e-6f));
    REQUIRE_THAT(dl.readAt(2.0f), WithinAbs(3.0f, 1e-6f));
    REQUIRE_THAT(dl.readAt(3.0f), WithinAbs(2.0f, 1e-6f));
    REQUIRE_THAT(dl.readAt(4.0f), WithinAbs(1.0f, 1e-6f));
}

TEST_CASE("DelayLine readAt fractional delay (linear interp)", "[DelayLine]")
{
    body::dsp::DelayLine dl;
    dl.prepare(4);

    dl.push(0.0f);
    dl.push(0.0f);
    dl.push(0.0f);
    dl.push(0.0f);
    dl.push(10.0f);

    // Delay 0.5 should interpolate between 10.0 (delay 0) and 0.0 (delay 1)
    REQUIRE_THAT(dl.readAt(0.5f), WithinAbs(5.0f, 1e-6f));
}

TEST_CASE("DelayLine reset clears buffer", "[DelayLine]")
{
    body::dsp::DelayLine dl;
    dl.prepare(4);

    dl.push(1.0f);
    dl.push(2.0f);
    dl.reset();

    REQUIRE_THAT(dl.read(), WithinAbs(0.0f, 1e-6f));
    REQUIRE_THAT(dl.readAt(0.0f), WithinAbs(0.0f, 1e-6f));
}

TEST_CASE("DelayLine clamps out-of-range delay", "[DelayLine]")
{
    body::dsp::DelayLine dl;
    dl.prepare(4);

    dl.push(1.0f);
    dl.push(2.0f);
    dl.push(3.0f);
    dl.push(4.0f);
    dl.push(5.0f);

    // Requesting beyond max should clamp
    REQUIRE_THAT(dl.readAt(100.0f), WithinAbs(1.0f, 1e-6f));
    // Negative should clamp to 0
    REQUIRE_THAT(dl.readAt(-1.0f), WithinAbs(5.0f, 1e-6f));
}
