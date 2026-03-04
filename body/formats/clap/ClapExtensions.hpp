// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include <clap/clap.h>

namespace body {

/// @brief Implements CLAP extension interfaces for params, audio ports, note ports, state, and GUI
class ClapExtensions {
public:
    /// @brief Get the clap_plugin_params extension
    [[nodiscard]] static const clap_plugin_params_t* getParamsExtension();

    /// @brief Get the clap_plugin_audio_ports extension
    [[nodiscard]] static const clap_plugin_audio_ports_t* getAudioPortsExtension();

    /// @brief Get the clap_plugin_note_ports extension
    [[nodiscard]] static const clap_plugin_note_ports_t* getNotePortsExtension();

    /// @brief Get the clap_plugin_state extension
    [[nodiscard]] static const clap_plugin_state_t* getStateExtension();

#if defined(__APPLE__)
    /// @brief Get the clap_plugin_gui extension
    [[nodiscard]] static const clap_plugin_gui_t* getGuiExtension();
#endif

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

    // State extension callbacks
    static bool stateSave(const clap_plugin_t* plugin, const clap_ostream_t* stream);
    static bool stateLoad(const clap_plugin_t* plugin, const clap_istream_t* stream);

#if defined(__APPLE__)
    // GUI extension callbacks
    static bool guiIsApiSupported(const clap_plugin_t* plugin, const char* api, bool isFloating);
    static bool guiCreate(const clap_plugin_t* plugin, const char* api, bool isFloating);
    static void guiDestroy(const clap_plugin_t* plugin);
    static bool guiSetScale(const clap_plugin_t* plugin, double scale);
    static bool guiGetSize(const clap_plugin_t* plugin, uint32_t* width, uint32_t* height);
    static bool guiCanResize(const clap_plugin_t* plugin);
    static bool guiAdjustSize(const clap_plugin_t* plugin, uint32_t* width, uint32_t* height);
    static bool guiSetSize(const clap_plugin_t* plugin, uint32_t width, uint32_t height);
    static bool guiSetParent(const clap_plugin_t* plugin, const clap_window_t* window);
    static bool guiSetTransient(const clap_plugin_t* plugin, const clap_window_t* window);
    static void guiSuggestTitle(const clap_plugin_t* plugin, const char* title);
    static bool guiShow(const clap_plugin_t* plugin);
    static bool guiHide(const clap_plugin_t* plugin);
#endif
};

} // namespace body
