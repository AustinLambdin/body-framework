// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "GainPlugin.hpp"
#include "body/formats/clap/ClapEntry.hpp"
#include <cmath>

GainPlugin::GainPlugin() {
    gainParam_ = &parameters_.addParameter<body::FloatParameter>(
        body::ParameterID("gain"), "Gain", -60.0f, 24.0f, 0.0f, "dB");

    bypassParam_ = &parameters_.addParameter<body::BoolParameter>(
        body::ParameterID("bypass"), "Bypass", false);
}

void GainPlugin::prepare(double sampleRate, int /*maxBlockSize*/) {
    // Smoothing coefficient: ~10ms time constant
    smoothingCoeff_ = 1.0f - std::exp(-1.0f / (static_cast<float>(sampleRate) * 0.01f));
    currentGain_ = std::pow(10.0f, gainParam_->getDenormalized() / 20.0f);
}

void GainPlugin::processBlock(body::ProcessContext& ctx) {
    if (bypassParam_->getValue()) return;

    float targetDb = gainParam_->getDenormalized();
    targetGain_ = std::pow(10.0f, targetDb / 20.0f);

    for (int ch = 0; ch < ctx.audioOut.getNumChannels(); ++ch) {
        auto samples = ctx.audioOut.getChannel(ch);
        for (auto& sample : samples) {
            currentGain_ += smoothingCoeff_ * (targetGain_ - currentGain_);
            sample *= currentGain_;
        }
    }
}

void GainPlugin::reset() {
    currentGain_ = std::pow(10.0f, gainParam_->getDenormalized() / 20.0f);
    targetGain_ = currentGain_;
}

BODY_REGISTER_CLAP_PLUGIN(
    GainPlugin,
    "com.body-framework.gain",
    "Gain",
    "BODY Examples",
    "1.0.0",
    "Simple gain plugin",
    CLAP_PLUGIN_FEATURE_AUDIO_EFFECT,
    CLAP_PLUGIN_FEATURE_UTILITY
)
