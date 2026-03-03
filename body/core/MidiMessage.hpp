// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include <cstdint>

namespace body {

/// @brief Represents a single MIDI message with its position in the block
struct MidiMessage {
    enum class Type {
        NoteOn, NoteOff, ControlChange, PitchBend,
        ProgramChange, ChannelPressure, PolyPressure,
        SysEx, Unknown
    };

    uint8_t data[3]        = {};
    int     numBytes       = 0;
    int     samplePosition = 0;

    /// @brief Parse the MIDI message type from the status byte
    [[nodiscard]] Type getType() const noexcept;

    /// @brief Get the MIDI channel (0-15)
    [[nodiscard]] uint8_t getChannel() const noexcept;

    /// @brief Get the note number (for NoteOn/NoteOff)
    [[nodiscard]] uint8_t getNoteNumber() const noexcept;

    /// @brief Get the velocity (for NoteOn/NoteOff)
    [[nodiscard]] uint8_t getVelocity() const noexcept;

    /// @brief Get the CC number (for ControlChange)
    [[nodiscard]] uint8_t getCCNumber() const noexcept;

    /// @brief Get the CC value (for ControlChange)
    [[nodiscard]] uint8_t getCCValue() const noexcept;

    /// @brief Get pitch bend as normalized float (-1.0 to 1.0)
    [[nodiscard]] float getPitchBendValue() const noexcept;

    /// @brief Create a NoteOn message
    static MidiMessage noteOn(uint8_t channel, uint8_t note, uint8_t velocity, int samplePos = 0);

    /// @brief Create a NoteOff message
    static MidiMessage noteOff(uint8_t channel, uint8_t note, uint8_t velocity = 0, int samplePos = 0);

    /// @brief Create a ControlChange message
    static MidiMessage cc(uint8_t channel, uint8_t number, uint8_t value, int samplePos = 0);
};

} // namespace body
