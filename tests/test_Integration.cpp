// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "body/core/AudioProcessor.hpp"
#include "body/core/StateSerializer.hpp"
#include <cmath>

// --- Inline test plugin (no dependency on example code) ---

class TestPlugin : public body::AudioProcessor {
public:
    TestPlugin() {
        gainParam_ = &parameters_.addParameter<body::FloatParameter>(
            body::ParameterID("gain"), "Gain", -60.0f, 24.0f, 0.0f, "dB");
        bypassParam_ = &parameters_.addParameter<body::BoolParameter>(
            body::ParameterID("bypass"), "Bypass", false);
    }

    [[nodiscard]] std::string getName() const override { return "TestPlugin"; }
    [[nodiscard]] std::string getVendor() const override { return "BODY Tests"; }
    [[nodiscard]] std::string getVersion() const override { return "1.0.0"; }

    void prepare(double sampleRate, int /*maxBlockSize*/) override {
        prepareCount_++;
        sampleRate_ = sampleRate;
    }

    void processBlock(body::ProcessContext& ctx) override {
        processCount_++;
        if (bypassParam_->getValue()) return;

        float gainDb = gainParam_->getDenormalized();
        float gain = std::pow(10.0f, gainDb / 20.0f);
        for (int ch = 0; ch < ctx.audioOut.getNumChannels(); ++ch) {
            for (auto& sample : ctx.audioOut.getChannel(ch))
                sample *= gain;
        }
    }

    void reset() override { resetCount_++; }

    int prepareCount_ = 0;
    int processCount_ = 0;
    int resetCount_ = 0;

    body::FloatParameter* gainParam_ = nullptr;
    body::BoolParameter* bypassParam_ = nullptr;

private:
    double sampleRate_ = 0.0;
};

// --- Tests ---

TEST_CASE("Integration: Plugin lifecycle", "[integration]") {
    TestPlugin plugin;
    REQUIRE(plugin.prepareCount_ == 0);
    REQUIRE(plugin.processCount_ == 0);
    REQUIRE(plugin.resetCount_ == 0);

    // Prepare
    plugin.prepare(44100.0, 512);
    REQUIRE(plugin.prepareCount_ == 1);

    // Process a block
    {
        body::AudioBuffer<float> buffer(2, 256);
        body::ProcessContext ctx;
        ctx.audioOut = std::move(buffer);
        ctx.blockSize = 256;
        ctx.sampleRate = 44100.0;
        plugin.processBlock(ctx);
        REQUIRE(plugin.processCount_ == 1);
    }

    // Reset
    plugin.reset();
    REQUIRE(plugin.resetCount_ == 1);

    // Process again after reset
    {
        body::AudioBuffer<float> buffer(2, 128);
        body::ProcessContext ctx;
        ctx.audioOut = std::move(buffer);
        ctx.blockSize = 128;
        ctx.sampleRate = 44100.0;
        plugin.processBlock(ctx);
        REQUIRE(plugin.processCount_ == 2);
    }
}

TEST_CASE("Integration: Multi-instance independence", "[integration]") {
    TestPlugin pluginA;
    TestPlugin pluginB;

    pluginA.prepare(44100.0, 512);
    pluginB.prepare(44100.0, 512);

    // Set different gains: A = 0 dB (unity), B = -60 dB (near silence)
    pluginA.gainParam_->setDenormalized(0.0f);
    pluginB.gainParam_->setDenormalized(-60.0f);

    constexpr int numSamples = 64;

    // Process plugin A
    body::AudioBuffer<float> bufA(2, numSamples);
    for (int ch = 0; ch < 2; ++ch)
        for (int i = 0; i < numSamples; ++i)
            bufA.getChannel(ch)[i] = 1.0f;
    body::ProcessContext ctxA;
    ctxA.audioOut = std::move(bufA);
    ctxA.blockSize = numSamples;
    pluginA.processBlock(ctxA);

    // Process plugin B
    body::AudioBuffer<float> bufB(2, numSamples);
    for (int ch = 0; ch < 2; ++ch)
        for (int i = 0; i < numSamples; ++i)
            bufB.getChannel(ch)[i] = 1.0f;
    body::ProcessContext ctxB;
    ctxB.audioOut = std::move(bufB);
    ctxB.blockSize = numSamples;
    pluginB.processBlock(ctxB);

    // A at 0 dB should be unity
    REQUIRE_THAT(ctxA.audioOut.getChannel(0)[0],
                 Catch::Matchers::WithinAbs(1.0f, 0.001f));

    // B at -60 dB should be near silence
    REQUIRE(std::abs(ctxB.audioOut.getChannel(0)[0]) < 0.01f);
}

