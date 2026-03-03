// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include <string>
#include <vector>

namespace body {

/// @brief Describes a single audio bus (input or output)
struct BusInfo {
    std::string name;
    int numChannels = 2;
};

/// @brief Describes the input/output bus configuration for a plugin
struct BusLayout {
    std::vector<BusInfo> inputBuses;
    std::vector<BusInfo> outputBuses;

    /// @brief Create a default stereo in/out layout
    [[nodiscard]] static BusLayout stereoInOut();

    /// @brief Get total number of input channels across all buses
    [[nodiscard]] int getTotalInputChannels() const noexcept;

    /// @brief Get total number of output channels across all buses
    [[nodiscard]] int getTotalOutputChannels() const noexcept;
};

} // namespace body
