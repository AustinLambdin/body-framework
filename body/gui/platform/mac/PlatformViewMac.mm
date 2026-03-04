// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#if defined(__APPLE__)

#include "PlatformViewMac.hpp"
#include "GraphicsMac.hpp"
#import <AppKit/AppKit.h>

// --- BODYEditorView: NSView subclass for rendering the component tree ---

@interface BODYEditorView : NSView {
    body::PluginEditor* _editor;
}
- (instancetype)initWithEditor:(body::PluginEditor*)editor;
@end

@implementation BODYEditorView

- (instancetype)initWithEditor:(body::PluginEditor*)editor {
    NSRect frame = NSMakeRect(0, 0, editor->getDesiredWidth(), editor->getDesiredHeight());
    self = [super initWithFrame:frame];
    if (self) {
        _editor = editor;
    }
    return self;
}

- (BOOL)isFlipped {
    return YES;
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void)drawRect:(NSRect)dirtyRect {
    (void)dirtyRect;
    CGContextRef ctx = [[NSGraphicsContext currentContext] CGContext];
    if (!ctx || !_editor) return;

    body::GraphicsMac g(ctx);
    _editor->paintEntireComponent(g);
}

// --- Mouse event helpers ---

- (body::MouseEvent)mouseEventFromNS:(NSEvent*)event type:(body::MouseEvent::Type)type {
    NSPoint loc = [self convertPoint:[event locationInWindow] fromView:nil];
    body::MouseEvent me;
    me.type = type;
    me.position = {static_cast<float>(loc.x), static_cast<float>(loc.y)};
    me.shiftDown = (event.modifierFlags & NSEventModifierFlagShift) != 0;
    me.ctrlDown = (event.modifierFlags & NSEventModifierFlagControl) != 0;
    me.altDown = (event.modifierFlags & NSEventModifierFlagOption) != 0;
    me.cmdDown = (event.modifierFlags & NSEventModifierFlagCommand) != 0;

    switch (event.buttonNumber) {
        case 0: me.button = body::MouseEvent::Button::kLeft; break;
        case 1: me.button = body::MouseEvent::Button::kRight; break;
        case 2: me.button = body::MouseEvent::Button::kMiddle; break;
        default: me.button = body::MouseEvent::Button::kNone; break;
    }
    return me;
}

- (void)mouseDown:(NSEvent*)event {
    auto me = [self mouseEventFromNS:event type:body::MouseEvent::Type::kDown];
    auto* target = _editor->findComponentAt(me.position);
    if (target) {
        body::MouseEvent local = me;
        // Convert to local coordinates relative to the target's bounds
        local.position = {me.position.x - target->getBounds().x,
                          me.position.y - target->getBounds().y};
        target->mouseDown(local);
    }
}

- (void)mouseUp:(NSEvent*)event {
    auto me = [self mouseEventFromNS:event type:body::MouseEvent::Type::kUp];
    auto* target = _editor->findComponentAt(me.position);
    if (target) {
        body::MouseEvent local = me;
        local.position = {me.position.x - target->getBounds().x,
                          me.position.y - target->getBounds().y};
        target->mouseUp(local);
    }
}

- (void)mouseDragged:(NSEvent*)event {
    auto me = [self mouseEventFromNS:event type:body::MouseEvent::Type::kDrag];
    auto* target = _editor->findComponentAt(me.position);
    if (target) {
        body::MouseEvent local = me;
        local.position = {me.position.x - target->getBounds().x,
                          me.position.y - target->getBounds().y};
        target->mouseDrag(local);
    }
}

- (void)mouseMoved:(NSEvent*)event {
    auto me = [self mouseEventFromNS:event type:body::MouseEvent::Type::kMove];
    auto* target = _editor->findComponentAt(me.position);
    if (target) {
        body::MouseEvent local = me;
        local.position = {me.position.x - target->getBounds().x,
                          me.position.y - target->getBounds().y};
        target->mouseMove(local);
    }
}

- (void)scrollWheel:(NSEvent*)event {
    auto me = [self mouseEventFromNS:event type:body::MouseEvent::Type::kWheel];
    me.wheelDeltaX = static_cast<float>([event scrollingDeltaX]);
    me.wheelDeltaY = static_cast<float>([event scrollingDeltaY]);
    auto* target = _editor->findComponentAt(me.position);
    if (target) {
        body::MouseEvent local = me;
        local.position = {me.position.x - target->getBounds().x,
                          me.position.y - target->getBounds().y};
        target->mouseWheel(local);
    }
}

- (void)pollRepaint {
    if (_editor && _editor->needsRepaint()) {
        [self setNeedsDisplay:YES];
    }
}

@end

// --- PlatformViewMac implementation ---

namespace body {

PlatformViewMac::PlatformViewMac(std::unique_ptr<PluginEditor> editor)
    : editor_(std::move(editor)) {
    auto* view = [[BODYEditorView alloc] initWithEditor:editor_.get()];
    nativeView_ = (__bridge_retained void*)view;

    // 60Hz repaint timer
    NSTimer* nsTimer = [NSTimer scheduledTimerWithTimeInterval:1.0 / 60.0
                                                       target:view
                                                     selector:@selector(pollRepaint)
                                                     userInfo:nil
                                                      repeats:YES];
    timer_ = (__bridge_retained void*)nsTimer;
}

PlatformViewMac::~PlatformViewMac() {
    if (timer_) {
        NSTimer* nsTimer = (__bridge_transfer NSTimer*)timer_;
        [nsTimer invalidate];
        timer_ = nullptr;
    }
    if (nativeView_) {
        NSView* view = (__bridge_transfer NSView*)nativeView_;
        [view removeFromSuperview];
        nativeView_ = nullptr;
        (void)view;
    }
}

void* PlatformViewMac::getNativeView() const noexcept {
    return nativeView_ ? (__bridge void*)((__bridge NSView*)nativeView_) : nullptr;
}

float PlatformViewMac::getWidth() const noexcept {
    return editor_ ? editor_->getDesiredWidth() : 0.0f;
}

float PlatformViewMac::getHeight() const noexcept {
    return editor_ ? editor_->getDesiredHeight() : 0.0f;
}

void PlatformViewMac::setSize(float width, float height) {
    if (editor_) editor_->setSize(width, height);
    if (nativeView_) {
        NSView* view = (__bridge NSView*)nativeView_;
        [view setFrameSize:NSMakeSize(width, height)];
    }
}

void PlatformViewMac::repaint() {
    if (editor_) editor_->repaint();
}

} // namespace body

#endif // __APPLE__
