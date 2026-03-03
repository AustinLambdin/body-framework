// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include "AudioBuffer.hpp"
#include "MidiBuffer.hpp"
#include <cstdint>

namespace body {

/// @brief All data passed to AudioProcessor::processBlock() for one block
struct ProcessContext {
    // Audio buffers
    AudioBuffer<float> audioIn;
    AudioBuffer<float> audioOut;

    // MIDI
    MidiBuffer midiIn;
    MidiBuffer midiOut;

    // Transport
    double   sampleRate     = 44100.0;
    int      blockSize      = 0;
    bool     isPlaying      = false;
    bool     isRecording    = false;
    double   bpm            = 120.0;
    double   ppqPosition    = 0.0;
    int64_t  samplePosition = 0;

    // Timing
    bool isRealtime = true;
};

} // namespace body
