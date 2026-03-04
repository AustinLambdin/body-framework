# MIT License
# Copyright (c) 2025 BODY Framework Contributors

# Catch2 is added via add_subdirectory in root CMakeLists.txt

add_executable(body_tests
    test_AudioBuffer.cpp
    test_Parameter.cpp
    test_MidiBuffer.cpp
    test_ProcessContext.cpp
    test_StateSerializer.cpp
    test_GuiPrimitives.cpp
    test_Component.cpp
    test_ParameterAttachment.cpp
    test_SIMD.cpp
    test_DelayLine.cpp
    test_BiquadFilter.cpp
    test_SVFilter.cpp
    test_Envelope.cpp
    test_Oscillator.cpp
    test_FFT.cpp
    test_Integration.cpp
)
target_link_libraries(body_tests PRIVATE body::core body::gui body::dsp Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/third_party/catch2/extras)
include(CTest)
include(Catch)
catch_discover_tests(body_tests)
