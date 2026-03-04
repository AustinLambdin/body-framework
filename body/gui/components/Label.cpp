// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "Label.hpp"
#include "body/core/Parameter.hpp"

namespace body {

Label::Label() = default;

Label::Label(const std::string& text) : text_(text) {}

void Label::paint(Graphics& g) {
    std::string displayText = text_;
    if (parameter_) {
        displayText = parameter_->getDisplayValue();
    }

    g.setColour(textColour_);
    g.drawText(displayText, {0.0f, 0.0f, getWidth(), getHeight()}, font_);
}

void Label::setText(const std::string& text) {
    if (text_ != text) {
        text_ = text;
        repaint();
    }
}

} // namespace body
