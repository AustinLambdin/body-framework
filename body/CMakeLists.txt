# MIT License
# Copyright (c) 2025 BODY Framework Contributors

# --- body::core (STATIC) ---
add_library(body_core STATIC
    core/AudioBuffer.cpp
    core/MidiMessage.cpp
    core/MidiBuffer.cpp
    core/Parameter.cpp
    core/ParameterGroup.cpp
    core/BusLayout.cpp
    core/AudioProcessor.cpp
    core/StateSerializer.cpp
)
target_include_directories(body_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
add_library(body::core ALIAS body_core)

# --- body::dsp (STATIC) ---
add_library(body_dsp STATIC
    dsp/DelayLine.cpp
    dsp/BiquadFilter.cpp
    dsp/SVFilter.cpp
    dsp/Envelope.cpp
    dsp/Oscillator.cpp
    dsp/FFT.cpp
)
target_link_libraries(body_dsp PUBLIC body::core pffft)
add_library(body::dsp ALIAS body_dsp)

# --- body::gui (STATIC) ---
add_library(body_gui STATIC
    gui/Component.cpp
    gui/PluginEditor.cpp
    gui/ParameterAttachment.cpp
    gui/components/Knob.cpp
    gui/components/Slider.cpp
    gui/components/Label.cpp
    gui/components/Button.cpp
    gui/components/Toggle.cpp
)
if(APPLE)
    target_sources(body_gui PRIVATE
        gui/platform/mac/GraphicsMac.mm
        gui/platform/mac/PlatformViewMac.mm
    )
    target_link_libraries(body_gui PUBLIC
        "-framework CoreGraphics"
        "-framework AppKit"
        "-framework CoreText"
    )
endif()
target_link_libraries(body_gui PUBLIC body::core)
add_library(body::gui ALIAS body_gui)

# --- body::clap (STATIC) ---
add_library(body_clap STATIC
    formats/clap/ClapPlugin.cpp
    formats/clap/ClapExtensions.cpp
    formats/clap/ClapEntry.cpp
)
if(APPLE)
    target_sources(body_clap PRIVATE formats/clap/ClapGuiExtension.mm)
    target_link_libraries(body_clap PUBLIC body::gui)
endif()
target_link_libraries(body_clap PUBLIC body::core clap)
add_library(body::clap ALIAS body_clap)

# --- body::vst3 (STATIC, guarded by BODY_ENABLE_VST3) ---
if(BODY_ENABLE_VST3)
    add_library(body_vst3 STATIC
        formats/vst3/Vst3Plugin.cpp
        formats/vst3/Vst3Entry.cpp
        ${CMAKE_SOURCE_DIR}/third_party/vst3sdk/public.sdk/source/vst/vstsinglecomponenteffect.cpp
    )
    if(APPLE)
        target_sources(body_vst3 PRIVATE formats/vst3/Vst3EditorView.mm)
        target_link_libraries(body_vst3 PUBLIC body::gui)
    endif()
    target_link_libraries(body_vst3 PUBLIC body::core sdk)
    target_include_directories(body_vst3 PUBLIC
        ${CMAKE_SOURCE_DIR}/third_party/vst3sdk
    )
    add_library(body::vst3 ALIAS body_vst3)
endif()

# --- body::au (STATIC, macOS only, guarded by BODY_ENABLE_AU) ---
if(APPLE AND BODY_ENABLE_AU)
    add_library(body_au STATIC
        formats/au/AUPlugin.mm
        formats/au/AUEntry.mm
        formats/au/BODYEditorViewController.mm
    )
    target_link_libraries(body_au PUBLIC body::core body::gui)
    target_link_libraries(body_au PUBLIC
        "-framework AudioToolbox"
        "-framework AVFoundation"
        "-framework CoreAudio"
        "-framework Foundation"
        "-framework CoreAudioKit"
    )
    add_library(body::au ALIAS body_au)
endif()

# --- body::body (INTERFACE meta-target) ---
add_library(body_body INTERFACE)
target_link_libraries(body_body INTERFACE body::core body::dsp body::gui body::clap)
if(TARGET body_vst3)
    target_link_libraries(body_body INTERFACE body::vst3)
endif()
if(TARGET body_au)
    target_link_libraries(body_body INTERFACE body::au)
endif()
add_library(body::body ALIAS body_body)
