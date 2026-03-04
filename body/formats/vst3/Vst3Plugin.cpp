// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "Vst3Plugin.hpp"
#include "body/core/StateSerializer.hpp"
#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/ivstprocesscontext.h"
#include "pluginterfaces/vst/vstspeaker.h"
#include "base/source/fstring.h"
#include <cstring>

#include "body/gui/PluginEditor.hpp"

#if defined(__APPLE__)
#include "Vst3EditorView.hpp"
#endif

namespace body {

Vst3Plugin::Vst3Plugin() = default;

void Vst3Plugin::setFactory(AudioProcessorFactory factory) {
    factory_ = std::move(factory);
}

Steinberg::FUnknown* Vst3Plugin::createInstance(void* /*context*/) {
    auto* plugin = new Vst3Plugin();
    return static_cast<Steinberg::FUnknown*>(
        static_cast<Steinberg::Vst::IComponent*>(plugin));
}

Steinberg::tresult PLUGIN_API Vst3Plugin::initialize(Steinberg::FUnknown* context) {
    auto result = SingleComponentEffect::initialize(context);
    if (result != Steinberg::kResultOk)
        return result;

    if (!factory_)
        return Steinberg::kResultFalse;

    processor_ = factory_();
    if (!processor_)
        return Steinberg::kResultFalse;

    // Add audio buses from the processor's default bus layout
    auto layout = processor_->getDefaultBusLayout();
    for (size_t i = 0; i < layout.inputBuses.size(); ++i) {
        auto& bus = layout.inputBuses[i];
        Steinberg::Vst::SpeakerArrangement arr =
            (bus.numChannels == 2) ? Steinberg::Vst::SpeakerArr::kStereo
                                  : Steinberg::Vst::SpeakerArr::kMono;
        addAudioInput(
            STR16("Input"),
            arr,
            (i == 0) ? Steinberg::Vst::kMain : Steinberg::Vst::kAux);
    }

    for (size_t i = 0; i < layout.outputBuses.size(); ++i) {
        auto& bus = layout.outputBuses[i];
        Steinberg::Vst::SpeakerArrangement arr =
            (bus.numChannels == 2) ? Steinberg::Vst::SpeakerArr::kStereo
                                  : Steinberg::Vst::SpeakerArr::kMono;
        addAudioOutput(
            STR16("Output"),
            arr,
            (i == 0) ? Steinberg::Vst::kMain : Steinberg::Vst::kAux);
    }

    // Add event input for MIDI
    addEventInput(STR16("MIDI In"));

    // Register parameters from the processor's ParameterGroup
    auto params = processor_->getParameters().getAllParameters();
    for (size_t i = 0; i < params.size(); ++i) {
        auto* param = params[i];
        auto paramId = static_cast<Steinberg::Vst::ParamID>(i);

        Steinberg::int32 stepCount = 0;
        Steinberg::Vst::ParamValue defaultNorm = 0.0;

        switch (param->getType()) {
            case Parameter::Type::Float: {
                stepCount = 0; // continuous
                auto* fp = static_cast<FloatParameter*>(param);
                float range = fp->getMaxValue() - fp->getMinValue();
                if (range > 0.0f) {
                    defaultNorm = static_cast<double>(
                        (fp->getDefaultValue() - fp->getMinValue()) / range);
                }
                break;
            }
            case Parameter::Type::Int: {
                auto* ip = static_cast<IntParameter*>(param);
                stepCount = ip->getMaxValue() - ip->getMinValue();
                if (stepCount > 0) {
                    defaultNorm = static_cast<double>(
                        ip->getDefaultValue() - ip->getMinValue()) / stepCount;
                }
                break;
            }
            case Parameter::Type::Bool:
                stepCount = 1;
                defaultNorm = param->getDenormalized() > 0.5f ? 1.0 : 0.0;
                break;
            case Parameter::Type::Choice: {
                auto* cp = static_cast<ChoiceParameter*>(param);
                stepCount = static_cast<Steinberg::int32>(cp->getChoices().size()) - 1;
                if (stepCount > 0) {
                    defaultNorm = param->getDenormalized() / stepCount;
                }
                break;
            }
        }

        Steinberg::int32 flags = Steinberg::Vst::ParameterInfo::kCanAutomate;

        // Convert name to String128
        Steinberg::Vst::String128 nameStr{};
        auto name = param->getName();
        for (size_t c = 0; c < name.size() && c < 127; ++c) {
            nameStr[c] = static_cast<Steinberg::char16>(name[c]);
        }

        parameters.addParameter(nameStr, nullptr, stepCount, defaultNorm, flags, paramId);
    }

    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Vst3Plugin::terminate() {
    processor_.reset();
    return SingleComponentEffect::terminate();
}

Steinberg::tresult PLUGIN_API Vst3Plugin::setupProcessing(
    Steinberg::Vst::ProcessSetup& setup) {
    sampleRate_ = setup.sampleRate;
    maxBlockSize_ = setup.maxSamplesPerBlock;
    return SingleComponentEffect::setupProcessing(setup);
}

Steinberg::tresult PLUGIN_API Vst3Plugin::setActive(Steinberg::TBool state) {
    if (state && processor_) {
        processor_->prepare(sampleRate_, maxBlockSize_);
    } else if (!state && processor_) {
        processor_->reset();
    }
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Vst3Plugin::canProcessSampleSize(
    Steinberg::int32 symbolicSampleSize) {
    return (symbolicSampleSize == Steinberg::Vst::kSample32)
        ? Steinberg::kResultTrue
        : Steinberg::kResultFalse;
}

Steinberg::tresult PLUGIN_API Vst3Plugin::process(Steinberg::Vst::ProcessData& data) {
    if (!processor_)
        return Steinberg::kResultFalse;

    // Read parameter changes from the host
    if (data.inputParameterChanges) {
        Steinberg::int32 numParamsChanged = data.inputParameterChanges->getParameterCount();
        auto allParams = processor_->getParameters().getAllParameters();

        for (Steinberg::int32 i = 0; i < numParamsChanged; ++i) {
            auto* queue = data.inputParameterChanges->getParameterData(i);
            if (!queue) continue;

            auto paramId = queue->getParameterId();
            Steinberg::int32 numPoints = queue->getPointCount();
            if (numPoints <= 0) continue;

            // Use the last point value
            Steinberg::int32 sampleOffset = 0;
            Steinberg::Vst::ParamValue value = 0.0;
            if (queue->getPoint(numPoints - 1, sampleOffset, value) == Steinberg::kResultOk) {
                if (paramId < static_cast<Steinberg::Vst::ParamID>(allParams.size())) {
                    allParams[paramId]->setNormalized(static_cast<float>(value));
                }
            }
        }
    }

    // Build ProcessContext
    ProcessContext ctx;
    ctx.sampleRate = sampleRate_;
    ctx.blockSize = data.numSamples;

    // Wrap input audio
    if (data.numInputs > 0 && data.inputs[0].channelBuffers32 != nullptr) {
        ctx.audioIn = AudioBuffer<float>(
            data.inputs[0].channelBuffers32,
            data.inputs[0].numChannels,
            data.numSamples);
    }

    // Wrap output audio
    if (data.numOutputs > 0 && data.outputs[0].channelBuffers32 != nullptr) {
        ctx.audioOut = AudioBuffer<float>(
            data.outputs[0].channelBuffers32,
            data.outputs[0].numChannels,
            data.numSamples);
    }

    // Copy input to output if different buffers
    if (ctx.audioIn.getNumChannels() > 0 && ctx.audioOut.getNumChannels() > 0) {
        int channels = std::min(ctx.audioIn.getNumChannels(), ctx.audioOut.getNumChannels());
        for (int ch = 0; ch < channels; ++ch) {
            auto inSpan = ctx.audioIn.getChannel(ch);
            auto* outPtr = ctx.audioOut.getChannelPointer(ch);
            if (inSpan.data() != outPtr) {
                std::memcpy(outPtr, inSpan.data(),
                           static_cast<size_t>(data.numSamples) * sizeof(float));
            }
        }
    }

    // Transport info
    if (data.processContext) {
        ctx.sampleRate = data.processContext->sampleRate;
        ctx.samplePosition = data.processContext->projectTimeSamples;
        ctx.isPlaying = (data.processContext->state &
                        Steinberg::Vst::ProcessContext::kPlaying) != 0;
        ctx.isRecording = (data.processContext->state &
                          Steinberg::Vst::ProcessContext::kRecording) != 0;
        if (data.processContext->state & Steinberg::Vst::ProcessContext::kTempoValid) {
            ctx.bpm = data.processContext->tempo;
        }
    }

    processor_->processBlock(ctx);
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Vst3Plugin::setState(Steinberg::IBStream* state) {
    if (!state || !processor_)
        return Steinberg::kResultFalse;

    // Read all data from the stream
    std::vector<uint8_t> data;
    uint8_t buffer[4096];

    for (;;) {
        Steinberg::int32 bytesRead = 0;
        auto result = state->read(buffer, sizeof(buffer), &bytesRead);
        if (bytesRead <= 0) break;
        data.insert(data.end(), buffer, buffer + bytesRead);
        if (result != Steinberg::kResultOk) break;
    }

    if (!StateSerializer::deserialize(processor_->getParameters(), data.data(), data.size()))
        return Steinberg::kResultFalse;

    // Sync VST3 parameter values with restored plugin state
    auto allParams = processor_->getParameters().getAllParameters();
    for (size_t i = 0; i < allParams.size(); ++i) {
        auto paramId = static_cast<Steinberg::Vst::ParamID>(i);
        setParamNormalized(paramId, static_cast<double>(allParams[i]->getNormalized()));
    }

    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Vst3Plugin::getState(Steinberg::IBStream* state) {
    if (!state || !processor_)
        return Steinberg::kResultFalse;

    auto data = StateSerializer::serialize(processor_->getParameters());

    Steinberg::int32 bytesWritten = 0;
    state->write(data.data(), static_cast<Steinberg::int32>(data.size()), &bytesWritten);

    return (bytesWritten == static_cast<Steinberg::int32>(data.size()))
        ? Steinberg::kResultOk
        : Steinberg::kResultFalse;
}

Steinberg::tresult PLUGIN_API Vst3Plugin::setEditorState(Steinberg::IBStream* /*state*/) {
    return Steinberg::kResultOk;
}

Steinberg::tresult PLUGIN_API Vst3Plugin::getEditorState(Steinberg::IBStream* /*state*/) {
    return Steinberg::kResultOk;
}

Steinberg::IPlugView* PLUGIN_API Vst3Plugin::createView(Steinberg::FIDString name) {
    if (!processor_) return nullptr;
    if (strcmp(name, Steinberg::Vst::ViewType::kEditor) != 0) return nullptr;

#if defined(__APPLE__)
    auto editor = processor_->createEditor();
    if (!editor) return nullptr;
    return new Vst3EditorView(*processor_);
#else
    return nullptr;
#endif
}

} // namespace body
