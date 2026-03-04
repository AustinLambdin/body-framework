// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include "body/gui/PluginEditor.hpp"
#include "body/gui/components/Knob.hpp"
#include "body/gui/components/Label.hpp"
#include "body/gui/components/Toggle.hpp"

/// @brief Editor for the GainPlugin — demonstrates Knob, Labels, and Toggle
class GainEditor : public body::PluginEditor {
public:
    explicit GainEditor(body::AudioProcessor& processor,
                        body::FloatParameter& gainParam,
                        body::BoolParameter& bypassParam);

    void paint(body::Graphics& g) override;
    void resized() override;

private:
    body::Knob gainKnob_;
    body::Label titleLabel_;
    body::Label gainLabel_;
    body::Label bypassLabel_;
    body::Toggle bypassToggle_;
};
