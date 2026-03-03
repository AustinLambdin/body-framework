// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "ClapExtensions.hpp"
#include "ClapPlugin.hpp"
#include <cstring>

namespace body {

// --- Params Extension ---

const clap_plugin_params_t* ClapExtensions::getParamsExtension() {
    static const clap_plugin_params_t ext = {
        paramsCount,
        paramsGetInfo,
        paramsGetValue,
        paramsValueToText,
        paramsTextToValue,
        paramsFlush
    };
    return &ext;
}

uint32_t ClapExtensions::paramsCount(const clap_plugin_t* plugin) {
    auto* proc = ClapPlugin::fromPlugin(plugin)->getProcessor();
    return static_cast<uint32_t>(proc->getParameters().size());
}

bool ClapExtensions::paramsGetInfo(const clap_plugin_t* plugin, uint32_t paramIndex,
                                    clap_param_info_t* paramInfo) {
    auto* proc = ClapPlugin::fromPlugin(plugin)->getProcessor();
    auto params = proc->getParameters().getAllParameters();

    if (paramIndex >= static_cast<uint32_t>(params.size())) return false;

    auto* param = params[paramIndex];
    std::memset(paramInfo, 0, sizeof(*paramInfo));

    paramInfo->id = paramIndex;
    paramInfo->cookie = param;

    strncpy(paramInfo->name, param->getName().c_str(), CLAP_NAME_SIZE - 1);
    paramInfo->name[CLAP_NAME_SIZE - 1] = '\0';
    paramInfo->module[0] = '\0';

    paramInfo->flags = CLAP_PARAM_IS_AUTOMATABLE;

    switch (param->getType()) {
        case Parameter::Type::Float: {
            auto* fp = static_cast<FloatParameter*>(param);
            paramInfo->min_value = fp->getMinValue();
            paramInfo->max_value = fp->getMaxValue();
            paramInfo->default_value = fp->getDefaultValue();
            break;
        }
        case Parameter::Type::Int: {
            auto* ip = static_cast<IntParameter*>(param);
            paramInfo->flags |= CLAP_PARAM_IS_STEPPED;
            paramInfo->min_value = ip->getMinValue();
            paramInfo->max_value = ip->getMaxValue();
            paramInfo->default_value = ip->getDefaultValue();
            break;
        }
        case Parameter::Type::Bool: {
            paramInfo->flags |= CLAP_PARAM_IS_STEPPED;
            paramInfo->min_value = 0.0;
            paramInfo->max_value = 1.0;
            paramInfo->default_value = param->getDenormalized();
            break;
        }
        case Parameter::Type::Choice: {
            auto* cp = static_cast<ChoiceParameter*>(param);
            paramInfo->flags |= CLAP_PARAM_IS_STEPPED | CLAP_PARAM_IS_ENUM;
            paramInfo->min_value = 0.0;
            paramInfo->max_value = static_cast<double>(cp->getChoices().size()) - 1.0;
            paramInfo->default_value = param->getDenormalized();
            break;
        }
    }

    return true;
}

bool ClapExtensions::paramsGetValue(const clap_plugin_t* plugin, clap_id paramId,
                                     double* outValue) {
    auto* proc = ClapPlugin::fromPlugin(plugin)->getProcessor();
    auto params = proc->getParameters().getAllParameters();

    if (paramId >= static_cast<clap_id>(params.size())) return false;

    *outValue = params[paramId]->getDenormalized();
    return true;
}

bool ClapExtensions::paramsValueToText(const clap_plugin_t* plugin, clap_id paramId,
                                        double /*value*/, char* outBuffer,
                                        uint32_t outBufferCapacity) {
    auto* proc = ClapPlugin::fromPlugin(plugin)->getProcessor();
    auto params = proc->getParameters().getAllParameters();

    if (paramId >= static_cast<clap_id>(params.size())) return false;

    auto text = params[paramId]->getDisplayValue();
    strncpy(outBuffer, text.c_str(), outBufferCapacity - 1);
    outBuffer[outBufferCapacity - 1] = '\0';
    return true;
}

bool ClapExtensions::paramsTextToValue(const clap_plugin_t* /*plugin*/, clap_id /*paramId*/,
                                        const char* /*paramValueText*/, double* /*outValue*/) {
    // Not implemented for now
    return false;
}

void ClapExtensions::paramsFlush(const clap_plugin_t* plugin, const clap_input_events_t* in,
                                  const clap_output_events_t* /*out*/) {
    auto* proc = ClapPlugin::fromPlugin(plugin)->getProcessor();

    if (in == nullptr) return;

    uint32_t numEvents = in->size(in);
    for (uint32_t i = 0; i < numEvents; ++i) {
        auto* header = in->get(in, i);
        if (header->space_id != CLAP_CORE_EVENT_SPACE_ID) continue;

        if (header->type == CLAP_EVENT_PARAM_VALUE) {
            auto* ev = reinterpret_cast<const clap_event_param_value_t*>(header);
            auto params = proc->getParameters().getAllParameters();
            if (ev->param_id < static_cast<clap_id>(params.size())) {
                params[ev->param_id]->setDenormalized(static_cast<float>(ev->value));
            }
        }
    }
}

// --- Audio Ports Extension ---

const clap_plugin_audio_ports_t* ClapExtensions::getAudioPortsExtension() {
    static const clap_plugin_audio_ports_t ext = {
        audioPortsCount,
        audioPortsGet
    };
    return &ext;
}

uint32_t ClapExtensions::audioPortsCount(const clap_plugin_t* plugin, bool isInput) {
    auto* proc = ClapPlugin::fromPlugin(plugin)->getProcessor();
    auto layout = proc->getDefaultBusLayout();
    return static_cast<uint32_t>(isInput ? layout.inputBuses.size() : layout.outputBuses.size());
}

bool ClapExtensions::audioPortsGet(const clap_plugin_t* plugin, uint32_t index, bool isInput,
                                    clap_audio_port_info_t* info) {
    auto* proc = ClapPlugin::fromPlugin(plugin)->getProcessor();
    auto layout = proc->getDefaultBusLayout();

    const auto& buses = isInput ? layout.inputBuses : layout.outputBuses;
    if (index >= static_cast<uint32_t>(buses.size())) return false;

    const auto& bus = buses[index];
    std::memset(info, 0, sizeof(*info));

    info->id = index;
    strncpy(info->name, bus.name.c_str(), CLAP_NAME_SIZE - 1);
    info->name[CLAP_NAME_SIZE - 1] = '\0';
    info->channel_count = static_cast<uint32_t>(bus.numChannels);
    info->flags = (index == 0) ? CLAP_AUDIO_PORT_IS_MAIN : 0;
    info->port_type = (bus.numChannels == 2) ? CLAP_PORT_STEREO
                    : (bus.numChannels == 1) ? CLAP_PORT_MONO
                    : nullptr;
    info->in_place_pair = isInput ? index : CLAP_INVALID_ID;

    return true;
}

// --- Note Ports Extension ---

const clap_plugin_note_ports_t* ClapExtensions::getNotePortsExtension() {
    static const clap_plugin_note_ports_t ext = {
        notePortsCount,
        notePortsGet
    };
    return &ext;
}

uint32_t ClapExtensions::notePortsCount(const clap_plugin_t* /*plugin*/, bool isInput) {
    return isInput ? 1 : 0;
}

bool ClapExtensions::notePortsGet(const clap_plugin_t* /*plugin*/, uint32_t index, bool isInput,
                                   clap_note_port_info_t* info) {
    if (!isInput || index != 0) return false;

    std::memset(info, 0, sizeof(*info));
    info->id = 0;
    info->supported_dialects = CLAP_NOTE_DIALECT_CLAP | CLAP_NOTE_DIALECT_MIDI;
    info->preferred_dialect = CLAP_NOTE_DIALECT_CLAP;
    strncpy(info->name, "MIDI In", CLAP_NAME_SIZE - 1);
    info->name[CLAP_NAME_SIZE - 1] = '\0';

    return true;
}

} // namespace body
