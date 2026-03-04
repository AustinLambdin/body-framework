# MIT License
# Copyright (c) 2025 BODY Framework Contributors

# Shared plugin source (no format-specific code)
set(GAIN_PLUGIN_SOURCES GainPlugin.cpp GainEditor.cpp)

# --- CLAP ---
add_library(GainPlugin_CLAP MODULE
    ${GAIN_PLUGIN_SOURCES}
    GainPlugin_CLAP.cpp
)
target_link_libraries(GainPlugin_CLAP PRIVATE body::clap)
set_target_properties(GainPlugin_CLAP PROPERTIES
    PREFIX ""
    SUFFIX ".clap"
)

# --- VST3 ---
if(BODY_ENABLE_VST3)
    add_library(GainPlugin_VST3 MODULE
        ${GAIN_PLUGIN_SOURCES}
        GainPlugin_VST3.cpp
    )
    target_link_libraries(GainPlugin_VST3 PRIVATE body::vst3)

    # Platform-specific entry point for VST3
    if(APPLE)
        target_sources(GainPlugin_VST3 PRIVATE
            ${CMAKE_SOURCE_DIR}/third_party/vst3sdk/public.sdk/source/main/macmain.cpp
        )
    elseif(WIN32)
        target_sources(GainPlugin_VST3 PRIVATE
            ${CMAKE_SOURCE_DIR}/third_party/vst3sdk/public.sdk/source/main/dllmain.cpp
        )
    else()
        target_sources(GainPlugin_VST3 PRIVATE
            ${CMAKE_SOURCE_DIR}/third_party/vst3sdk/public.sdk/source/main/linuxmain.cpp
        )
    endif()

    if(APPLE)
        target_link_libraries(GainPlugin_VST3 PRIVATE
            "-framework CoreFoundation"
            "-framework Cocoa"
        )
    endif()

    set_target_properties(GainPlugin_VST3 PROPERTIES
        PREFIX ""
        SUFFIX ".vst3"
    )
endif()

# --- AU (macOS only) ---
if(APPLE AND BODY_ENABLE_AU)
    add_library(GainPlugin_AU MODULE
        ${GAIN_PLUGIN_SOURCES}
        GainPlugin_AU.mm
    )
    target_link_libraries(GainPlugin_AU PRIVATE body::au)
    set_target_properties(GainPlugin_AU PROPERTIES
        PREFIX ""
        SUFFIX ".component"
        BUNDLE TRUE
        BUNDLE_EXTENSION "component"
    )
endif()
