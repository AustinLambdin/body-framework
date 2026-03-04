// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#if defined(__APPLE__)

#include "Vst3EditorView.hpp"
#include "body/gui/platform/mac/PlatformViewMac.hpp"
#import <AppKit/AppKit.h>
#include "pluginterfaces/base/ftypes.h"

namespace body {

Vst3EditorView::Vst3EditorView(AudioProcessor& processor)
    : processor_(processor) {}

Vst3EditorView::~Vst3EditorView() = default;

Steinberg::tresult PLUGIN_API Vst3EditorView::isPlatformTypeSupported(Steinberg::FIDString type) {
    if (strcmp(type, Steinberg::kPlatformTypeNSView) == 0)
        return Steinberg::kResultTrue;
    return Steinberg::kResultFalse;
}

Steinberg::tresult PLUGIN_API Vst3EditorView::attached(void* parent, Steinberg::FIDString type) {
    if (strcmp(type, Steinberg::kPlatformTypeNSView) != 0)
        return Steinberg::kResultFalse;

    auto editor = processor_.createEditor();
    if (!editor) return Steinberg::kResultFalse;

    desiredWidth_ = editor->getDesiredWidth();
    desiredHeight_ = editor->getDesiredHeight();

    platformView_ = std::make_unique<PlatformViewMac>(std::move(editor));

    NSView* parentView = (__bridge NSView*)parent;
    NSView* editorView = (__bridge NSView*)platformView_->getNativeView();
    [parentView addSubview:editorView];

    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Vst3EditorView::removed() {
    platformView_.reset();
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Vst3EditorView::onWheel(float /*distance*/) {
    return Steinberg::kResultFalse;
}

Steinberg::tresult PLUGIN_API Vst3EditorView::onKeyDown(Steinberg::char16 /*key*/,
                                                          Steinberg::int16 /*keyCode*/,
                                                          Steinberg::int16 /*modifiers*/) {
    return Steinberg::kResultFalse;
}

Steinberg::tresult PLUGIN_API Vst3EditorView::onKeyUp(Steinberg::char16 /*key*/,
                                                        Steinberg::int16 /*keyCode*/,
                                                        Steinberg::int16 /*modifiers*/) {
    return Steinberg::kResultFalse;
}

Steinberg::tresult PLUGIN_API Vst3EditorView::getSize(Steinberg::ViewRect* size) {
    if (!size) return Steinberg::kInvalidArgument;
    size->left = 0;
    size->top = 0;
    size->right = static_cast<Steinberg::int32>(desiredWidth_);
    size->bottom = static_cast<Steinberg::int32>(desiredHeight_);
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Vst3EditorView::onSize(Steinberg::ViewRect* /*newSize*/) {
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Vst3EditorView::onFocus(Steinberg::TBool /*state*/) {
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Vst3EditorView::setFrame(Steinberg::IPlugFrame* frame) {
    frame_ = frame;
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Vst3EditorView::canResize() {
    return Steinberg::kResultFalse;
}

Steinberg::tresult PLUGIN_API Vst3EditorView::checkSizeConstraint(Steinberg::ViewRect* rect) {
    if (!rect) return Steinberg::kInvalidArgument;
    rect->right = rect->left + static_cast<Steinberg::int32>(desiredWidth_);
    rect->bottom = rect->top + static_cast<Steinberg::int32>(desiredHeight_);
    return Steinberg::kResultOk;
}

// FUnknown
Steinberg::tresult PLUGIN_API Vst3EditorView::queryInterface(const Steinberg::TUID iid,
                                                               void** obj) {
    if (Steinberg::FUnknownPrivate::iidEqual(iid, Steinberg::IPlugView::iid) ||
        Steinberg::FUnknownPrivate::iidEqual(iid, Steinberg::FUnknown::iid)) {
        addRef();
        *obj = static_cast<Steinberg::IPlugView*>(this);
        return Steinberg::kResultOk;
    }
    *obj = nullptr;
    return Steinberg::kNoInterface;
}

Steinberg::uint32 PLUGIN_API Vst3EditorView::addRef() {
    return ++refCount_;
}

Steinberg::uint32 PLUGIN_API Vst3EditorView::release() {
    auto count = --refCount_;
    if (count == 0) delete this;
    return count;
}

} // namespace body

#endif // __APPLE__
