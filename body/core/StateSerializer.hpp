// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include "ParameterGroup.hpp"
#include <cstdint>
#include <vector>

namespace body {

/// @brief Serializes/deserializes ParameterGroup state to a binary format
///
/// Binary format:
///   "BODY" magic (4 bytes)
///   version (uint32)
///   param count (uint32)
///   repeated: [id string length (uint32) + id string bytes + float value]
///
/// Deserialization skips unknown parameter IDs gracefully.
class StateSerializer {
public:
    static constexpr uint32_t kCurrentVersion = 1;

    /// @brief Serialize all parameters to a binary blob
    [[nodiscard]] static std::vector<uint8_t> serialize(const ParameterGroup& params);

    /// @brief Deserialize a binary blob into a ParameterGroup
    /// @return true if the data was valid and at least partially restored
    static bool deserialize(ParameterGroup& params, const uint8_t* data, size_t size);
};

} // namespace body
