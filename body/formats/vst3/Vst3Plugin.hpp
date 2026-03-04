// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include "body/core/AudioProcessor.hpp"
#include "public.sdk/source/vst/vstsinglecomponenteffect.h"
#include <memory>

namespace body {

/// @brief Bridges a body::AudioProcessor to the VST3 SingleComponentEffect lifecycle
///
/// Implements initialize (creates processor, registers VST3 parameters),
/// process (wraps AudioBusBuffers, reads IParameterChanges, calls processBlock),
/// setState/getState (uses StateSerializer via IBStream).
class Vst3Plugin : public Steinberg::Vst::SingleComponentEffect {
public:
    Vst3Plugin();

    /// @brief Set the factory function used to create the AudioProcessor
    void setFactory(AudioProcessorFactory factory);

    // IPluginBase
    Steinberg::tresult PLUGIN_API initialize(Steinberg::FUnknown* context) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API terminate() SMTG_OVERRIDE;

    // IAudioProcessor
    Steinberg::tresult PLUGIN_API setupProcessing(
        Steinberg::Vst::ProcessSetup& setup) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API setActive(Steinberg::TBool state) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API process(
        Steinberg::Vst::ProcessData& data) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API canProcessSampleSize(
        Steinberg::int32 symbolicSampleSize) SMTG_OVERRIDE;

    // IComponent state
    Steinberg::tresult PLUGIN_API setState(Steinberg::IBStream* state) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API getState(Steinberg::IBStream* state) SMTG_OVERRIDE;

    // IEditController state (called by host after setState)
    Steinberg::tresult PLUGIN_API setEditorState(Steinberg::IBStream* state) SMTG_OVERRIDE;
    Steinberg::tresult PLUGIN_API getEditorState(Steinberg::IBStream* state) SMTG_OVERRIDE;

    // IEditController GUI
    Steinberg::IPlugView* PLUGIN_API createView(Steinberg::FIDString name) SMTG_OVERRIDE;

    static Steinberg::FUnknown* createInstance(void* context);

private:
    AudioProcessorFactory factory_;
    std::unique_ptr<AudioProcessor> processor_;
    double sampleRate_ = 44100.0;
    int maxBlockSize_ = 0;
};

} // namespace body
