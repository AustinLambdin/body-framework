// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "SynthPlugin.hpp"
#include "body/formats/vst3/Vst3Entry.hpp"

BODY_REGISTER_VST3_PLUGIN(
    SynthPlugin,
    0x424F4459, 0x53796E74, 0x68506C75, 0x67696E31,  // "BODYSynthPlugin1"
    "Synth",
    "BODY Examples",
    "1.0.0",
    "Instrument|Synth"
)
