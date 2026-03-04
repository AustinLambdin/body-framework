// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include "Point.hpp"

namespace body {

/// @brief Describes a mouse event delivered to a Component
struct MouseEvent {
    enum class Type { kDown, kUp, kDrag, kMove, kWheel };
    enum class Button { kNone, kLeft, kRight, kMiddle };

    Type type = Type::kMove;
    Button button = Button::kNone;
    Point<float> position;
    float wheelDeltaX = 0.0f;
    float wheelDeltaY = 0.0f;
    bool shiftDown = false;
    bool ctrlDown = false;
    bool altDown = false;
    bool cmdDown = false;
};

} // namespace body
