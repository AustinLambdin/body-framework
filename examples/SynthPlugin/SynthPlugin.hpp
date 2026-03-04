// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include "body/core/AudioProcessor.hpp"
#include "body/dsp/Envelope.hpp"
#include "body/dsp/Oscillator.hpp"
#include "body/dsp/SVFilter.hpp"

namespace body { class PluginEditor; }

/// @brief Monophonic subtractive synth — demonstrates the BODY DSP library
///
/// Signal chain: Oscillator → SVFilter (low-pass) → ADSR Envelope → Volume
/// Driven by MIDI note on/off events.
class SynthPlugin : public body::AudioProcessor {
public:
    SynthPlugin();

    [[nodiscard]] std::string getName() const override { return "Synth"; }
    [[nodiscard]] std::string getVendor() const override { return "BODY Examples"; }
    [[nodiscard]] std::string getVersion() const override { return "1.0.0"; }

    void prepare(double sampleRate, int maxBlockSize) override;
    void processBlock(body::ProcessContext& context) override;
    void reset() override;

    [[nodiscard]] body::BusLayout getDefaultBusLayout() const override;

    [[nodiscard]] std::unique_ptr<body::PluginEditor> createEditor() override;

private:
    // Parameters
    body::ChoiceParameter* waveformParam_ = nullptr;
    body::FloatParameter* filterCutoffParam_ = nullptr;
    body::FloatParameter* filterResoParam_ = nullptr;
    body::FloatParameter* attackParam_ = nullptr;
    body::FloatParameter* decayParam_ = nullptr;
    body::FloatParameter* sustainParam_ = nullptr;
    body::FloatParameter* releaseParam_ = nullptr;
    body::FloatParameter* volumeParam_ = nullptr;

    // DSP
    body::dsp::Oscillator oscillator_;
    body::dsp::Envelope envelope_;
    body::dsp::SVFilter filter_;

    // Voice state
    int currentNote_ = -1;
    float currentVelocity_ = 0.0f;
};
