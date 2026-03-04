// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "Toggle.hpp"
#include "body/core/Parameter.hpp"

namespace body {

Toggle::Toggle(BoolParameter& parameter)
    : attachment_(parameter) {
    attachment_.setCallback([this](float) { repaint(); });
}

void Toggle::paint(Graphics& g) {
    float w = getWidth();
    float h = getHeight();
    bool isOn = attachment_.getValue() >= 0.5f;

    // Track
    g.setColour(isOn ? onColour_ : offColour_);
    g.fillRoundedRect({0.0f, 0.0f, w, h}, h * 0.5f);

    // Thumb circle
    float thumbDiameter = h * 0.8f;
    float thumbMargin = h * 0.1f;
    float thumbX = isOn ? (w - thumbDiameter - thumbMargin) : thumbMargin;
    float thumbY = thumbMargin;
    g.setColour(thumbColour_);
    g.fillEllipse({thumbX, thumbY, thumbDiameter, thumbDiameter});
}

void Toggle::mouseDown(const MouseEvent& /*event*/) {
    bool isOn = attachment_.getValue() >= 0.5f;
    attachment_.setValue(isOn ? 0.0f : 1.0f);
    repaint();
}

void Toggle::checkForUpdates() {
    attachment_.checkForUpdates();
}

} // namespace body
