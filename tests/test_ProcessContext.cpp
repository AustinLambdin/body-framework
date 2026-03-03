// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "body/core/ProcessContext.hpp"

using namespace Catch::Matchers;

TEST_CASE("ProcessContext default construction", "[ProcessContext]") {
    body::ProcessContext ctx;

    REQUIRE(ctx.audioIn.getNumChannels() == 0);
    REQUIRE(ctx.audioIn.getNumSamples() == 0);
    REQUIRE(ctx.audioOut.getNumChannels() == 0);
    REQUIRE(ctx.audioOut.getNumSamples() == 0);
    REQUIRE(ctx.midiIn.isEmpty());
    REQUIRE(ctx.midiOut.isEmpty());
}

TEST_CASE("ProcessContext default values", "[ProcessContext]") {
    body::ProcessContext ctx;

    REQUIRE_THAT(ctx.sampleRate, WithinAbs(44100.0, 1e-6));
    REQUIRE(ctx.blockSize == 0);
    REQUIRE(ctx.isPlaying == false);
    REQUIRE(ctx.isRecording == false);
    REQUIRE_THAT(ctx.bpm, WithinAbs(120.0, 1e-6));
    REQUIRE_THAT(ctx.ppqPosition, WithinAbs(0.0, 1e-6));
    REQUIRE(ctx.samplePosition == 0);
    REQUIRE(ctx.isRealtime == true);
}

TEST_CASE("ProcessContext with audio buffers", "[ProcessContext]") {
    body::ProcessContext ctx;
    ctx.audioIn = body::AudioBuffer<float>(2, 512);
    ctx.audioOut = body::AudioBuffer<float>(2, 512);
    ctx.sampleRate = 48000.0;
    ctx.blockSize = 512;

    REQUIRE(ctx.audioIn.getNumChannels() == 2);
    REQUIRE(ctx.audioOut.getNumSamples() == 512);
    REQUIRE_THAT(ctx.sampleRate, WithinAbs(48000.0, 1e-6));
}
