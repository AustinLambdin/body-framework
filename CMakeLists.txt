# MIT License
# Copyright (c) 2025 BODY Framework Contributors

cmake_minimum_required(VERSION 3.24)
project(BODY VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Strict warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-Wall -Wextra -Wpedantic)
elseif(MSVC)
    add_compile_options(/W4)
endif()

# Options
option(BODY_BUILD_TESTS "Build BODY tests" ON)
option(BODY_BUILD_EXAMPLES "Build BODY example plugins" ON)

# Third-party
add_subdirectory(third_party/clap EXCLUDE_FROM_ALL)

# Framework
add_subdirectory(body)

# Examples
if(BODY_BUILD_EXAMPLES)
    add_subdirectory(examples/GainPlugin)
endif()

# Tests
if(BODY_BUILD_TESTS)
    add_subdirectory(third_party/catch2 EXCLUDE_FROM_ALL)
    add_subdirectory(tests)
endif()
