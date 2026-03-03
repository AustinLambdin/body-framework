// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "ClapPlugin.hpp"
#include "ClapExtensions.hpp"
#include <cstring>

namespace body {

ClapPlugin::ClapPlugin(const Registration& reg, const clap_host_t* host)
    : host_(host) {
    processor_ = reg.factory();

    plugin_.desc = &reg.descriptor;
    plugin_.plugin_data = this;
    plugin_.init = clapInit;
    plugin_.destroy = clapDestroy;
    plugin_.activate = clapActivate;
    plugin_.deactivate = clapDeactivate;
    plugin_.start_processing = clapStartProcessing;
    plugin_.stop_processing = clapStopProcessing;
    plugin_.reset = clapReset;
    plugin_.process = clapProcess;
    plugin_.get_extension = clapGetExtension;
    plugin_.on_main_thread = clapOnMainThread;
}

const clap_plugin_t* ClapPlugin::create(const Registration& reg, const clap_host_t* host) {
    auto* instance = new ClapPlugin(reg, host);
    return &instance->plugin_;
}

ClapPlugin* ClapPlugin::fromPlugin(const clap_plugin_t* plugin) {
    return static_cast<ClapPlugin*>(plugin->plugin_data);
}

bool ClapPlugin::clapInit(const clap_plugin_t* /*plugin*/) {
    return true;
}

void ClapPlugin::clapDestroy(const clap_plugin_t* plugin) {
    delete fromPlugin(plugin);
}

bool ClapPlugin::clapActivate(const clap_plugin_t* plugin, double sampleRate,
                               uint32_t /*minFrames*/, uint32_t maxFrames) {
    auto* self = fromPlugin(plugin);
    self->sampleRate_ = sampleRate;
    self->maxFrames_ = maxFrames;
    self->processor_->prepare(sampleRate, static_cast<int>(maxFrames));
    return true;
}

void ClapPlugin::clapDeactivate(const clap_plugin_t* /*plugin*/) {
}

bool ClapPlugin::clapStartProcessing(const clap_plugin_t* /*plugin*/) {
    return true;
}

void ClapPlugin::clapStopProcessing(const clap_plugin_t* /*plugin*/) {
}

void ClapPlugin::clapReset(const clap_plugin_t* plugin) {
    fromPlugin(plugin)->processor_->reset();
}

clap_process_status ClapPlugin::clapProcess(const clap_plugin_t* plugin,
                                             const clap_process_t* process) {
    auto* self = fromPlugin(plugin);
    auto* proc = self->processor_.get();
    auto numFrames = static_cast<int>(process->frames_count);

    // Process input events (parameter changes and MIDI)
    ProcessContext ctx;
    ctx.sampleRate = self->sampleRate_;
    ctx.blockSize = numFrames;

    // Wrap input audio as non-owning AudioBuffer
    if (process->audio_inputs_count > 0 && process->audio_inputs[0].data32 != nullptr) {
        auto& inBuf = process->audio_inputs[0];
        ctx.audioIn = AudioBuffer<float>(inBuf.data32,
                                         static_cast<int>(inBuf.channel_count),
                                         numFrames);
    }

    // Wrap output audio as non-owning AudioBuffer
    if (process->audio_outputs_count > 0 && process->audio_outputs[0].data32 != nullptr) {
        auto& outBuf = process->audio_outputs[0];
        ctx.audioOut = AudioBuffer<float>(outBuf.data32,
                                          static_cast<int>(outBuf.channel_count),
                                          numFrames);
    }

    // Copy input to output if they are different buffers
    if (ctx.audioIn.getNumChannels() > 0 && ctx.audioOut.getNumChannels() > 0) {
        int channels = std::min(ctx.audioIn.getNumChannels(), ctx.audioOut.getNumChannels());
        for (int ch = 0; ch < channels; ++ch) {
            auto inSpan = ctx.audioIn.getChannel(ch);
            auto* outPtr = ctx.audioOut.getChannelPointer(ch);
            if (inSpan.data() != outPtr) {
                std::memcpy(outPtr, inSpan.data(),
                           static_cast<size_t>(numFrames) * sizeof(float));
            }
        }
    }

    // Process CLAP input events
    if (process->in_events != nullptr) {
        uint32_t numEvents = process->in_events->size(process->in_events);
        for (uint32_t i = 0; i < numEvents; ++i) {
            auto* header = process->in_events->get(process->in_events, i);
            if (header->space_id != CLAP_CORE_EVENT_SPACE_ID) continue;

            switch (header->type) {
                case CLAP_EVENT_PARAM_VALUE: {
                    auto* ev = reinterpret_cast<const clap_event_param_value_t*>(header);
                    auto params = proc->getParameters().getAllParameters();
                    if (ev->param_id < static_cast<clap_id>(params.size())) {
                        params[ev->param_id]->setDenormalized(static_cast<float>(ev->value));
                    }
                    break;
                }
                case CLAP_EVENT_NOTE_ON: {
                    auto* ev = reinterpret_cast<const clap_event_note_t*>(header);
                    ctx.midiIn.addMessage(MidiMessage::noteOn(
                        static_cast<uint8_t>(ev->channel),
                        static_cast<uint8_t>(ev->key),
                        static_cast<uint8_t>(ev->velocity * 127.0),
                        static_cast<int>(header->time)));
                    break;
                }
                case CLAP_EVENT_NOTE_OFF: {
                    auto* ev = reinterpret_cast<const clap_event_note_t*>(header);
                    ctx.midiIn.addMessage(MidiMessage::noteOff(
                        static_cast<uint8_t>(ev->channel),
                        static_cast<uint8_t>(ev->key),
                        static_cast<uint8_t>(ev->velocity * 127.0),
                        static_cast<int>(header->time)));
                    break;
                }
                case CLAP_EVENT_MIDI: {
                    auto* ev = reinterpret_cast<const clap_event_midi_t*>(header);
                    MidiMessage msg;
                    msg.data[0] = ev->data[0];
                    msg.data[1] = ev->data[1];
                    msg.data[2] = ev->data[2];
                    msg.numBytes = 3;
                    msg.samplePosition = static_cast<int>(header->time);
                    ctx.midiIn.addMessage(msg);
                    break;
                }
                case CLAP_EVENT_TRANSPORT: {
                    auto* ev = reinterpret_cast<const clap_event_transport_t*>(header);
                    if (ev->flags & CLAP_TRANSPORT_HAS_TEMPO) {
                        ctx.bpm = ev->tempo;
                    }
                    ctx.isPlaying = (ev->flags & CLAP_TRANSPORT_IS_PLAYING) != 0;
                    ctx.isRecording = (ev->flags & CLAP_TRANSPORT_IS_RECORDING) != 0;
                    break;
                }
                default:
                    break;
            }
        }
    }

    // Transport from process
    if (process->transport != nullptr) {
        if (process->transport->flags & CLAP_TRANSPORT_HAS_TEMPO) {
            ctx.bpm = process->transport->tempo;
        }
        ctx.isPlaying = (process->transport->flags & CLAP_TRANSPORT_IS_PLAYING) != 0;
        ctx.isRecording = (process->transport->flags & CLAP_TRANSPORT_IS_RECORDING) != 0;
    }

    ctx.samplePosition = process->steady_time;

    // Call the plugin's processBlock
    proc->processBlock(ctx);

    return CLAP_PROCESS_CONTINUE;
}

const void* ClapPlugin::clapGetExtension(const clap_plugin_t* /*plugin*/, const char* id) {
    if (strcmp(id, CLAP_EXT_PARAMS) == 0)
        return ClapExtensions::getParamsExtension();
    if (strcmp(id, CLAP_EXT_AUDIO_PORTS) == 0)
        return ClapExtensions::getAudioPortsExtension();
    if (strcmp(id, CLAP_EXT_NOTE_PORTS) == 0)
        return ClapExtensions::getNotePortsExtension();
    return nullptr;
}

void ClapPlugin::clapOnMainThread(const clap_plugin_t* /*plugin*/) {
}

} // namespace body
