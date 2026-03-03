// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include "BusLayout.hpp"
#include "ParameterGroup.hpp"
#include "ProcessContext.hpp"
#include <string>

namespace body {

/// @brief Base class that all BODY plugins inherit from
///
/// Subclass this and implement prepare(), processBlock(), and the metadata
/// methods to create a plugin. Format wrappers handle all host communication.
class AudioProcessor {
public:
    virtual ~AudioProcessor() = default;

    /// @brief Called once when the plugin is initialized
    /// @param sampleRate The host sample rate in Hz
    /// @param maxBlockSize Maximum number of samples per block
    /// @note Allocations should happen here, not in processBlock()
    virtual void prepare(double sampleRate, int maxBlockSize) = 0;

    /// @brief Called for every block of audio — THE hot path
    /// @param context Contains audio/MIDI buffers and transport info
    /// @note Must be real-time safe — no allocations, no locks, no I/O
    virtual void processBlock(ProcessContext& context) = 0;

    /// @brief Called when the plugin is reset (transport stop, etc.)
    virtual void reset() {}

    // Plugin metadata
    [[nodiscard]] virtual std::string getName() const = 0;
    [[nodiscard]] virtual std::string getVendor() const = 0;
    [[nodiscard]] virtual std::string getVersion() const = 0;

    // Parameter access
    [[nodiscard]] ParameterGroup& getParameters() { return parameters_; }
    [[nodiscard]] const ParameterGroup& getParameters() const { return parameters_; }

    // Bus layout — override to customize
    [[nodiscard]] virtual BusLayout getDefaultBusLayout() const;
    [[nodiscard]] virtual bool isBusLayoutSupported(const BusLayout& layout) const;

protected:
    ParameterGroup parameters_;
};

} // namespace body
