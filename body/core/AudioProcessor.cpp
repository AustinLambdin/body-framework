// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "AudioProcessor.hpp"
#include "body/gui/PluginEditor.hpp"

namespace body {

BusLayout AudioProcessor::getDefaultBusLayout() const {
    return BusLayout::stereoInOut();
}

bool AudioProcessor::isBusLayoutSupported(const BusLayout& layout) const {
    // By default, accept any layout with at least one output bus
    return !layout.outputBuses.empty();
}

} // namespace body
