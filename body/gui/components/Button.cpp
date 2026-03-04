// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "Button.hpp"

namespace body {

Button::Button() = default;

Button::Button(const std::string& text) : text_(text) {}

void Button::paint(Graphics& g) {
    g.setColour(isPressed_ ? pressedColour_ : backgroundColour_);
    g.fillRoundedRect({0.0f, 0.0f, getWidth(), getHeight()}, 4.0f);

    g.setColour(textColour_);
    g.drawText(text_, {0.0f, 0.0f, getWidth(), getHeight()}, font_);
}

void Button::mouseDown(const MouseEvent& /*event*/) {
    isPressed_ = true;
    repaint();
}

void Button::mouseUp(const MouseEvent& /*event*/) {
    if (isPressed_) {
        isPressed_ = false;
        repaint();
        if (onClick_) onClick_();
    }
}

void Button::setText(const std::string& text) {
    if (text_ != text) {
        text_ = text;
        repaint();
    }
}

} // namespace body
