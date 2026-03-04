// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "GainPlugin.hpp"
#include "body/formats/clap/ClapEntry.hpp"

BODY_REGISTER_CLAP_PLUGIN(
    GainPlugin,
    "com.body-framework.gain",
    "Gain",
    "BODY Examples",
    "1.0.0",
    "Simple gain plugin",
    CLAP_PLUGIN_FEATURE_AUDIO_EFFECT,
    CLAP_PLUGIN_FEATURE_UTILITY
)
