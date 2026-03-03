# MIT License
# Copyright (c) 2025 BODY Framework Contributors

add_library(GainPlugin_CLAP MODULE
    GainPlugin.cpp
)
target_link_libraries(GainPlugin_CLAP PRIVATE body::clap)
set_target_properties(GainPlugin_CLAP PROPERTIES
    PREFIX ""
    SUFFIX ".clap"
)
