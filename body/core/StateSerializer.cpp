// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "StateSerializer.hpp"
#include <cstring>

namespace body {

namespace {

void writeUint32(std::vector<uint8_t>& out, uint32_t value) {
    out.push_back(static_cast<uint8_t>(value & 0xFF));
    out.push_back(static_cast<uint8_t>((value >> 8) & 0xFF));
    out.push_back(static_cast<uint8_t>((value >> 16) & 0xFF));
    out.push_back(static_cast<uint8_t>((value >> 24) & 0xFF));
}

bool readUint32(const uint8_t* data, size_t size, size_t& offset, uint32_t& value) {
    if (offset + 4 > size) return false;
    value = static_cast<uint32_t>(data[offset])
          | (static_cast<uint32_t>(data[offset + 1]) << 8)
          | (static_cast<uint32_t>(data[offset + 2]) << 16)
          | (static_cast<uint32_t>(data[offset + 3]) << 24);
    offset += 4;
    return true;
}

void writeFloat(std::vector<uint8_t>& out, float value) {
    uint32_t bits{};
    std::memcpy(&bits, &value, sizeof(bits));
    writeUint32(out, bits);
}

bool readFloat(const uint8_t* data, size_t size, size_t& offset, float& value) {
    uint32_t bits{};
    if (!readUint32(data, size, offset, bits)) return false;
    std::memcpy(&value, &bits, sizeof(value));
    return true;
}

} // namespace

std::vector<uint8_t> StateSerializer::serialize(const ParameterGroup& params) {
    std::vector<uint8_t> out;

    // Magic: "BODY"
    out.push_back('B');
    out.push_back('O');
    out.push_back('D');
    out.push_back('Y');

    // Version
    writeUint32(out, kCurrentVersion);

    // Param count
    auto allParams = const_cast<ParameterGroup&>(params).getAllParameters();
    writeUint32(out, static_cast<uint32_t>(allParams.size()));

    // Each parameter: [id length + id bytes + float value]
    for (auto* param : allParams) {
        const auto& id = param->getId();
        writeUint32(out, static_cast<uint32_t>(id.size()));
        out.insert(out.end(), id.begin(), id.end());
        writeFloat(out, param->getDenormalized());
    }

    return out;
}

bool StateSerializer::deserialize(ParameterGroup& params, const uint8_t* data, size_t size) {
    size_t offset = 0;

    // Check magic
    if (size < 4) return false;
    if (data[0] != 'B' || data[1] != 'O' || data[2] != 'D' || data[3] != 'Y')
        return false;
    offset = 4;

    // Read version
    uint32_t version{};
    if (!readUint32(data, size, offset, version)) return false;
    if (version == 0 || version > kCurrentVersion) return false;

    // Read param count
    uint32_t paramCount{};
    if (!readUint32(data, size, offset, paramCount)) return false;

    // Read each parameter
    for (uint32_t i = 0; i < paramCount; ++i) {
        uint32_t idLen{};
        if (!readUint32(data, size, offset, idLen)) return false;
        if (offset + idLen > size) return false;

        std::string id(reinterpret_cast<const char*>(data + offset), idLen);
        offset += idLen;

        float value{};
        if (!readFloat(data, size, offset, value)) return false;

        // Find and set — skip if unknown
        auto* param = params.findParameter(id);
        if (param != nullptr) {
            param->setDenormalized(value);
        }
    }

    return true;
}

} // namespace body
