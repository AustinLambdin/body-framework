// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include "Point.hpp"

namespace body {

/// @brief 2D rectangle with position and size
template <typename T>
struct Rectangle {
    T x = {};
    T y = {};
    T width = {};
    T height = {};

    constexpr Rectangle() = default;
    constexpr Rectangle(T px, T py, T w, T h) : x(px), y(py), width(w), height(h) {}

    [[nodiscard]] constexpr bool contains(const Point<T>& point) const {
        return point.x >= x && point.x < x + width &&
               point.y >= y && point.y < y + height;
    }

    [[nodiscard]] constexpr bool intersects(const Rectangle& other) const {
        return x < other.x + other.width && x + width > other.x &&
               y < other.y + other.height && y + height > other.y;
    }

    [[nodiscard]] constexpr Rectangle reduced(T amount) const {
        return {x + amount, y + amount, width - amount * 2, height - amount * 2};
    }

    [[nodiscard]] constexpr Rectangle expanded(T amount) const {
        return {x - amount, y - amount, width + amount * 2, height + amount * 2};
    }

    [[nodiscard]] constexpr Rectangle translated(T dx, T dy) const {
        return {x + dx, y + dy, width, height};
    }

    [[nodiscard]] constexpr Point<T> getCentre() const {
        return {x + width / 2, y + height / 2};
    }

    [[nodiscard]] constexpr T getRight() const { return x + width; }
    [[nodiscard]] constexpr T getBottom() const { return y + height; }

    constexpr bool operator==(const Rectangle& other) const = default;
};

} // namespace body
