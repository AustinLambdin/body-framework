// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "Knob.hpp"
#include <algorithm>
#include <cmath>

namespace body {

Knob::Knob(Parameter& parameter)
    : attachment_(parameter) {
    attachment_.setCallback([this](float) { repaint(); });
}

void Knob::paint(Graphics& g) {
    float w = getWidth();
    float h = getHeight();
    float size = std::min(w, h);
    float radius = size * 0.4f;
    Point<float> centre{w * 0.5f, h * 0.5f};

    float normalized = attachment_.getNormalizedValue();
    float valueAngle = kStartAngle + normalized * kAngleRange;

    // Background arc (track)
    g.setColour(trackColour_);
    g.drawArc(centre, radius, kStartAngle, kEndAngle, 3.0f);

    // Value arc
    g.setColour(valueColour_);
    g.drawArc(centre, radius, kStartAngle, valueAngle, 3.0f);

    // Indicator dot
    float dotRadius = size * 0.06f;
    float dotX = centre.x + radius * std::cos(valueAngle);
    float dotY = centre.y + radius * std::sin(valueAngle);
    g.setColour(thumbColour_);
    g.fillEllipse({dotX - dotRadius, dotY - dotRadius, dotRadius * 2.0f, dotRadius * 2.0f});
}

void Knob::mouseDown(const MouseEvent& event) {
    dragStartValue_ = attachment_.getNormalizedValue();
    dragStartY_ = event.position.y;
}

void Knob::mouseDrag(const MouseEvent& event) {
    float delta = (dragStartY_ - event.position.y) / 200.0f;
    float newValue = std::clamp(dragStartValue_ + delta, 0.0f, 1.0f);
    attachment_.setNormalizedValue(newValue);
    repaint();
}

void Knob::mouseWheel(const MouseEvent& event) {
    float delta = event.wheelDeltaY * 0.01f;
    float current = attachment_.getNormalizedValue();
    float newValue = std::clamp(current + delta, 0.0f, 1.0f);
    attachment_.setNormalizedValue(newValue);
    repaint();
}

void Knob::checkForUpdates() {
    attachment_.checkForUpdates();
}

} // namespace body
