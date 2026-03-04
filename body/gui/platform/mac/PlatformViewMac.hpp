// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#if defined(__APPLE__)

#include "body/gui/PluginEditor.hpp"

namespace body {

/// @brief Wraps an NSView that hosts a PluginEditor on macOS
///
/// Creates a BODYEditorView (NSView subclass) that:
/// - Paints the component tree via CoreGraphics
/// - Delivers mouse events to the component tree
/// - Polls for repaint at 60Hz via NSTimer
class PlatformViewMac {
public:
    explicit PlatformViewMac(std::unique_ptr<PluginEditor> editor);
    ~PlatformViewMac();

    PlatformViewMac(const PlatformViewMac&) = delete;
    PlatformViewMac& operator=(const PlatformViewMac&) = delete;

    /// @brief Get the native NSView* as void* for embedding in host windows
    [[nodiscard]] void* getNativeView() const noexcept;

    [[nodiscard]] float getWidth() const noexcept;
    [[nodiscard]] float getHeight() const noexcept;
    void setSize(float width, float height);

    void repaint();

    [[nodiscard]] PluginEditor* getEditor() noexcept { return editor_.get(); }

private:
    std::unique_ptr<PluginEditor> editor_;
    void* nativeView_ = nullptr; // BODYEditorView*
    void* timer_ = nullptr;      // NSTimer*
};

} // namespace body

#endif // __APPLE__
