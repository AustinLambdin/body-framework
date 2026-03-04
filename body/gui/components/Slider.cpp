// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "Slider.hpp"
#include <algorithm>

namespace body {

Slider::Slider(Parameter& parameter, Orientation orientation)
    : attachment_(parameter), orientation_(orientation) {
    attachment_.setCallback([this](float) { repaint(); });
}

void Slider::paint(Graphics& g) {
    float w = getWidth();
    float h = getHeight();
    float normalized = attachment_.getNormalizedValue();

    if (orientation_ == Orientation::kHorizontal) {
        float trackY = h * 0.5f - 2.0f;
        float trackH = 4.0f;

        // Background track
        g.setColour(trackColour_);
        g.fillRoundedRect({0.0f, trackY, w, trackH}, 2.0f);

        // Filled portion
        float fillW = w * normalized;
        g.setColour(fillColour_);
        g.fillRoundedRect({0.0f, trackY, fillW, trackH}, 2.0f);

        // Thumb
        float thumbW = 12.0f;
        float thumbH = h * 0.6f;
        float thumbX = fillW - thumbW * 0.5f;
        float thumbY = h * 0.5f - thumbH * 0.5f;
        g.setColour(thumbColour_);
        g.fillRoundedRect({thumbX, thumbY, thumbW, thumbH}, 4.0f);
    } else {
        float trackX = w * 0.5f - 2.0f;
        float trackW = 4.0f;

        g.setColour(trackColour_);
        g.fillRoundedRect({trackX, 0.0f, trackW, h}, 2.0f);

        float fillH = h * (1.0f - normalized);
        g.setColour(fillColour_);
        g.fillRoundedRect({trackX, fillH, trackW, h - fillH}, 2.0f);

        float thumbW = w * 0.6f;
        float thumbH = 12.0f;
        float thumbX = w * 0.5f - thumbW * 0.5f;
        float thumbY = fillH - thumbH * 0.5f;
        g.setColour(thumbColour_);
        g.fillRoundedRect({thumbX, thumbY, thumbW, thumbH}, 4.0f);
    }
}

void Slider::mouseDown(const MouseEvent& event) {
    setValueFromPosition(event.position.x, event.position.y);
}

void Slider::mouseDrag(const MouseEvent& event) {
    setValueFromPosition(event.position.x, event.position.y);
}

void Slider::setValueFromPosition(float x, float y) {
    float normalized;
    if (orientation_ == Orientation::kHorizontal) {
        normalized = std::clamp(x / getWidth(), 0.0f, 1.0f);
    } else {
        normalized = std::clamp(1.0f - y / getHeight(), 0.0f, 1.0f);
    }
    attachment_.setNormalizedValue(normalized);
    repaint();
}

void Slider::checkForUpdates() {
    attachment_.checkForUpdates();
}

} // namespace body
