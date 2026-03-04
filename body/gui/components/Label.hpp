// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include "body/gui/Component.hpp"
#include "body/gui/Font.hpp"
#include <string>

namespace body {

class Parameter;

/// @brief Text display component
///
/// Shows static text or can bind to a Parameter to display its value.
class Label : public Component {
public:
    enum class Justification { kLeft, kCentre, kRight };

    Label();
    explicit Label(const std::string& text);

    void paint(Graphics& g) override;

    void setText(const std::string& text);
    [[nodiscard]] const std::string& getText() const noexcept { return text_; }

    void setFont(const Font& font) { font_ = font; }
    void setTextColour(const Colour& c) { textColour_ = c; }
    void setJustification(Justification j) { justification_ = j; }

    void setParameter(Parameter* param) { parameter_ = param; }

private:
    std::string text_;
    Font font_;
    Colour textColour_{1.0f, 1.0f, 1.0f};
    Justification justification_ = Justification::kCentre;
    Parameter* parameter_ = nullptr;
};

} // namespace body
