// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "body/dsp/Envelope.hpp"

#include <algorithm>
#include <cmath>

namespace body::dsp {

void Envelope::prepare(double sampleRate) noexcept
{
    sampleRate_ = sampleRate;
    reset();
}

void Envelope::setParameters(float attackSeconds, float decaySeconds,
                             float sustainLevel, float releaseSeconds) noexcept
{
    attackCoeff_ = calculateCoefficient(attackSeconds);
    decayCoeff_ = calculateCoefficient(decaySeconds);
    sustainLevel_ = std::clamp(sustainLevel, 0.0f, 1.0f);
    releaseCoeff_ = calculateCoefficient(releaseSeconds);
}

void Envelope::noteOn() noexcept
{
    stage_ = Stage::Attack;
}

void Envelope::noteOff() noexcept
{
    if (stage_ != Stage::Idle)
        stage_ = Stage::Release;
}

float Envelope::process() noexcept
{
    switch (stage_) {
        case Stage::Idle:
            return 0.0f;

        case Stage::Attack:
            // Target slightly above 1.0 so we actually reach 1.0
            output_ += attackCoeff_ * (1.3f - output_);
            if (output_ >= 1.0f) {
                output_ = 1.0f;
                stage_ = Stage::Decay;
            }
            break;

        case Stage::Decay:
            output_ += decayCoeff_ * (sustainLevel_ - output_);
            if (std::abs(output_ - sustainLevel_) < 1e-4f) {
                output_ = sustainLevel_;
                stage_ = Stage::Sustain;
            }
            break;

        case Stage::Sustain:
            output_ = sustainLevel_;
            break;

        case Stage::Release:
            output_ += releaseCoeff_ * (0.0f - output_);
            if (output_ < 1e-4f) {
                output_ = 0.0f;
                stage_ = Stage::Idle;
            }
            break;
    }

    return output_;
}

void Envelope::reset() noexcept
{
    output_ = 0.0f;
    stage_ = Stage::Idle;
}

float Envelope::calculateCoefficient(float timeSeconds) const noexcept
{
    if (timeSeconds <= 0.0f)
        return 1.0f;

    // One-pole coefficient: 1 - exp(-1 / (time * sampleRate))
    return 1.0f - std::exp(-1.0f / (timeSeconds * static_cast<float>(sampleRate_)));
}

} // namespace body::dsp
