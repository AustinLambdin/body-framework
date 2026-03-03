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
)
target_include_directories(body_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
add_library(body::core ALIAS body_core)

# --- body::clap (STATIC) ---
add_library(body_clap STATIC
    formats/clap/ClapPlugin.cpp
    formats/clap/ClapExtensions.cpp
    formats/clap/ClapEntry.cpp
)
target_link_libraries(body_clap PUBLIC body::core clap)
add_library(body::clap ALIAS body_clap)

# --- body::body (INTERFACE meta-target) ---
add_library(body_body INTERFACE)
target_link_libraries(body_body INTERFACE body::core body::clap)
add_library(body::body ALIAS body_body)
