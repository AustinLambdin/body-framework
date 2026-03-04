// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include "body/gui/Component.hpp"
#include "body/gui/ParameterAttachment.hpp"

namespace body {

class BoolParameter;

/// @brief On/off toggle switch bound to a BoolParameter
///
/// Rounded rect track with a sliding circle indicator.
class Toggle : public Component {
public:
    explicit Toggle(BoolParameter& parameter);

    void paint(Graphics& g) override;
    void mouseDown(const MouseEvent& event) override;

    void checkForUpdates();

    void setOnColour(const Colour& c) { onColour_ = c; }
    void setOffColour(const Colour& c) { offColour_ = c; }
    void setThumbColour(const Colour& c) { thumbColour_ = c; }

private:
    ParameterAttachment attachment_;

    Colour onColour_{0.4f, 0.7f, 1.0f};
    Colour offColour_{0.3f, 0.3f, 0.3f};
    Colour thumbColour_{1.0f, 1.0f, 1.0f};
};

} // namespace body
