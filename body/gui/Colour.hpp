// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include <cstdint>

namespace body {

/// @brief RGBA colour with float components in [0, 1]
struct Colour {
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 1.0f;

    constexpr Colour() = default;
    constexpr Colour(float red, float green, float blue, float alpha = 1.0f)
        : r(red), g(green), b(blue), a(alpha) {}

    [[nodiscard]] static constexpr Colour fromRGBA(uint8_t red, uint8_t green,
                                                    uint8_t blue, uint8_t alpha = 255) {
        return {static_cast<float>(red) / 255.0f,
                static_cast<float>(green) / 255.0f,
                static_cast<float>(blue) / 255.0f,
                static_cast<float>(alpha) / 255.0f};
    }

    [[nodiscard]] constexpr Colour withAlpha(float alpha) const {
        return {r, g, b, alpha};
    }

    constexpr bool operator==(const Colour& other) const = default;
};

namespace Colours {
    inline constexpr Colour kBlack{0.0f, 0.0f, 0.0f, 1.0f};
    inline constexpr Colour kWhite{1.0f, 1.0f, 1.0f, 1.0f};
    inline constexpr Colour kTransparent{0.0f, 0.0f, 0.0f, 0.0f};
    inline constexpr Colour kRed{1.0f, 0.0f, 0.0f, 1.0f};
    inline constexpr Colour kGreen{0.0f, 1.0f, 0.0f, 1.0f};
    inline constexpr Colour kBlue{0.0f, 0.0f, 1.0f, 1.0f};
    inline constexpr Colour kGrey{0.5f, 0.5f, 0.5f, 1.0f};
} // namespace Colours

} // namespace body
