# MIT License
# Copyright (c) 2025 BODY Framework Contributors

# Shared plugin source (no format-specific code)
set(SYNTH_PLUGIN_SOURCES SynthPlugin.cpp SynthEditor.cpp)

# --- CLAP ---
add_library(SynthPlugin_CLAP MODULE
    ${SYNTH_PLUGIN_SOURCES}
    SynthPlugin_CLAP.cpp
)
target_link_libraries(SynthPlugin_CLAP PRIVATE body::clap body::dsp)
set_target_properties(SynthPlugin_CLAP PROPERTIES
    PREFIX ""
    SUFFIX ".clap"
)

# --- VST3 ---
if(BODY_ENABLE_VST3)
    add_library(SynthPlugin_VST3 MODULE
        ${SYNTH_PLUGIN_SOURCES}
        SynthPlugin_VST3.cpp
    )
    target_link_libraries(SynthPlugin_VST3 PRIVATE body::vst3 body::dsp)

    # Platform-specific entry point for VST3
    if(APPLE)
        target_sources(SynthPlugin_VST3 PRIVATE
            ${CMAKE_SOURCE_DIR}/third_party/vst3sdk/public.sdk/source/main/macmain.cpp
        )
    elseif(WIN32)
        target_sources(SynthPlugin_VST3 PRIVATE
            ${CMAKE_SOURCE_DIR}/third_party/vst3sdk/public.sdk/source/main/dllmain.cpp
        )
    else()
        target_sources(SynthPlugin_VST3 PRIVATE
            ${CMAKE_SOURCE_DIR}/third_party/vst3sdk/public.sdk/source/main/linuxmain.cpp
        )
    endif()

    if(APPLE)
        target_link_libraries(SynthPlugin_VST3 PRIVATE
            "-framework CoreFoundation"
            "-framework Cocoa"
        )
    endif()

    set_target_properties(SynthPlugin_VST3 PROPERTIES
        PREFIX ""
        SUFFIX ".vst3"
    )
endif()

# --- AU (macOS only) ---
if(APPLE AND BODY_ENABLE_AU)
    add_library(SynthPlugin_AU MODULE
        ${SYNTH_PLUGIN_SOURCES}
        SynthPlugin_AU.mm
    )
    target_link_libraries(SynthPlugin_AU PRIVATE body::au body::dsp)
    set_target_properties(SynthPlugin_AU PROPERTIES
        PREFIX ""
        SUFFIX ".component"
        BUNDLE TRUE
        BUNDLE_EXTENSION "component"
    )
endif()
