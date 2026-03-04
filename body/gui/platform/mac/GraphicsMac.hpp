// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#if defined(__APPLE__)

#include "body/gui/Graphics.hpp"
#include <CoreGraphics/CoreGraphics.h>

namespace body {

/// @brief CoreGraphics implementation of the Graphics interface
class GraphicsMac : public Graphics {
public:
    explicit GraphicsMac(CGContextRef context);

    void setColour(const Colour& colour) override;

    void fillRect(const Rectangle<float>& rect) override;
    void fillRoundedRect(const Rectangle<float>& rect, float cornerRadius) override;
    void fillEllipse(const Rectangle<float>& rect) override;

    void drawRect(const Rectangle<float>& rect, float lineWidth) override;
    void drawRoundedRect(const Rectangle<float>& rect, float cornerRadius,
                          float lineWidth) override;
    void drawLine(Point<float> start, Point<float> end, float lineWidth) override;

    void drawArc(Point<float> centre, float radius,
                  float startAngle, float endAngle, float lineWidth) override;
    void fillArc(Point<float> centre, float radius,
                  float startAngle, float endAngle) override;

    void drawText(const std::string& text, const Rectangle<float>& bounds,
                   const Font& font) override;

    void saveState() override;
    void restoreState() override;
    void setClipBounds(const Rectangle<float>& bounds) override;
    void translate(float dx, float dy) override;

private:
    CGContextRef context_;
    Colour currentColour_;
};

} // namespace body

#endif // __APPLE__
