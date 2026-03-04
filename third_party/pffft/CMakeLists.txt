# pffft - Pretty Fast FFT
# BSD-like license (FFTPACK)

add_library(pffft STATIC pffft.c)
target_include_directories(pffft PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Disable SIMD-related warnings for vendored code
if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(pffft PRIVATE -w)
endif()
