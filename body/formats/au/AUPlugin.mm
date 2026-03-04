// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#if defined(__APPLE__)

#import "AUPlugin.hpp"
#import "body/core/StateSerializer.hpp"
#import <AudioToolbox/AudioToolbox.h>
#import <AVFoundation/AVFoundation.h>
#import <CoreAudioKit/CoreAudioKit.h>
#import "BODYEditorViewController.h"

namespace body {

AURegistry& AURegistry::instance() {
    static AURegistry registry;
    return registry;
}

void AURegistry::registerPlugin(AURegistration registration) {
    registrations_.push_back(std::move(registration));
}

const std::vector<AURegistration>& AURegistry::getRegistrations() const noexcept {
    return registrations_;
}

} // namespace body

// --- BODYAudioUnit: AUAudioUnit subclass that bridges to body::AudioProcessor ---

@interface BODYAudioUnit : AUAudioUnit {
    std::unique_ptr<body::AudioProcessor> _processor;
    AUAudioUnitBusArray* _inputBusArray;
    AUAudioUnitBusArray* _outputBusArray;
    AUParameterTree* _parameterTree;
    double _sampleRate;
    int _maxFrames;
}
@end

@implementation BODYAudioUnit

- (instancetype)initWithComponentDescription:(AudioComponentDescription)componentDescription
                                     options:(AudioComponentInstantiationOptions)options
                                       error:(NSError**)outError {
    self = [super initWithComponentDescription:componentDescription
                                       options:options
                                         error:outError];
    if (!self) return nil;

    // Find matching registration
    auto& regs = body::AURegistry::instance().getRegistrations();
    const body::AURegistration* matchedReg = nullptr;

    for (const auto& reg : regs) {
        if (reg.componentSubType.size() >= 4) {
            UInt32 subType = 0;
            subType |= static_cast<UInt32>(reg.componentSubType[0]) << 24;
            subType |= static_cast<UInt32>(reg.componentSubType[1]) << 16;
            subType |= static_cast<UInt32>(reg.componentSubType[2]) << 8;
            subType |= static_cast<UInt32>(reg.componentSubType[3]);
            if (subType == componentDescription.componentSubType) {
                matchedReg = &reg;
                break;
            }
        }
    }

    if (!matchedReg || !matchedReg->factory) {
        if (outError) {
            *outError = [NSError errorWithDomain:@"com.body-framework"
                                            code:-1
                                        userInfo:@{NSLocalizedDescriptionKey: @"No matching plugin registration"}];
        }
        return nil;
    }

    _processor = matchedReg->factory();
    if (!_processor) return nil;

    _sampleRate = 44100.0;
    _maxFrames = 1024;

    // Create AU buses matching the processor's bus layout
    auto layout = _processor->getDefaultBusLayout();

    AVAudioFormat* stereoFormat = [[AVAudioFormat alloc] initStandardFormatWithSampleRate:_sampleRate
                                                                                channels:2];

    NSMutableArray* inputs = [NSMutableArray array];
    for (size_t i = 0; i < layout.inputBuses.size(); ++i) {
        auto& bus = layout.inputBuses[i];
        AVAudioFormat* fmt = [[AVAudioFormat alloc] initStandardFormatWithSampleRate:_sampleRate
                                                                           channels:static_cast<AVAudioChannelCount>(bus.numChannels)];
        NSError* busError = nil;
        AUAudioUnitBus* auBus = [[AUAudioUnitBus alloc] initWithFormat:fmt error:&busError];
        if (auBus) [inputs addObject:auBus];
    }
    if (inputs.count == 0) {
        NSError* busError = nil;
        AUAudioUnitBus* auBus = [[AUAudioUnitBus alloc] initWithFormat:stereoFormat error:&busError];
        if (auBus) [inputs addObject:auBus];
    }

    NSMutableArray* outputs = [NSMutableArray array];
    for (size_t i = 0; i < layout.outputBuses.size(); ++i) {
        auto& bus = layout.outputBuses[i];
        AVAudioFormat* fmt = [[AVAudioFormat alloc] initStandardFormatWithSampleRate:_sampleRate
                                                                           channels:static_cast<AVAudioChannelCount>(bus.numChannels)];
        NSError* busError = nil;
        AUAudioUnitBus* auBus = [[AUAudioUnitBus alloc] initWithFormat:fmt error:&busError];
        if (auBus) [outputs addObject:auBus];
    }
    if (outputs.count == 0) {
        NSError* busError = nil;
        AUAudioUnitBus* auBus = [[AUAudioUnitBus alloc] initWithFormat:stereoFormat error:&busError];
        if (auBus) [outputs addObject:auBus];
    }

    _inputBusArray = [[AUAudioUnitBusArray alloc] initWithAudioUnit:self
                                                            busType:AUAudioUnitBusTypeInput
                                                              busses:inputs];
    _outputBusArray = [[AUAudioUnitBusArray alloc] initWithAudioUnit:self
                                                             busType:AUAudioUnitBusTypeOutput
                                                               busses:outputs];

    // Build AUParameterTree from processor parameters
    auto params = _processor->getParameters().getAllParameters();
    NSMutableArray<AUParameter*>* auParams = [NSMutableArray array];

    for (size_t i = 0; i < params.size(); ++i) {
        auto* param = params[i];
        NSString* identifier = [NSString stringWithUTF8String:param->getId().c_str()];
        NSString* name = [NSString stringWithUTF8String:param->getName().c_str()];

        AUValue minValue = 0.0f;
        AUValue maxValue = 1.0f;
        AudioUnitParameterUnit unit = kAudioUnitParameterUnit_Generic;
        AudioUnitParameterOptions flags = kAudioUnitParameterFlag_IsReadable |
                                          kAudioUnitParameterFlag_IsWritable;

        switch (param->getType()) {
            case body::Parameter::Type::Float: {
                auto* fp = static_cast<body::FloatParameter*>(param);
                minValue = fp->getMinValue();
                maxValue = fp->getMaxValue();
                break;
            }
            case body::Parameter::Type::Int: {
                auto* ip = static_cast<body::IntParameter*>(param);
                minValue = static_cast<AUValue>(ip->getMinValue());
                maxValue = static_cast<AUValue>(ip->getMaxValue());
                unit = kAudioUnitParameterUnit_Indexed;
                break;
            }
            case body::Parameter::Type::Bool:
                minValue = 0.0f;
                maxValue = 1.0f;
                unit = kAudioUnitParameterUnit_Boolean;
                break;
            case body::Parameter::Type::Choice: {
                auto* cp = static_cast<body::ChoiceParameter*>(param);
                minValue = 0.0f;
                maxValue = static_cast<AUValue>(cp->getChoices().size() - 1);
                unit = kAudioUnitParameterUnit_Indexed;
                break;
            }
        }

        AUParameter* auParam = [AUParameterTree createParameterWithIdentifier:identifier
                                                                         name:name
                                                                      address:static_cast<AUParameterAddress>(i)
                                                                          min:minValue
                                                                          max:maxValue
                                                                         unit:unit
                                                                     unitName:nil
                                                                        flags:flags
                                                                 valueStrings:nil
                                                          dependentParameters:nil];
        auParam.value = param->getDenormalized();
        [auParams addObject:auParam];
    }

    _parameterTree = [AUParameterTree createTreeWithChildren:auParams];

    // Parameter observer: host changes -> processor
    __unsafe_unretained BODYAudioUnit* weakSelf = self;
    _parameterTree.implementorValueObserver = ^(AUParameter* auParam, AUValue value) {
        auto allParams = weakSelf->_processor->getParameters().getAllParameters();
        auto address = auParam.address;
        if (address < static_cast<AUParameterAddress>(allParams.size())) {
            allParams[address]->setDenormalized(value);
        }
    };

    _parameterTree.implementorValueProvider = ^AUValue(AUParameter* auParam) {
        auto allParams = weakSelf->_processor->getParameters().getAllParameters();
        auto address = auParam.address;
        if (address < static_cast<AUParameterAddress>(allParams.size())) {
            return allParams[address]->getDenormalized();
        }
        return 0.0f;
    };

    _parameterTree.implementorStringFromValueCallback = ^NSString*(AUParameter* auParam,
                                                                   const AUValue* /*value*/) {
        auto allParams = weakSelf->_processor->getParameters().getAllParameters();
        auto address = auParam.address;
        if (address < static_cast<AUParameterAddress>(allParams.size())) {
            return [NSString stringWithUTF8String:
                allParams[address]->getDisplayValue().c_str()];
        }
        return @"";
    };

    return self;
}

