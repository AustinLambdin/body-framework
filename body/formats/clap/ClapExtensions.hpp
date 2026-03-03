// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include <clap/clap.h>

namespace body {

/// @brief Implements CLAP extension interfaces for params, audio ports, and note ports
class ClapExtensions {
public:
    /// @brief Get the clap_plugin_params extension
    [[nodiscard]] static const clap_plugin_params_t* getParamsExtension();

    /// @brief Get the clap_plugin_audio_ports extension
    [[nodiscard]] static const clap_plugin_audio_ports_t* getAudioPortsExtension();

    /// @brief Get the clap_plugin_note_ports extension
    [[nodiscard]] static const clap_plugin_note_ports_t* getNotePortsExtension();

private:
    // Params extension callbacks
    static uint32_t paramsCount(const clap_plugin_t* plugin);
    static bool paramsGetInfo(const clap_plugin_t* plugin, uint32_t paramIndex,
                              clap_param_info_t* paramInfo);
    static bool paramsGetValue(const clap_plugin_t* plugin, clap_id paramId, double* outValue);
    static bool paramsValueToText(const clap_plugin_t* plugin, clap_id paramId, double value,
                                   char* outBuffer, uint32_t outBufferCapacity);
    static bool paramsTextToValue(const clap_plugin_t* plugin, clap_id paramId,
                                   const char* paramValueText, double* outValue);
    static void paramsFlush(const clap_plugin_t* plugin, const clap_input_events_t* in,
                             const clap_output_events_t* out);

    // Audio ports extension callbacks
    static uint32_t audioPortsCount(const clap_plugin_t* plugin, bool isInput);
    static bool audioPortsGet(const clap_plugin_t* plugin, uint32_t index, bool isInput,
                               clap_audio_port_info_t* info);

    // Note ports extension callbacks
    static uint32_t notePortsCount(const clap_plugin_t* plugin, bool isInput);
    static bool notePortsGet(const clap_plugin_t* plugin, uint32_t index, bool isInput,
                              clap_note_port_info_t* info);
};

} // namespace body
