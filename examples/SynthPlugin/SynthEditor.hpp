// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include "body/gui/PluginEditor.hpp"
#include "body/gui/components/Knob.hpp"
#include "body/gui/components/Label.hpp"

/// @brief Editor for SynthPlugin — knobs for all synth parameters
class SynthEditor : public body::PluginEditor {
public:
    SynthEditor(body::AudioProcessor& processor,
                body::ChoiceParameter& waveformParam,
                body::FloatParameter& filterCutoffParam,
                body::FloatParameter& filterResoParam,
                body::FloatParameter& attackParam,
                body::FloatParameter& decayParam,
                body::FloatParameter& sustainParam,
                body::FloatParameter& releaseParam,
                body::FloatParameter& volumeParam);

    void paint(body::Graphics& g) override;
    void resized() override;

private:
    body::Label titleLabel_;

    // Row 1: Waveform, Filter
    body::Knob waveformKnob_;
    body::Label waveformLabel_;
    body::Knob cutoffKnob_;
    body::Label cutoffLabel_;
    body::Knob resoKnob_;
    body::Label resoLabel_;

    // Row 2: ADSR, Volume
    body::Knob attackKnob_;
    body::Label attackLabel_;
    body::Knob decayKnob_;
    body::Label decayLabel_;
    body::Knob sustainKnob_;
    body::Label sustainLabel_;
    body::Knob releaseKnob_;
    body::Label releaseLabel_;
    body::Knob volumeKnob_;
    body::Label volumeLabel_;
};
