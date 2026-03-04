// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "SynthPlugin.hpp"
#include "body/formats/clap/ClapEntry.hpp"

BODY_REGISTER_CLAP_PLUGIN(
    SynthPlugin,
    "com.body-framework.synth",
    "Synth",
    "BODY Examples",
    "1.0.0",
    "Monophonic subtractive synthesizer",
    CLAP_PLUGIN_FEATURE_INSTRUMENT,
    CLAP_PLUGIN_FEATURE_SYNTHESIZER
)
