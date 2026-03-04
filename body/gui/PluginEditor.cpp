// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "PluginEditor.hpp"

namespace body {

PluginEditor::PluginEditor(AudioProcessor& processor)
    : processor_(processor) {
    setSize(desiredWidth_, desiredHeight_);
}

void PluginEditor::setSize(float width, float height) {
    desiredWidth_ = width;
    desiredHeight_ = height;
    setBounds(0.0f, 0.0f, width, height);
}

} // namespace body
