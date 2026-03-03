// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "MidiMessage.hpp"

namespace body {

MidiMessage::Type MidiMessage::getType() const noexcept {
    if (numBytes == 0) return Type::Unknown;

    uint8_t status = data[0] & 0xF0;
    switch (status) {
        case 0x90: return (data[2] > 0) ? Type::NoteOn : Type::NoteOff;
        case 0x80: return Type::NoteOff;
        case 0xB0: return Type::ControlChange;
        case 0xE0: return Type::PitchBend;
        case 0xC0: return Type::ProgramChange;
        case 0xD0: return Type::ChannelPressure;
        case 0xA0: return Type::PolyPressure;
        case 0xF0: return Type::SysEx;
        default:   return Type::Unknown;
    }
}

uint8_t MidiMessage::getChannel() const noexcept {
    return data[0] & 0x0F;
}

uint8_t MidiMessage::getNoteNumber() const noexcept {
    return data[1];
}

uint8_t MidiMessage::getVelocity() const noexcept {
    return data[2];
}

uint8_t MidiMessage::getCCNumber() const noexcept {
    return data[1];
}

uint8_t MidiMessage::getCCValue() const noexcept {
    return data[2];
}

float MidiMessage::getPitchBendValue() const noexcept {
    int value = (static_cast<int>(data[2]) << 7) | static_cast<int>(data[1]);
    return static_cast<float>(value - 8192) / 8192.0f;
}

MidiMessage MidiMessage::noteOn(uint8_t channel, uint8_t note, uint8_t velocity, int samplePos) {
    MidiMessage msg;
    msg.data[0] = static_cast<uint8_t>(0x90 | (channel & 0x0F));
    msg.data[1] = note & 0x7F;
    msg.data[2] = velocity & 0x7F;
    msg.numBytes = 3;
    msg.samplePosition = samplePos;
    return msg;
}

MidiMessage MidiMessage::noteOff(uint8_t channel, uint8_t note, uint8_t velocity, int samplePos) {
    MidiMessage msg;
    msg.data[0] = static_cast<uint8_t>(0x80 | (channel & 0x0F));
    msg.data[1] = note & 0x7F;
    msg.data[2] = velocity & 0x7F;
    msg.numBytes = 3;
    msg.samplePosition = samplePos;
    return msg;
}

MidiMessage MidiMessage::cc(uint8_t channel, uint8_t number, uint8_t value, int samplePos) {
    MidiMessage msg;
    msg.data[0] = static_cast<uint8_t>(0xB0 | (channel & 0x0F));
    msg.data[1] = number & 0x7F;
    msg.data[2] = value & 0x7F;
    msg.numBytes = 3;
    msg.samplePosition = samplePos;
    return msg;
}

} // namespace body
