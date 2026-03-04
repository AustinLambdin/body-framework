// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#if defined(__APPLE__)

#include "SynthPlugin.hpp"
#include "body/formats/au/AUPlugin.hpp"

BODY_REGISTER_AU_PLUGIN(
    SynthPlugin,
    "aumu",
    "synt",
    "Body",
    "BODY: Synth",
    "1.0.0"
)

#endif // __APPLE__
