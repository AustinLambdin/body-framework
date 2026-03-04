// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#if defined(__APPLE__)

#include "body/core/AudioProcessor.hpp"
#include "pluginterfaces/gui/iplugview.h"
#include <memory>

namespace body {

class PlatformViewMac;

/// @brief VST3 IPlugView implementation that hosts a PluginEditor via PlatformViewMac
class Vst3EditorView : public Steinberg::IPlugView {
public:
    explicit Vst3EditorView(AudioProcessor& processor);
    ~Vst3EditorView();

    // IPlugView
    Steinberg::tresult PLUGIN_API isPlatformTypeSupported(Steinberg::FIDString type) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API attached(void* parent, Steinberg::FIDString type) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API removed() SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API onWheel(float distance) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API onKeyDown(Steinberg::char16 key, Steinberg::int16 keyCode,
                                             Steinberg::int16 modifiers) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API onKeyUp(Steinberg::char16 key, Steinberg::int16 keyCode,
                                           Steinberg::int16 modifiers) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API getSize(Steinberg::ViewRect* size) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API onSize(Steinberg::ViewRect* newSize) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API onFocus(Steinberg::TBool state) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API setFrame(Steinberg::IPlugFrame* frame) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API canResize() SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API checkSizeConstraint(Steinberg::ViewRect* rect) SMTG_OVERRIDE;

    // FUnknown
    Steinberg::tresult PLUGIN_API queryInterface(const Steinberg::TUID iid, void** obj) SMTG_OVERRIDE;
    Steinberg::uint32 PLUGIN_API addRef() SMTG_OVERRIDE;
    Steinberg::uint32 PLUGIN_API release() SMTG_OVERRIDE;

private:
    AudioProcessor& processor_;
    std::unique_ptr<PlatformViewMac> platformView_;
    Steinberg::IPlugFrame* frame_ = nullptr;
    std::atomic<Steinberg::uint32> refCount_{1};
    float desiredWidth_ = 400.0f;
    float desiredHeight_ = 300.0f;
};

} // namespace body

#endif // __APPLE__
