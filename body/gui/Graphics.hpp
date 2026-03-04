// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include "Colour.hpp"
#include "Font.hpp"
#include "Point.hpp"
#include "Rectangle.hpp"
#include <string>

namespace body {

/// @brief Abstract graphics context for 2D rendering
///
/// Components paint through this interface. Platform backends (CoreGraphics,
/// Direct2D, etc.) provide concrete implementations.
class Graphics {
public:
    virtual ~Graphics() = default;

    virtual void setColour(const Colour& colour) = 0;

    virtual void fillRect(const Rectangle<float>& rect) = 0;
    virtual void fillRoundedRect(const Rectangle<float>& rect, float cornerRadius) = 0;
    virtual void fillEllipse(const Rectangle<float>& rect) = 0;

    virtual void drawRect(const Rectangle<float>& rect, float lineWidth = 1.0f) = 0;
    virtual void drawRoundedRect(const Rectangle<float>& rect, float cornerRadius,
                                  float lineWidth = 1.0f) = 0;
    virtual void drawLine(Point<float> start, Point<float> end,
                           float lineWidth = 1.0f) = 0;

    virtual void drawArc(Point<float> centre, float radius,
                          float startAngle, float endAngle, float lineWidth = 1.0f) = 0;
    virtual void fillArc(Point<float> centre, float radius,
                          float startAngle, float endAngle) = 0;

    virtual void drawText(const std::string& text, const Rectangle<float>& bounds,
                           const Font& font) = 0;

    virtual void saveState() = 0;
    virtual void restoreState() = 0;
    virtual void setClipBounds(const Rectangle<float>& bounds) = 0;
    virtual void translate(float dx, float dy) = 0;
};

} // namespace body
