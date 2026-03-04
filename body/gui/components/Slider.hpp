// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include "body/gui/Component.hpp"
#include "body/gui/ParameterAttachment.hpp"

namespace body {

/// @brief Linear slider component bound to a Parameter
class Slider : public Component {
public:
    enum class Orientation { kHorizontal, kVertical };

    explicit Slider(Parameter& parameter, Orientation orientation = Orientation::kHorizontal);

    void paint(Graphics& g) override;
    void mouseDown(const MouseEvent& event) override;
    void mouseDrag(const MouseEvent& event) override;

    void checkForUpdates();

    void setTrackColour(const Colour& c) { trackColour_ = c; }
    void setFillColour(const Colour& c) { fillColour_ = c; }
    void setThumbColour(const Colour& c) { thumbColour_ = c; }

private:
    ParameterAttachment attachment_;
    Orientation orientation_;

    Colour trackColour_{0.3f, 0.3f, 0.3f};
    Colour fillColour_{0.4f, 0.7f, 1.0f};
    Colour thumbColour_{1.0f, 1.0f, 1.0f};

    void setValueFromPosition(float x, float y);
};

} // namespace body
