// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "body/dsp/Oscillator.hpp"

#include <cmath>
#include <numbers>

namespace body::dsp {

void Oscillator::prepare(double sampleRate) noexcept
{
    sampleRate_ = sampleRate;
    phaseIncrement_ = frequency_ / static_cast<float>(sampleRate_);
    reset();
}

void Oscillator::setFrequency(float hz) noexcept
{
    frequency_ = hz;
    phaseIncrement_ = frequency_ / static_cast<float>(sampleRate_);
}

void Oscillator::setWaveform(Waveform waveform) noexcept
{
    waveform_ = waveform;
}

float Oscillator::process() noexcept
{
    float output = 0.0f;

    switch (waveform_) {
        case Waveform::Sine:
            output = std::sin(2.0f * std::numbers::pi_v<float> * phase_);
            break;

        case Waveform::Saw: {
            // Naive saw: goes from -1 to 1
            output = 2.0f * phase_ - 1.0f;
            // Apply PolyBLEP at discontinuity (phase wraps at 1.0)
            output -= polyBlep(phase_);
            break;
        }

        case Waveform::Square: {
            // Naive square
            output = (phase_ < 0.5f) ? 1.0f : -1.0f;
            // PolyBLEP at both transitions
            output += polyBlep(phase_);
            output -= polyBlep(std::fmod(phase_ + 0.5f, 1.0f));
            break;
        }

        case Waveform::Triangle: {
            // Integrate a PolyBLEP square to get band-limited triangle
            // Naive triangle from phase
            if (phase_ < 0.25f)
                output = 4.0f * phase_;
            else if (phase_ < 0.75f)
                output = 2.0f - 4.0f * phase_;
            else
                output = 4.0f * phase_ - 4.0f;
            break;
        }
    }

    // Advance phase
    phase_ += phaseIncrement_;
    if (phase_ >= 1.0f)
        phase_ -= 1.0f;

    return output;
}

void Oscillator::reset() noexcept
{
    phase_ = 0.0f;
}

float Oscillator::polyBlep(float t) const noexcept
{
    float dt = phaseIncrement_;
    if (dt <= 0.0f)
        return 0.0f;

    if (t < dt) {
        // Near start of period
        float x = t / dt;
        return x + x - x * x - 1.0f;
    }
    if (t > 1.0f - dt) {
        // Near end of period
        float x = (t - 1.0f) / dt;
        return x * x + x + x + 1.0f;
    }

    return 0.0f;
}

} // namespace body::dsp
