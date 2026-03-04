// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#if defined(__APPLE__)

#include "GraphicsMac.hpp"
#import <CoreText/CoreText.h>
#import <Foundation/Foundation.h>

namespace body {

GraphicsMac::GraphicsMac(CGContextRef context)
    : context_(context) {}

void GraphicsMac::setColour(const Colour& colour) {
    currentColour_ = colour;
    CGContextSetRGBFillColor(context_, colour.r, colour.g, colour.b, colour.a);
    CGContextSetRGBStrokeColor(context_, colour.r, colour.g, colour.b, colour.a);
}

void GraphicsMac::fillRect(const Rectangle<float>& rect) {
    CGContextFillRect(context_, CGRectMake(rect.x, rect.y, rect.width, rect.height));
}

void GraphicsMac::fillRoundedRect(const Rectangle<float>& rect, float cornerRadius) {
    CGRect cgRect = CGRectMake(rect.x, rect.y, rect.width, rect.height);
    CGPathRef path = CGPathCreateWithRoundedRect(cgRect, cornerRadius, cornerRadius, nullptr);
    CGContextAddPath(context_, path);
    CGContextFillPath(context_);
    CGPathRelease(path);
}

void GraphicsMac::fillEllipse(const Rectangle<float>& rect) {
    CGContextFillEllipseInRect(context_, CGRectMake(rect.x, rect.y, rect.width, rect.height));
}

void GraphicsMac::drawRect(const Rectangle<float>& rect, float lineWidth) {
    CGContextSetLineWidth(context_, lineWidth);
    CGContextStrokeRect(context_, CGRectMake(rect.x, rect.y, rect.width, rect.height));
}

void GraphicsMac::drawRoundedRect(const Rectangle<float>& rect, float cornerRadius,
                                    float lineWidth) {
    CGRect cgRect = CGRectMake(rect.x, rect.y, rect.width, rect.height);
    CGPathRef path = CGPathCreateWithRoundedRect(cgRect, cornerRadius, cornerRadius, nullptr);
    CGContextSetLineWidth(context_, lineWidth);
    CGContextAddPath(context_, path);
    CGContextStrokePath(context_);
    CGPathRelease(path);
}

void GraphicsMac::drawLine(Point<float> start, Point<float> end, float lineWidth) {
    CGContextSetLineWidth(context_, lineWidth);
    CGContextMoveToPoint(context_, start.x, start.y);
    CGContextAddLineToPoint(context_, end.x, end.y);
    CGContextStrokePath(context_);
}

void GraphicsMac::drawArc(Point<float> centre, float radius,
                            float startAngle, float endAngle, float lineWidth) {
    CGContextSetLineWidth(context_, lineWidth);
    CGContextAddArc(context_, centre.x, centre.y, radius, startAngle, endAngle, 0);
    CGContextStrokePath(context_);
}

void GraphicsMac::fillArc(Point<float> centre, float radius,
                            float startAngle, float endAngle) {
    CGContextMoveToPoint(context_, centre.x, centre.y);
    CGContextAddArc(context_, centre.x, centre.y, radius, startAngle, endAngle, 0);
    CGContextClosePath(context_);
    CGContextFillPath(context_);
}

void GraphicsMac::drawText(const std::string& text, const Rectangle<float>& bounds,
                             const Font& font) {
    @autoreleasepool {
        // Create attributed string
        NSString* nsText = [NSString stringWithUTF8String:text.c_str()];
        if (!nsText) return;

        NSString* fontFamily = [NSString stringWithUTF8String:font.family.c_str()];
        CTFontRef ctFont = nullptr;

        switch (font.weight) {
            case Font::Weight::kBold:
                ctFont = CTFontCreateWithName((__bridge CFStringRef)fontFamily,
                                               font.size, nullptr);
                if (ctFont) {
                    CTFontRef bold = CTFontCreateCopyWithSymbolicTraits(
                        ctFont, font.size, nullptr, kCTFontBoldTrait, kCTFontBoldTrait);
                    if (bold) { CFRelease(ctFont); ctFont = bold; }
                }
                break;
            default:
                ctFont = CTFontCreateWithName((__bridge CFStringRef)fontFamily,
                                               font.size, nullptr);
                break;
        }
        if (!ctFont) {
            ctFont = CTFontCreateUIFontForLanguage(kCTFontUIFontSystem, font.size, nullptr);
        }

        // Set text colour
        CGColorRef cgColor = CGColorCreateSRGB(currentColour_.r, currentColour_.g,
                                                currentColour_.b, currentColour_.a);

        NSDictionary* attrs = @{
            (__bridge id)kCTFontAttributeName: (__bridge id)ctFont,
            (__bridge id)kCTForegroundColorAttributeName: (__bridge id)cgColor
        };

        NSAttributedString* attrStr = [[NSAttributedString alloc]
            initWithString:nsText attributes:attrs];

        // Create framesetter and frame
        CTFramesetterRef framesetter = CTFramesetterCreateWithAttributedString(
            (__bridge CFAttributedStringRef)attrStr);

        CGRect cgBounds = CGRectMake(bounds.x, bounds.y, bounds.width, bounds.height);
        CGPathRef path = CGPathCreateWithRect(cgBounds, nullptr);
        CTFrameRef frame = CTFramesetterCreateFrame(framesetter, CFRangeMake(0, 0), path, nullptr);

        CTFrameDraw(frame, context_);

        CFRelease(frame);
        CGPathRelease(path);
        CFRelease(framesetter);
        CGColorRelease(cgColor);
        CFRelease(ctFont);
    }
}

void GraphicsMac::saveState() {
    CGContextSaveGState(context_);
}

void GraphicsMac::restoreState() {
    CGContextRestoreGState(context_);
}

void GraphicsMac::setClipBounds(const Rectangle<float>& bounds) {
    CGContextClipToRect(context_, CGRectMake(bounds.x, bounds.y, bounds.width, bounds.height));
}

void GraphicsMac::translate(float dx, float dy) {
    CGContextTranslateCTM(context_, dx, dy);
}

} // namespace body

#endif // __APPLE__
