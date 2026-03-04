// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include <cmath>

namespace body {

/// @brief 2D point with x/y coordinates
template <typename T>
struct Point {
    T x = {};
    T y = {};

    constexpr Point() = default;
    constexpr Point(T px, T py) : x(px), y(py) {}

    constexpr Point operator+(const Point& other) const { return {x + other.x, y + other.y}; }
    constexpr Point operator-(const Point& other) const { return {x - other.x, y - other.y}; }
    constexpr Point operator*(T scalar) const { return {x * scalar, y * scalar}; }

    constexpr bool operator==(const Point& other) const = default;

    [[nodiscard]] float distanceTo(const Point& other) const {
        auto dx = static_cast<float>(x - other.x);
        auto dy = static_cast<float>(y - other.y);
        return std::sqrt(dx * dx + dy * dy);
    }
};

} // namespace body
