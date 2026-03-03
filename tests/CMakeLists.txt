# MIT License
# Copyright (c) 2025 BODY Framework Contributors

# Catch2 is added via add_subdirectory in root CMakeLists.txt

add_executable(body_tests
    test_AudioBuffer.cpp
    test_Parameter.cpp
    test_MidiBuffer.cpp
    test_ProcessContext.cpp
)
target_link_libraries(body_tests PRIVATE body::core Catch2::Catch2WithMain)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/third_party/catch2/extras)
include(CTest)
include(Catch)
catch_discover_tests(body_tests)