- (AUAudioUnitBusArray*)inputBusses {
    return _inputBusArray;
}

- (AUAudioUnitBusArray*)outputBusses {
    return _outputBusArray;
}

- (AUParameterTree*)parameterTree {
    return _parameterTree;
}

- (BOOL)allocateRenderResourcesAndReturnError:(NSError**)outError {
    if (![super allocateRenderResourcesAndReturnError:outError]) return NO;

    _sampleRate = self.outputBusses[0].format.sampleRate;
    _maxFrames = static_cast<int>(self.maximumFramesToRender);
    _processor->prepare(_sampleRate, _maxFrames);

    return YES;
}

- (void)deallocateRenderResources {
    _processor->reset();
    [super deallocateRenderResources];
}

- (AUInternalRenderBlock)internalRenderBlock {
    // Capture raw pointer for real-time safety (no ARC in render block)
    auto* processor = _processor.get();
    double sampleRate = _sampleRate;

    AUInternalRenderBlock renderBlock =
        ^AUAudioUnitStatus(AudioUnitRenderActionFlags* actionFlags,
                           const AudioTimeStamp* timestamp,
                           AVAudioFrameCount frameCount,
                           NSInteger outputBusNumber,
                           AudioBufferList* outputData,
                           const AURenderEvent* realtimeEventListHead,
                           AURenderPullInputBlock __unsafe_unretained pullInputBlock) {
        (void)actionFlags;
        (void)outputBusNumber;

        // Pull input audio
        AudioUnitRenderActionFlags pullFlags = 0;
        AUAudioUnitStatus err = pullInputBlock(&pullFlags, timestamp, frameCount,
                                               0, outputData);
        if (err != noErr) return err;

        // Process parameter events
        const AURenderEvent* event = realtimeEventListHead;
        while (event != nullptr) {
            if (event->head.eventType == AURenderEventParameter) {
                auto& paramEvent = event->parameter;
                auto allParams = processor->getParameters().getAllParameters();
                auto address = paramEvent.parameterAddress;
                if (address < static_cast<AUParameterAddress>(allParams.size())) {
                    allParams[address]->setDenormalized(paramEvent.value);
                }
            }
            event = event->head.next;
        }

        // Build ProcessContext
        body::ProcessContext ctx;
        ctx.sampleRate = sampleRate;
        ctx.blockSize = static_cast<int>(frameCount);

        // Wrap output buffer list as AudioBuffer<float>
        int numChannels = static_cast<int>(outputData->mNumberBuffers);
        if (numChannels > 0) {
            if (outputData->mBuffers[0].mNumberChannels == 1 && numChannels > 1) {
                // Non-interleaved (typical for AU v3)
                std::vector<float*> channelPtrs(static_cast<size_t>(numChannels));
                for (int ch = 0; ch < numChannels; ++ch) {
                    channelPtrs[static_cast<size_t>(ch)] =
                        static_cast<float*>(outputData->mBuffers[ch].mData);
                }
                ctx.audioOut = body::AudioBuffer<float>(
                    channelPtrs.data(), numChannels, static_cast<int>(frameCount));
                ctx.audioIn = ctx.audioOut;
            } else {
                float* data = static_cast<float*>(outputData->mBuffers[0].mData);
                int channels = static_cast<int>(outputData->mBuffers[0].mNumberChannels);
                std::vector<float*> channelPtrs = {data};
                ctx.audioOut = body::AudioBuffer<float>(
                    channelPtrs.data(), 1, static_cast<int>(frameCount) * channels);
                ctx.audioIn = ctx.audioOut;
            }
        }

        processor->processBlock(ctx);
        return noErr;
    };
    return renderBlock;
}

