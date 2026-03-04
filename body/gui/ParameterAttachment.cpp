// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "ParameterAttachment.hpp"
#include <cmath>

namespace body {

ParameterAttachment::ParameterAttachment(Parameter& parameter)
    : parameter_(parameter)
    , lastKnownValue_(parameter.getDenormalized()) {}

void ParameterAttachment::setCallback(ValueChangedCallback callback) {
    callback_ = std::move(callback);
}

void ParameterAttachment::checkForUpdates() {
    float current = parameter_.getDenormalized();
    if (std::abs(current - lastKnownValue_) > 1e-7f) {
        lastKnownValue_ = current;
        if (callback_) callback_(current);
    }
}

float ParameterAttachment::getValue() const noexcept {
    return parameter_.getDenormalized();
}

void ParameterAttachment::setValue(float denormalized) {
    parameter_.setDenormalized(denormalized);
    lastKnownValue_ = denormalized;
}

float ParameterAttachment::getNormalizedValue() const noexcept {
    return parameter_.getNormalized();
}

void ParameterAttachment::setNormalizedValue(float normalized) {
    parameter_.setNormalized(normalized);
    lastKnownValue_ = parameter_.getDenormalized();
}

std::string ParameterAttachment::getDisplayValue() const {
    return parameter_.getDisplayValue();
}

} // namespace body
