// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include "body/gui/Component.hpp"
#include "body/gui/ParameterAttachment.hpp"

namespace body {

/// @brief Rotary knob component bound to a Parameter
///
/// Vertical drag changes value. Paints a background arc, value arc, and indicator dot.
/// Arc range: 225 degrees (from 7 o'clock to 5 o'clock).
class Knob : public Component {
public:
    explicit Knob(Parameter& parameter);

    void paint(Graphics& g) override;
    void mouseDown(const MouseEvent& event) override;
    void mouseDrag(const MouseEvent& event) override;
    void mouseWheel(const MouseEvent& event) override;

    void checkForUpdates();

    void setTrackColour(const Colour& c) { trackColour_ = c; }
    void setValueColour(const Colour& c) { valueColour_ = c; }
    void setThumbColour(const Colour& c) { thumbColour_ = c; }

private:
    ParameterAttachment attachment_;
    float dragStartValue_ = 0.0f;
    float dragStartY_ = 0.0f;

    Colour trackColour_{0.3f, 0.3f, 0.3f};
    Colour valueColour_{0.4f, 0.7f, 1.0f};
    Colour thumbColour_{1.0f, 1.0f, 1.0f};

    static constexpr float kStartAngle = 2.356194f;  // 135 degrees = 3π/4
    static constexpr float kEndAngle = 7.068583f;     // 405 degrees = 9π/4 - π/4
    static constexpr float kAngleRange = kEndAngle - kStartAngle;
};

} // namespace body
