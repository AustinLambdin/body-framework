// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "GainEditor.hpp"

GainEditor::GainEditor(body::AudioProcessor& processor,
                       body::FloatParameter& gainParam,
                       body::BoolParameter& bypassParam)
    : body::PluginEditor(processor)
    , gainKnob_(gainParam)
    , titleLabel_("Gain")
    , gainLabel_("")
    , bypassLabel_("Bypass")
    , bypassToggle_(bypassParam) {

    setSize(400.0f, 300.0f);

    // Title
    titleLabel_.setFont(body::Font("Helvetica", 24.0f, body::Font::Weight::kBold));
    titleLabel_.setTextColour(body::Colour(1.0f, 1.0f, 1.0f));
    addChild(&titleLabel_);

    // Gain knob
    gainKnob_.setValueColour(body::Colour(0.4f, 0.7f, 1.0f));
    addChild(&gainKnob_);

    // Gain value label (bound to parameter)
    gainLabel_.setParameter(&gainParam);
    gainLabel_.setFont(body::Font(12.0f));
    gainLabel_.setTextColour(body::Colour(0.8f, 0.8f, 0.8f));
    addChild(&gainLabel_);

    // Bypass toggle and label
    bypassLabel_.setFont(body::Font(14.0f));
    bypassLabel_.setTextColour(body::Colour(0.8f, 0.8f, 0.8f));
    addChild(&bypassLabel_);
    addChild(&bypassToggle_);

    resized();
}

void GainEditor::paint(body::Graphics& g) {
    // Dark grey background
    g.setColour(body::Colour(0.15f, 0.15f, 0.15f));
    g.fillRect({0.0f, 0.0f, getWidth(), getHeight()});
}

void GainEditor::resized() {
    float w = getWidth();

    titleLabel_.setBounds(0.0f, 15.0f, w, 30.0f);
    gainKnob_.setBounds(w * 0.5f - 60.0f, 60.0f, 120.0f, 120.0f);
    gainLabel_.setBounds(w * 0.5f - 60.0f, 185.0f, 120.0f, 20.0f);

    bypassLabel_.setBounds(w * 0.5f - 60.0f, 230.0f, 70.0f, 25.0f);
    bypassToggle_.setBounds(w * 0.5f + 15.0f, 230.0f, 50.0f, 25.0f);
}
