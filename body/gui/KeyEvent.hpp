// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include <cstdint>

namespace body {

/// @brief Describes a keyboard event delivered to a Component
struct KeyEvent {
    enum class Type { kDown, kUp };

    Type type = Type::kDown;
    char keyChar = '\0';
    uint16_t keyCode = 0;
    bool shiftDown = false;
    bool ctrlDown = false;
    bool altDown = false;
    bool cmdDown = false;
};

} // namespace body
