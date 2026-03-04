// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "SynthPlugin.hpp"
#include "SynthEditor.hpp"
#include <cmath>

SynthPlugin::SynthPlugin() {
    waveformParam_ = &parameters_.addParameter<body::ChoiceParameter>(
        body::ParameterID("waveform"), "Waveform",
        std::vector<std::string>{"Sine", "Saw", "Square", "Triangle"}, 1);

    filterCutoffParam_ = &parameters_.addParameter<body::FloatParameter>(
        body::ParameterID("filterCutoff"), "Filter Cutoff",
        20.0f, 20000.0f, 5000.0f, "Hz");

    filterResoParam_ = &parameters_.addParameter<body::FloatParameter>(
        body::ParameterID("filterReso"), "Filter Resonance",
        0.1f, 10.0f, 0.707f);

    attackParam_ = &parameters_.addParameter<body::FloatParameter>(
        body::ParameterID("attack"), "Attack",
        0.001f, 5.0f, 0.01f, "s");

    decayParam_ = &parameters_.addParameter<body::FloatParameter>(
        body::ParameterID("decay"), "Decay",
        0.001f, 5.0f, 0.1f, "s");

    sustainParam_ = &parameters_.addParameter<body::FloatParameter>(
        body::ParameterID("sustain"), "Sustain",
        0.0f, 1.0f, 0.7f);

    releaseParam_ = &parameters_.addParameter<body::FloatParameter>(
        body::ParameterID("release"), "Release",
        0.001f, 10.0f, 0.3f, "s");

    volumeParam_ = &parameters_.addParameter<body::FloatParameter>(
        body::ParameterID("volume"), "Volume",
        -60.0f, 12.0f, -6.0f, "dB");
}

void SynthPlugin::prepare(double sampleRate, int /*maxBlockSize*/) {
    oscillator_.prepare(sampleRate);
    envelope_.prepare(sampleRate);
    filter_.prepare(sampleRate);

    oscillator_.setWaveform(
        static_cast<body::dsp::Oscillator::Waveform>(waveformParam_->getChoiceIndex()));
    filter_.setParameters(body::dsp::SVFilter::Type::LowPass,
                          filterCutoffParam_->getDenormalized(),
                          filterResoParam_->getDenormalized());
    envelope_.setParameters(attackParam_->getDenormalized(),
                            decayParam_->getDenormalized(),
                            sustainParam_->getDenormalized(),
                            releaseParam_->getDenormalized());
}

void SynthPlugin::processBlock(body::ProcessContext& ctx) {
    // Read parameters atomically
    auto waveform = static_cast<body::dsp::Oscillator::Waveform>(
        waveformParam_->getChoiceIndex());
    float cutoff = filterCutoffParam_->getDenormalized();
    float reso = filterResoParam_->getDenormalized();
    float attack = attackParam_->getDenormalized();
    float decay = decayParam_->getDenormalized();
    float sustain = sustainParam_->getDenormalized();
    float release = releaseParam_->getDenormalized();
    float volumeDb = volumeParam_->getDenormalized();
    float volumeGain = std::pow(10.0f, volumeDb / 20.0f);

    // Update DSP state
    oscillator_.setWaveform(waveform);
    filter_.setParameters(body::dsp::SVFilter::Type::LowPass, cutoff, reso);
    envelope_.setParameters(attack, decay, sustain, release);

    // Process MIDI events
    for (const auto& msg : ctx.midiIn) {
        if (msg.getType() == body::MidiMessage::Type::NoteOn && msg.getVelocity() > 0) {
            currentNote_ = msg.getNoteNumber();
            currentVelocity_ = static_cast<float>(msg.getVelocity()) / 127.0f;
            float freq = 440.0f * std::pow(2.0f,
                (static_cast<float>(currentNote_) - 69.0f) / 12.0f);
            oscillator_.setFrequency(freq);
            envelope_.noteOn();
        } else if (msg.getType() == body::MidiMessage::Type::NoteOff ||
                   (msg.getType() == body::MidiMessage::Type::NoteOn &&
                    msg.getVelocity() == 0)) {
            if (msg.getNoteNumber() == currentNote_) {
                envelope_.noteOff();
                currentNote_ = -1;
            }
        }
    }

    // Generate audio: Oscillator → Filter → Envelope → Volume
    int numSamples = ctx.audioOut.getNumSamples();
    int numChannels = ctx.audioOut.getNumChannels();

    if (numChannels == 0 || numSamples == 0) return;

    // Render mono into channel 0
    auto ch0 = ctx.audioOut.getChannel(0);
    for (int i = 0; i < numSamples; ++i) {
        float sample = oscillator_.process();
        sample = filter_.process(sample);
        sample *= envelope_.process() * currentVelocity_ * volumeGain;
        ch0[i] = sample;
    }

    // Copy mono to remaining channels
    for (int ch = 1; ch < numChannels; ++ch) {
        auto dst = ctx.audioOut.getChannel(ch);
        for (int i = 0; i < numSamples; ++i)
            dst[i] = ch0[i];
    }
}

void SynthPlugin::reset() {
    oscillator_.reset();
    envelope_.reset();
    filter_.reset();
    currentNote_ = -1;
    currentVelocity_ = 0.0f;
}

body::BusLayout SynthPlugin::getDefaultBusLayout() const {
    body::BusLayout layout;
    // Instrument: no inputs, stereo output
    layout.outputBuses.push_back({"Main Output", 2});
    return layout;
}

std::unique_ptr<body::PluginEditor> SynthPlugin::createEditor() {
    return std::make_unique<SynthEditor>(*this,
        *waveformParam_, *filterCutoffParam_, *filterResoParam_,
        *attackParam_, *decayParam_, *sustainParam_, *releaseParam_,
        *volumeParam_);
}
