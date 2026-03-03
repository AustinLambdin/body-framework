// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include <string>
#include <utility>

namespace body {

/// @brief Type-safe string identifier for parameters and other named objects
struct Identifier {
    std::string id;

    explicit Identifier(std::string id) : id(std::move(id)) {}

    [[nodiscard]] bool operator==(const Identifier& other) const noexcept {
        return id == other.id;
    }

    [[nodiscard]] bool operator!=(const Identifier& other) const noexcept {
        return id != other.id;
    }

    [[nodiscard]] bool operator<(const Identifier& other) const noexcept {
        return id < other.id;
    }
};

/// @brief Type-safe parameter ID
using ParameterID = Identifier;

} // namespace body