- (NSDictionary<NSString*, id>*)fullState {
    auto data = body::StateSerializer::serialize(_processor->getParameters());
    NSData* stateData = [NSData dataWithBytes:data.data() length:data.size()];
    NSMutableDictionary* state = [[super fullState] mutableCopy];
    if (!state) state = [NSMutableDictionary dictionary];
    state[@"BODYState"] = stateData;
    return state;
}

- (void)setFullState:(NSDictionary<NSString*, id>*)fullState {
    [super setFullState:fullState];
    NSData* stateData = fullState[@"BODYState"];
    if (stateData && _processor) {
        body::StateSerializer::deserialize(
            _processor->getParameters(),
            static_cast<const uint8_t*>(stateData.bytes),
            static_cast<size_t>(stateData.length));
    }
}

- (void)requestViewControllerWithCompletionHandler:(void (^)(NSViewController * _Nullable))completionHandler {
    if (!_processor || !completionHandler) {
        if (completionHandler) completionHandler(nil);
        return;
    }

    auto editor = _processor->createEditor();
    if (!editor) {
        completionHandler(nil);
        return;
    }

    BODYEditorViewController* vc = [[BODYEditorViewController alloc] initWithEditor:std::move(editor)];
    completionHandler(vc);
}

@end

#endif // __APPLE__
