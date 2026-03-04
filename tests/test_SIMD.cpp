// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "body/dsp/SIMD.hpp"

#include <array>

using namespace Catch::Matchers;

TEST_CASE("SIMD add", "[SIMD]")
{
    std::array<float, 8> dst = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    std::array<float, 8> src = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f};

    body::dsp::simd::add(dst.data(), src.data(), dst.size());

    for (std::size_t i = 0; i < dst.size(); ++i) {
        REQUIRE_THAT(dst[i], WithinAbs(static_cast<float>(i + 1) + 0.5f, 1e-6f));
    }
}

TEST_CASE("SIMD multiply", "[SIMD]")
{
    std::array<float, 8> dst = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    std::array<float, 8> src = {2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f};

    body::dsp::simd::multiply(dst.data(), src.data(), dst.size());

    for (std::size_t i = 0; i < dst.size(); ++i) {
        REQUIRE_THAT(dst[i], WithinAbs(static_cast<float>(i + 1) * 2.0f, 1e-6f));
    }
}

TEST_CASE("SIMD applyGain", "[SIMD]")
{
    std::array<float, 8> dst = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};

    body::dsp::simd::applyGain(dst.data(), 0.5f, dst.size());

    for (std::size_t i = 0; i < dst.size(); ++i) {
        REQUIRE_THAT(dst[i], WithinAbs(static_cast<float>(i + 1) * 0.5f, 1e-6f));
    }
}

TEST_CASE("SIMD handles non-multiple-of-4 sizes", "[SIMD]")
{
    std::array<float, 5> dst = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f};
    std::array<float, 5> src = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f};

    body::dsp::simd::add(dst.data(), src.data(), dst.size());

    for (std::size_t i = 0; i < dst.size(); ++i) {
        REQUIRE_THAT(dst[i], WithinAbs(static_cast<float>(i + 1) + 1.0f, 1e-6f));
    }
}

TEST_CASE("SIMD zero-length is safe", "[SIMD]")
{
    float val = 1.0f;
    body::dsp::simd::add(&val, &val, 0);
    body::dsp::simd::multiply(&val, &val, 0);
    body::dsp::simd::applyGain(&val, 2.0f, 0);
    REQUIRE_THAT(val, WithinAbs(1.0f, 1e-6f));
}
