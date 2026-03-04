// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include "body/gui/Component.hpp"
#include "body/gui/Font.hpp"
#include <functional>
#include <string>

namespace body {

/// @brief Clickable button component
class Button : public Component {
public:
    using ClickCallback = std::function<void()>;

    Button();
    explicit Button(const std::string& text);

    void paint(Graphics& g) override;
    void mouseDown(const MouseEvent& event) override;
    void mouseUp(const MouseEvent& event) override;

    void setText(const std::string& text);
    void setOnClick(ClickCallback callback) { onClick_ = std::move(callback); }

    void setFont(const Font& font) { font_ = font; }
    void setTextColour(const Colour& c) { textColour_ = c; }
    void setBackgroundColour(const Colour& c) { backgroundColour_ = c; }
    void setPressedColour(const Colour& c) { pressedColour_ = c; }

private:
    std::string text_;
    Font font_;
    ClickCallback onClick_;
    bool isPressed_ = false;

    Colour textColour_{1.0f, 1.0f, 1.0f};
    Colour backgroundColour_{0.3f, 0.3f, 0.3f};
    Colour pressedColour_{0.2f, 0.2f, 0.2f};
};

} // namespace body
