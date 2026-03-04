# MIT License
# Copyright (c) 2025 BODY Framework Contributors

cmake_minimum_required(VERSION 3.24)
if(APPLE)
    project(BODY VERSION 0.1.0 LANGUAGES C CXX OBJCXX)
else()
    project(BODY VERSION 0.1.0 LANGUAGES C CXX)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Static libraries linked into shared MODULE plugins need PIC on Linux
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Strict warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif(MSVC)
    add_compile_options(/W4)
endif()

# Options
option(BODY_BUILD_TESTS "Build BODY tests" ON)
option(BODY_BUILD_EXAMPLES "Build BODY example plugins" ON)
option(BODY_ENABLE_VST3 "Build VST3 format support" ON)
if(APPLE)
    option(BODY_ENABLE_AU "Build Audio Unit format support" ON)
endif()

# Third-party: CLAP
add_subdirectory(third_party/clap EXCLUDE_FROM_ALL)

# Third-party: pffft (Pretty Fast FFT)
add_subdirectory(third_party/pffft EXCLUDE_FROM_ALL)

# Third-party: VST3 SDK
if(BODY_ENABLE_VST3)
    set(SMTG_ENABLE_VST3_PLUGIN_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SMTG_ENABLE_VST3_HOSTING_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(SMTG_ENABLE_VSTGUI_SUPPORT OFF CACHE BOOL "" FORCE)
    add_subdirectory(third_party/vst3sdk EXCLUDE_FROM_ALL)
endif()

# Framework
add_subdirectory(body)

# Examples
if(BODY_BUILD_EXAMPLES)
    add_subdirectory(examples/GainPlugin)
    add_subdirectory(examples/SynthPlugin)
endif()

# Tests
if(BODY_BUILD_TESTS)
    enable_testing()
    add_subdirectory(third_party/catch2 EXCLUDE_FROM_ALL)
    add_subdirectory(tests)
endif()
