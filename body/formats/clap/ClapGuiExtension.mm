// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#if defined(__APPLE__)

#include "ClapExtensions.hpp"
#include "ClapPlugin.hpp"
#include "body/gui/platform/mac/PlatformViewMac.hpp"
#import <AppKit/AppKit.h>
#include <clap/ext/gui.h>
#include <cstring>

namespace body {

const clap_plugin_gui_t* ClapExtensions::getGuiExtension() {
    static const clap_plugin_gui_t ext = {
        /* is_api_supported */ guiIsApiSupported,
        /* get_preferred_api */ [](const clap_plugin_t*, const char** api, bool* isFloating) -> bool {
            *api = CLAP_WINDOW_API_COCOA;
            *isFloating = false;
            return true;
        },
        /* create */ guiCreate,
        /* destroy */ guiDestroy,
        /* set_scale */ guiSetScale,
        /* get_size */ guiGetSize,
        /* can_resize */ guiCanResize,
        /* get_resize_hints */ [](const clap_plugin_t*, clap_gui_resize_hints_t* hints) -> bool {
            hints->can_resize_horizontally = false;
            hints->can_resize_vertically = false;
            hints->preserve_aspect_ratio = false;
            hints->aspect_ratio_width = 0;
            hints->aspect_ratio_height = 0;
            return true;
        },
        /* adjust_size */ guiAdjustSize,
        /* set_size */ guiSetSize,
        /* set_parent */ guiSetParent,
        /* set_transient */ guiSetTransient,
        /* suggest_title */ guiSuggestTitle,
        /* show */ guiShow,
        /* hide */ guiHide
    };
    return &ext;
}

bool ClapExtensions::guiIsApiSupported(const clap_plugin_t* /*plugin*/, const char* api,
                                        bool isFloating) {
    if (isFloating) return false;
    return strcmp(api, CLAP_WINDOW_API_COCOA) == 0;
}

bool ClapExtensions::guiCreate(const clap_plugin_t* plugin, const char* api, bool isFloating) {
    if (isFloating || strcmp(api, CLAP_WINDOW_API_COCOA) != 0) return false;

    auto* self = ClapPlugin::fromPlugin(plugin);
    auto* proc = self->getProcessor();
    if (!proc) return false;

    auto editor = proc->createEditor();
    if (!editor) return false;

    self->platformView_ = std::make_unique<PlatformViewMac>(std::move(editor));
    return true;
}

void ClapExtensions::guiDestroy(const clap_plugin_t* plugin) {
    auto* self = ClapPlugin::fromPlugin(plugin);
    self->platformView_.reset();
}

bool ClapExtensions::guiSetScale(const clap_plugin_t* /*plugin*/, double /*scale*/) {
    return false;
}

bool ClapExtensions::guiGetSize(const clap_plugin_t* plugin, uint32_t* width, uint32_t* height) {
    auto* self = ClapPlugin::fromPlugin(plugin);
    if (!self->platformView_) return false;
    *width = static_cast<uint32_t>(self->platformView_->getWidth());
    *height = static_cast<uint32_t>(self->platformView_->getHeight());
    return true;
}

bool ClapExtensions::guiCanResize(const clap_plugin_t* /*plugin*/) {
    return false;
}

bool ClapExtensions::guiAdjustSize(const clap_plugin_t* /*plugin*/, uint32_t* /*width*/,
                                     uint32_t* /*height*/) {
    return false;
}

bool ClapExtensions::guiSetSize(const clap_plugin_t* /*plugin*/, uint32_t /*width*/,
                                  uint32_t /*height*/) {
    return false;
}

bool ClapExtensions::guiSetParent(const clap_plugin_t* plugin, const clap_window_t* window) {
    auto* self = ClapPlugin::fromPlugin(plugin);
    if (!self->platformView_ || !window) return false;

    void* nsView = self->platformView_->getNativeView();
    if (!nsView) return false;

    NSView* parentView = (__bridge NSView*)window->cocoa;
    NSView* editorView = (__bridge NSView*)nsView;
    [parentView addSubview:editorView];
    return true;
}

bool ClapExtensions::guiSetTransient(const clap_plugin_t* /*plugin*/,
                                       const clap_window_t* /*window*/) {
    return false;
}

void ClapExtensions::guiSuggestTitle(const clap_plugin_t* /*plugin*/, const char* /*title*/) {
}

bool ClapExtensions::guiShow(const clap_plugin_t* plugin) {
    auto* self = ClapPlugin::fromPlugin(plugin);
    if (!self->platformView_) return false;
    NSView* view = (__bridge NSView*)self->platformView_->getNativeView();
    [view setHidden:NO];
    return true;
}

bool ClapExtensions::guiHide(const clap_plugin_t* plugin) {
    auto* self = ClapPlugin::fromPlugin(plugin);
    if (!self->platformView_) return false;
    NSView* view = (__bridge NSView*)self->platformView_->getNativeView();
    [view setHidden:YES];
    return true;
}

} // namespace body

#endif // __APPLE__
