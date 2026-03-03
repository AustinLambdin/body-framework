// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "body/core/MidiMessage.hpp"
#include "body/core/MidiBuffer.hpp"

using namespace Catch::Matchers;

TEST_CASE("MidiMessage noteOn factory", "[MidiMessage]") {
    auto msg = body::MidiMessage::noteOn(0, 60, 100, 10);

    REQUIRE(msg.getType() == body::MidiMessage::Type::NoteOn);
    REQUIRE(msg.getChannel() == 0);
    REQUIRE(msg.getNoteNumber() == 60);
    REQUIRE(msg.getVelocity() == 100);
    REQUIRE(msg.samplePosition == 10);
    REQUIRE(msg.numBytes == 3);
}

TEST_CASE("MidiMessage noteOff factory", "[MidiMessage]") {
    auto msg = body::MidiMessage::noteOff(1, 64, 0, 20);

    REQUIRE(msg.getType() == body::MidiMessage::Type::NoteOff);
    REQUIRE(msg.getChannel() == 1);
    REQUIRE(msg.getNoteNumber() == 64);
    REQUIRE(msg.getVelocity() == 0);
}

TEST_CASE("MidiMessage CC factory", "[MidiMessage]") {
    auto msg = body::MidiMessage::cc(0, 74, 127);

    REQUIRE(msg.getType() == body::MidiMessage::Type::ControlChange);
    REQUIRE(msg.getCCNumber() == 74);
    REQUIRE(msg.getCCValue() == 127);
}

TEST_CASE("MidiMessage noteOn with zero velocity is NoteOff", "[MidiMessage]") {
    auto msg = body::MidiMessage::noteOn(0, 60, 0);
    REQUIRE(msg.getType() == body::MidiMessage::Type::NoteOff);
}

TEST_CASE("MidiMessage pitch bend", "[MidiMessage]") {
    body::MidiMessage msg;
    msg.data[0] = 0xE0; // pitch bend channel 0
    msg.data[1] = 0x00; // LSB
    msg.data[2] = 0x40; // MSB (center)
    msg.numBytes = 3;

    REQUIRE(msg.getType() == body::MidiMessage::Type::PitchBend);
    REQUIRE_THAT(msg.getPitchBendValue(), WithinAbs(0.0f, 0.01f));
}

TEST_CASE("MidiBuffer add and iterate", "[MidiBuffer]") {
    body::MidiBuffer buffer;

    REQUIRE(buffer.isEmpty());
    REQUIRE(buffer.size() == 0);

    buffer.addMessage(body::MidiMessage::noteOn(0, 60, 100, 10));
    buffer.addMessage(body::MidiMessage::noteOff(0, 60, 0, 50));

    REQUIRE_FALSE(buffer.isEmpty());
    REQUIRE(buffer.size() == 2);

    int count = 0;
    for (const auto& msg : buffer) {
        (void)msg;
        ++count;
    }
    REQUIRE(count == 2);
}

TEST_CASE("MidiBuffer sample-position ordering", "[MidiBuffer]") {
    body::MidiBuffer buffer;

    // Add out of order
    buffer.addMessage(body::MidiMessage::noteOn(0, 60, 100, 100));
    buffer.addMessage(body::MidiMessage::noteOn(0, 64, 80, 10));
    buffer.addMessage(body::MidiMessage::noteOn(0, 67, 90, 50));

    REQUIRE(buffer.size() == 3);

    int lastPos = -1;
    for (const auto& msg : buffer) {
        REQUIRE(msg.samplePosition >= lastPos);
        lastPos = msg.samplePosition;
    }
}

TEST_CASE("MidiBuffer range queries", "[MidiBuffer]") {
    body::MidiBuffer buffer;

    buffer.addMessage(body::MidiMessage::noteOn(0, 60, 100, 0));
    buffer.addMessage(body::MidiMessage::noteOn(0, 64, 80, 50));
    buffer.addMessage(body::MidiMessage::noteOn(0, 67, 90, 100));
    buffer.addMessage(body::MidiMessage::noteOff(0, 60, 0, 200));

    auto range = buffer.getMessagesInRange(40, 110);
    REQUIRE(range.size() == 2);
    REQUIRE(range[0].getNoteNumber() == 64);
    REQUIRE(range[1].getNoteNumber() == 67);
}

TEST_CASE("MidiBuffer clear", "[MidiBuffer]") {
    body::MidiBuffer buffer;
    buffer.addMessage(body::MidiMessage::noteOn(0, 60, 100));
    REQUIRE(buffer.size() == 1);

    buffer.clear();
    REQUIRE(buffer.isEmpty());
    REQUIRE(buffer.size() == 0);
}
