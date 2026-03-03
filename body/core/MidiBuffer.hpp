// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include "MidiMessage.hpp"
#include <vector>

namespace body {

/// @brief Sorted collection of MIDI messages for one processing block
class MidiBuffer {
public:
    /// @brief Add a message, maintaining sort order by sample position
    void addMessage(MidiMessage message);

    /// @brief Remove all messages
    void clear() noexcept;

    /// @brief Check if the buffer contains no messages
    [[nodiscard]] bool isEmpty() const noexcept;

    /// @brief Get the number of messages in the buffer
    [[nodiscard]] int size() const noexcept;

    /// @brief Iterator access (messages are in sample-position order)
    [[nodiscard]] auto begin() const noexcept { return messages_.begin(); }
    [[nodiscard]] auto end() const noexcept { return messages_.end(); }
    [[nodiscard]] auto begin() noexcept { return messages_.begin(); }
    [[nodiscard]] auto end() noexcept { return messages_.end(); }

    /// @brief Get messages within a sample range [startSample, endSample)
    [[nodiscard]] std::vector<MidiMessage> getMessagesInRange(int startSample, int endSample) const;

private:
    std::vector<MidiMessage> messages_;
};

} // namespace body
