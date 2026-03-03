// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include "body/core/AudioProcessor.hpp"
#include <clap/clap.h>
#include <functional>
#include <memory>
#include <string>

namespace body {

/// @brief Factory function type for creating AudioProcessor instances
using AudioProcessorFactory = std::function<std::unique_ptr<AudioProcessor>()>;

/// @brief Bridges a body::AudioProcessor to the CLAP plugin lifecycle
///
/// Implements init, destroy, activate, deactivate, process, reset.
/// Wraps host audio buffers as non-owning AudioBuffers.
class ClapPlugin {
public:
    /// @brief Register a plugin descriptor and factory for CLAP hosting
    struct Registration {
        clap_plugin_descriptor_t descriptor;
        AudioProcessorFactory factory;
    };

    /// @brief Create the clap_plugin_t for a given registration and host
    static const clap_plugin_t* create(const Registration& reg, const clap_host_t* host);

    /// @brief Get the ClapPlugin instance from a clap_plugin_t
    static ClapPlugin* fromPlugin(const clap_plugin_t* plugin);

    /// @brief Get the underlying AudioProcessor
    [[nodiscard]] AudioProcessor* getProcessor() noexcept { return processor_.get(); }
    [[nodiscard]] const AudioProcessor* getProcessor() const noexcept { return processor_.get(); }

private:
    ClapPlugin(const Registration& reg, const clap_host_t* host);

    // CLAP lifecycle callbacks
    static bool clapInit(const clap_plugin_t* plugin);
    static void clapDestroy(const clap_plugin_t* plugin);
    static bool clapActivate(const clap_plugin_t* plugin, double sampleRate,
                             uint32_t minFrames, uint32_t maxFrames);
    static void clapDeactivate(const clap_plugin_t* plugin);
    static bool clapStartProcessing(const clap_plugin_t* plugin);
    static void clapStopProcessing(const clap_plugin_t* plugin);
    static void clapReset(const clap_plugin_t* plugin);
    static clap_process_status clapProcess(const clap_plugin_t* plugin,
                                           const clap_process_t* process);
    static const void* clapGetExtension(const clap_plugin_t* plugin, const char* id);
    static void clapOnMainThread(const clap_plugin_t* plugin);

    std::unique_ptr<AudioProcessor> processor_;
    [[maybe_unused]] const clap_host_t* host_ = nullptr;
    clap_plugin_t plugin_ = {};
    double sampleRate_ = 44100.0;
    uint32_t maxFrames_ = 0;
};

} // namespace body
