// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "MidiBuffer.hpp"
#include <algorithm>

namespace body {

void MidiBuffer::addMessage(MidiMessage message) {
    auto it = std::lower_bound(messages_.begin(), messages_.end(), message,
        [](const MidiMessage& a, const MidiMessage& b) {
            return a.samplePosition < b.samplePosition;
        });
    messages_.insert(it, message);
}

void MidiBuffer::clear() noexcept {
    messages_.clear();
}

bool MidiBuffer::isEmpty() const noexcept {
    return messages_.empty();
}

int MidiBuffer::size() const noexcept {
    return static_cast<int>(messages_.size());
}

std::vector<MidiMessage> MidiBuffer::getMessagesInRange(int startSample, int endSample) const {
    std::vector<MidiMessage> result;
    for (const auto& msg : messages_) {
        if (msg.samplePosition >= startSample && msg.samplePosition < endSample) {
            result.push_back(msg);
        }
    }
    return result;
}

} // namespace body
