// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#if defined(__APPLE__)

#include "GainPlugin.hpp"
#include "body/formats/au/AUPlugin.hpp"

BODY_REGISTER_AU_PLUGIN(
    GainPlugin,
    "aufx",
    "gain",
    "Body",
    "BODY: Gain",
    "1.0.0"
)

#endif // __APPLE__