TEST_CASE("Integration: State round-trip", "[integration]") {
    TestPlugin plugin;
    plugin.prepare(44100.0, 512);

    // Set custom values
    plugin.gainParam_->setDenormalized(6.0f);
    plugin.bypassParam_->setDenormalized(1.0f);

    // Serialize
    auto state = body::StateSerializer::serialize(plugin.getParameters());
    REQUIRE(!state.empty());

    // Reset params to defaults
    plugin.gainParam_->setDenormalized(0.0f);
    plugin.bypassParam_->setDenormalized(0.0f);
    REQUIRE_THAT(plugin.gainParam_->getDenormalized(),
                 Catch::Matchers::WithinAbs(0.0f, 0.01f));

    // Deserialize
    bool ok = body::StateSerializer::deserialize(
        plugin.getParameters(), state.data(), state.size());
    REQUIRE(ok);

    // Verify restored values
    REQUIRE_THAT(plugin.gainParam_->getDenormalized(),
                 Catch::Matchers::WithinAbs(6.0f, 0.01f));
    REQUIRE(plugin.bypassParam_->getValue() == true);
}

TEST_CASE("Integration: Audio flow — unity and silence", "[integration]") {
    TestPlugin plugin;
    plugin.prepare(44100.0, 512);

    constexpr int numSamples = 128;
    constexpr float testValue = 0.5f;

    SECTION("0 dB gain = unity") {
        plugin.gainParam_->setDenormalized(0.0f);

        body::AudioBuffer<float> buffer(2, numSamples);
        for (int ch = 0; ch < 2; ++ch)
            for (int i = 0; i < numSamples; ++i)
                buffer.getChannel(ch)[i] = testValue;

        body::ProcessContext ctx;
        ctx.audioOut = std::move(buffer);
        ctx.blockSize = numSamples;
        plugin.processBlock(ctx);

        for (int i = 0; i < numSamples; ++i)
            REQUIRE_THAT(ctx.audioOut.getChannel(0)[i],
                         Catch::Matchers::WithinAbs(testValue, 0.001f));
    }

    SECTION("-60 dB gain = near silence") {
        plugin.gainParam_->setDenormalized(-60.0f);

        body::AudioBuffer<float> buffer(2, numSamples);
        for (int ch = 0; ch < 2; ++ch)
            for (int i = 0; i < numSamples; ++i)
                buffer.getChannel(ch)[i] = testValue;

        body::ProcessContext ctx;
        ctx.audioOut = std::move(buffer);
        ctx.blockSize = numSamples;
        plugin.processBlock(ctx);

        for (int i = 0; i < numSamples; ++i)
            REQUIRE(std::abs(ctx.audioOut.getChannel(0)[i]) < 0.01f);
    }
}

TEST_CASE("Integration: Bypass leaves audio untouched", "[integration]") {
    TestPlugin plugin;
    plugin.prepare(44100.0, 512);
    plugin.bypassParam_->setDenormalized(1.0f);  // Enable bypass

    constexpr int numSamples = 64;
    constexpr float testValue = 0.75f;

    // Set gain to something non-unity to confirm bypass skips processing
    plugin.gainParam_->setDenormalized(-20.0f);

    body::AudioBuffer<float> buffer(2, numSamples);
    for (int ch = 0; ch < 2; ++ch)
        for (int i = 0; i < numSamples; ++i)
            buffer.getChannel(ch)[i] = testValue;

    body::ProcessContext ctx;
    ctx.audioOut = std::move(buffer);
    ctx.blockSize = numSamples;
    plugin.processBlock(ctx);

    // Audio should be untouched because bypass is on
    for (int ch = 0; ch < 2; ++ch)
        for (int i = 0; i < numSamples; ++i)
            REQUIRE_THAT(ctx.audioOut.getChannel(ch)[i],
                         Catch::Matchers::WithinAbs(testValue, 0.0001f));
}
