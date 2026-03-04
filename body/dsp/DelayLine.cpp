// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "body/dsp/DelayLine.hpp"

#include <algorithm>

namespace body::dsp {

void DelayLine::prepare(std::size_t maxDelaySamples)
{
    maxDelay_ = maxDelaySamples;
    buffer_.resize(maxDelay_ + 1, 0.0f);
    writePos_ = 0;
}

void DelayLine::push(float sample) noexcept
{
    buffer_[writePos_] = sample;
    writePos_ = (writePos_ + 1) % buffer_.size();
}

float DelayLine::read() const noexcept
{
    return buffer_[writePos_];
}

float DelayLine::readAt(float delaySamples) const noexcept
{
    float clamped = std::clamp(delaySamples, 0.0f, static_cast<float>(maxDelay_));

    auto intDelay = static_cast<std::size_t>(clamped);
    float frac = clamped - static_cast<float>(intDelay);

    std::size_t bufSize = buffer_.size();
    // writePos_ points to next write slot, so most recent sample is at writePos_ - 1
    std::size_t idx0 = (writePos_ + bufSize - 1 - intDelay) % bufSize;
    std::size_t idx1 = (writePos_ + bufSize - 2 - intDelay) % bufSize;

    return buffer_[idx0] * (1.0f - frac) + buffer_[idx1] * frac;
}

void DelayLine::reset() noexcept
{
    std::fill(buffer_.begin(), buffer_.end(), 0.0f);
    writePos_ = 0;
}

} // namespace body::dsp
