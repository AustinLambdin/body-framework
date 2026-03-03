// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "body/core/AudioBuffer.hpp"

using namespace Catch::Matchers;

TEST_CASE("AudioBuffer owning construction", "[AudioBuffer]") {
    body::AudioBuffer<float> buf(2, 512);

    REQUIRE(buf.getNumChannels() == 2);
    REQUIRE(buf.getNumSamples() == 512);
}

TEST_CASE("AudioBuffer default construction", "[AudioBuffer]") {
    body::AudioBuffer<float> buf;

    REQUIRE(buf.getNumChannels() == 0);
    REQUIRE(buf.getNumSamples() == 0);
}

TEST_CASE("AudioBuffer channel access", "[AudioBuffer]") {
    body::AudioBuffer<float> buf(2, 128);

    auto ch0 = buf.getChannel(0);
    auto ch1 = buf.getChannel(1);

    REQUIRE(ch0.size() == 128);
    REQUIRE(ch1.size() == 128);

    // Write and read back
    ch0[0] = 1.0f;
    ch1[0] = 2.0f;

    REQUIRE(buf.getChannelPointer(0)[0] == 1.0f);
    REQUIRE(buf.getChannelPointer(1)[0] == 2.0f);
}

TEST_CASE("AudioBuffer clear", "[AudioBuffer]") {
    body::AudioBuffer<float> buf(2, 64);

    // Fill with data
    for (int ch = 0; ch < 2; ++ch) {
        auto span = buf.getChannel(ch);
        for (auto& s : span) s = 1.0f;
    }

    buf.clear();

    for (int ch = 0; ch < 2; ++ch) {
        auto span = buf.getChannel(ch);
        for (const auto& s : span) {
            REQUIRE(s == 0.0f);
        }
    }
}

TEST_CASE("AudioBuffer partial clear", "[AudioBuffer]") {
    body::AudioBuffer<float> buf(1, 10);

    auto span = buf.getChannel(0);
    for (auto& s : span) s = 1.0f;

    buf.clear(0, 2, 5);

    REQUIRE(span[0] == 1.0f);
    REQUIRE(span[1] == 1.0f);
    REQUIRE(span[2] == 0.0f);
    REQUIRE(span[6] == 0.0f);
    REQUIRE(span[7] == 1.0f);
}

TEST_CASE("AudioBuffer applyGain", "[AudioBuffer]") {
    body::AudioBuffer<float> buf(2, 4);

    for (int ch = 0; ch < 2; ++ch) {
        auto span = buf.getChannel(ch);
        for (auto& s : span) s = 1.0f;
    }

    buf.applyGain(0.5f);

    for (int ch = 0; ch < 2; ++ch) {
        auto span = buf.getChannel(ch);
        for (const auto& s : span) {
            REQUIRE_THAT(s, WithinAbs(0.5f, 1e-6f));
        }
    }
}

TEST_CASE("AudioBuffer applyGain partial", "[AudioBuffer]") {
    body::AudioBuffer<float> buf(1, 4);
    auto span = buf.getChannel(0);
    for (auto& s : span) s = 2.0f;

    buf.applyGain(0, 1, 2, 0.5f);

    REQUIRE(span[0] == 2.0f);
    REQUIRE_THAT(span[1], WithinAbs(1.0f, 1e-6f));
    REQUIRE_THAT(span[2], WithinAbs(1.0f, 1e-6f));
    REQUIRE(span[3] == 2.0f);
}

TEST_CASE("AudioBuffer non-owning view", "[AudioBuffer]") {
    float ch0Data[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    float ch1Data[4] = {5.0f, 6.0f, 7.0f, 8.0f};
    float* channels[2] = {ch0Data, ch1Data};

    body::AudioBuffer<float> buf(channels, 2, 4);

    REQUIRE(buf.getNumChannels() == 2);
    REQUIRE(buf.getNumSamples() == 4);
    REQUIRE(buf.getChannelPointer(0) == ch0Data);
    REQUIRE(buf.getChannelPointer(1) == ch1Data);

    // Modifying the buffer modifies the original data
    buf.applyGain(2.0f);
    REQUIRE(ch0Data[0] == 2.0f);
    REQUIRE(ch1Data[3] == 16.0f);
}

TEST_CASE("AudioBuffer addFrom", "[AudioBuffer]") {
    body::AudioBuffer<float> dst(2, 4);
    body::AudioBuffer<float> src(2, 4);

    for (int ch = 0; ch < 2; ++ch) {
        auto span = src.getChannel(ch);
        for (auto& s : span) s = 1.0f;
    }
    dst.clear();

    dst.addFrom(src, 0.5f);

    for (int ch = 0; ch < 2; ++ch) {
        auto span = dst.getChannel(ch);
        for (const auto& s : span) {
            REQUIRE_THAT(s, WithinAbs(0.5f, 1e-6f));
        }
    }
}
