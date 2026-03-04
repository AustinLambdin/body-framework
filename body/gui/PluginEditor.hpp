// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include "Component.hpp"

namespace body {

class AudioProcessor;

/// @brief Base class for plugin GUI editors
///
/// Subclass this and add child components to create a plugin UI.
/// The processor reference allows reading parameters for display.
class PluginEditor : public Component {
public:
    explicit PluginEditor(AudioProcessor& processor);
    ~PluginEditor() override = default;

    [[nodiscard]] AudioProcessor& getProcessor() noexcept { return processor_; }
    [[nodiscard]] const AudioProcessor& getProcessor() const noexcept { return processor_; }

    void setSize(float width, float height);
    [[nodiscard]] float getDesiredWidth() const noexcept { return desiredWidth_; }
    [[nodiscard]] float getDesiredHeight() const noexcept { return desiredHeight_; }

private:
    AudioProcessor& processor_;
    float desiredWidth_ = 400.0f;
    float desiredHeight_ = 300.0f;
};

} // namespace body
