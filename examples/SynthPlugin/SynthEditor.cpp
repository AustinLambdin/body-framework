// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "SynthEditor.hpp"

namespace {
    constexpr float kKnobSize = 70.0f;
    constexpr float kLabelHeight = 18.0f;
    constexpr float kRow1Y = 60.0f;
    constexpr float kRow2Y = 210.0f;
    const body::Colour kAccentBlue{0.3f, 0.5f, 0.9f};
    const body::Colour kLabelGrey{0.8f, 0.8f, 0.8f};
}

SynthEditor::SynthEditor(body::AudioProcessor& processor,
                         body::ChoiceParameter& waveformParam,
                         body::FloatParameter& filterCutoffParam,
                         body::FloatParameter& filterResoParam,
                         body::FloatParameter& attackParam,
                         body::FloatParameter& decayParam,
                         body::FloatParameter& sustainParam,
                         body::FloatParameter& releaseParam,
                         body::FloatParameter& volumeParam)
    : body::PluginEditor(processor)
    , titleLabel_("Synth")
    , waveformKnob_(waveformParam)
    , waveformLabel_("Waveform")
    , cutoffKnob_(filterCutoffParam)
    , cutoffLabel_("Cutoff")
    , resoKnob_(filterResoParam)
    , resoLabel_("Resonance")
    , attackKnob_(attackParam)
    , attackLabel_("Attack")
    , decayKnob_(decayParam)
    , decayLabel_("Decay")
    , sustainKnob_(sustainParam)
    , sustainLabel_("Sustain")
    , releaseKnob_(releaseParam)
    , releaseLabel_("Release")
    , volumeKnob_(volumeParam)
    , volumeLabel_("Volume") {

    setSize(600.0f, 400.0f);

    // Title
    titleLabel_.setFont(body::Font("Helvetica", 28.0f, body::Font::Weight::kBold));
    titleLabel_.setTextColour(body::Colour(1.0f, 1.0f, 1.0f));
    addChild(&titleLabel_);

    // Helper to set up a knob + label pair
    auto setupKnob = [this](body::Knob& knob, body::Label& label) {
        knob.setValueColour(kAccentBlue);
        addChild(&knob);
        label.setFont(body::Font(11.0f));
        label.setTextColour(kLabelGrey);
        addChild(&label);
    };

    setupKnob(waveformKnob_, waveformLabel_);
    setupKnob(cutoffKnob_, cutoffLabel_);
    setupKnob(resoKnob_, resoLabel_);
    setupKnob(attackKnob_, attackLabel_);
    setupKnob(decayKnob_, decayLabel_);
    setupKnob(sustainKnob_, sustainLabel_);
    setupKnob(releaseKnob_, releaseLabel_);
    setupKnob(volumeKnob_, volumeLabel_);

    resized();
}

void SynthEditor::paint(body::Graphics& g) {
    // Dark grey background
    g.setColour(body::Colour(0.12f, 0.12f, 0.12f));
    g.fillRect({0.0f, 0.0f, getWidth(), getHeight()});
}

void SynthEditor::resized() {
    float w = getWidth();

    titleLabel_.setBounds(0.0f, 12.0f, w, 35.0f);

    // Row 1: Waveform | Cutoff | Resonance — evenly spaced across width
    float row1Spacing = w / 4.0f;
    float knobOffset = kKnobSize / 2.0f;

    waveformKnob_.setBounds(row1Spacing * 1.0f - knobOffset, kRow1Y, kKnobSize, kKnobSize);
    waveformLabel_.setBounds(row1Spacing * 1.0f - knobOffset, kRow1Y + kKnobSize + 4.0f,
                             kKnobSize, kLabelHeight);

    cutoffKnob_.setBounds(row1Spacing * 2.0f - knobOffset, kRow1Y, kKnobSize, kKnobSize);
    cutoffLabel_.setBounds(row1Spacing * 2.0f - knobOffset, kRow1Y + kKnobSize + 4.0f,
                           kKnobSize, kLabelHeight);

    resoKnob_.setBounds(row1Spacing * 3.0f - knobOffset, kRow1Y, kKnobSize, kKnobSize);
    resoLabel_.setBounds(row1Spacing * 3.0f - knobOffset, kRow1Y + kKnobSize + 4.0f,
                         kKnobSize, kLabelHeight);

    // Row 2: A | D | S | R | Volume — 5 items across width
    float row2Spacing = w / 6.0f;

    attackKnob_.setBounds(row2Spacing * 1.0f - knobOffset, kRow2Y, kKnobSize, kKnobSize);
    attackLabel_.setBounds(row2Spacing * 1.0f - knobOffset, kRow2Y + kKnobSize + 4.0f,
                           kKnobSize, kLabelHeight);

    decayKnob_.setBounds(row2Spacing * 2.0f - knobOffset, kRow2Y, kKnobSize, kKnobSize);
    decayLabel_.setBounds(row2Spacing * 2.0f - knobOffset, kRow2Y + kKnobSize + 4.0f,
                          kKnobSize, kLabelHeight);

    sustainKnob_.setBounds(row2Spacing * 3.0f - knobOffset, kRow2Y, kKnobSize, kKnobSize);
    sustainLabel_.setBounds(row2Spacing * 3.0f - knobOffset, kRow2Y + kKnobSize + 4.0f,
                            kKnobSize, kLabelHeight);

    releaseKnob_.setBounds(row2Spacing * 4.0f - knobOffset, kRow2Y, kKnobSize, kKnobSize);
    releaseLabel_.setBounds(row2Spacing * 4.0f - knobOffset, kRow2Y + kKnobSize + 4.0f,
                            kKnobSize, kLabelHeight);

    volumeKnob_.setBounds(row2Spacing * 5.0f - knobOffset, kRow2Y, kKnobSize, kKnobSize);
    volumeLabel_.setBounds(row2Spacing * 5.0f - knobOffset, kRow2Y + kKnobSize + 4.0f,
                           kKnobSize, kLabelHeight);
}
