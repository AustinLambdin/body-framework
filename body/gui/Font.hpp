// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include <string>

namespace body {

/// @brief Describes a font for text rendering
struct Font {
    enum class Weight { kLight, kRegular, kBold };

    std::string family = "Helvetica";
    float size = 14.0f;
    Weight weight = Weight::kRegular;

    Font() = default;
    Font(std::string f, float s, Weight w = Weight::kRegular)
        : family(std::move(f)), size(s), weight(w) {}
    explicit Font(float s) : size(s) {}
};

} // namespace body
