// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "BusLayout.hpp"

namespace body {

BusLayout BusLayout::stereoInOut() {
    BusLayout layout;
    layout.inputBuses.push_back({"Main Input", 2});
    layout.outputBuses.push_back({"Main Output", 2});
    return layout;
}

int BusLayout::getTotalInputChannels() const noexcept {
    int total = 0;
    for (const auto& bus : inputBuses) {
        total += bus.numChannels;
    }
    return total;
}

int BusLayout::getTotalOutputChannels() const noexcept {
    int total = 0;
    for (const auto& bus : outputBuses) {
        total += bus.numChannels;
    }
    return total;
}

} // namespace body
