// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "Parameter.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace body {

// --- Parameter base ---

Parameter::Parameter(ParameterID id, std::string name, std::string units)
    : id_(std::move(id)), name_(std::move(name)), units_(std::move(units)) {}

const std::string& Parameter::getId() const noexcept { return id_.id; }
const std::string& Parameter::getName() const noexcept { return name_; }
const std::string& Parameter::getUnits() const noexcept { return units_; }

// --- FloatParameter ---

FloatParameter::FloatParameter(ParameterID id, std::string name,
                               float minValue, float maxValue, float defaultValue,
                               std::string units)
    : Parameter(std::move(id), std::move(name), std::move(units))
    , min_(minValue), max_(maxValue), default_(defaultValue)
    , atomicValue_(defaultValue) {}

float FloatParameter::getMinValue() const noexcept { return min_; }
float FloatParameter::getMaxValue() const noexcept { return max_; }
float FloatParameter::getDefaultValue() const noexcept { return default_; }

float FloatParameter::getNormalized() const noexcept {
    float value = atomicValue_.load(std::memory_order_relaxed);
    if (max_ == min_) return 0.0f;
    return (value - min_) / (max_ - min_);
}

void FloatParameter::setNormalized(float normalized) noexcept {
    normalized = std::clamp(normalized, 0.0f, 1.0f);
    float value = min_ + normalized * (max_ - min_);
    atomicValue_.store(value, std::memory_order_relaxed);
}

float FloatParameter::getDenormalized() const noexcept {
    return atomicValue_.load(std::memory_order_relaxed);
}

void FloatParameter::setDenormalized(float value) noexcept {
    value = std::clamp(value, min_, max_);
    atomicValue_.store(value, std::memory_order_relaxed);
}

std::string FloatParameter::getDisplayValue() const {
    std::ostringstream oss;
    oss.precision(2);
    oss << std::fixed << getDenormalized();
    if (!units_.empty()) oss << " " << units_;
    return oss.str();
}

Parameter::Type FloatParameter::getType() const noexcept { return Type::Float; }

// --- IntParameter ---

IntParameter::IntParameter(ParameterID id, std::string name,
                           int minValue, int maxValue, int defaultValue,
                           std::string units)
    : Parameter(std::move(id), std::move(name), std::move(units))
    , min_(minValue), max_(maxValue), default_(defaultValue)
    , atomicValue_(static_cast<float>(defaultValue)) {}

int IntParameter::getMinValue() const noexcept { return min_; }
int IntParameter::getMaxValue() const noexcept { return max_; }
int IntParameter::getDefaultValue() const noexcept { return default_; }

float IntParameter::getNormalized() const noexcept {
    float value = atomicValue_.load(std::memory_order_relaxed);
    if (max_ == min_) return 0.0f;
    return (value - static_cast<float>(min_)) / static_cast<float>(max_ - min_);
}

void IntParameter::setNormalized(float normalized) noexcept {
    normalized = std::clamp(normalized, 0.0f, 1.0f);
    float value = static_cast<float>(min_) + normalized * static_cast<float>(max_ - min_);
    atomicValue_.store(std::round(value), std::memory_order_relaxed);
}

float IntParameter::getDenormalized() const noexcept {
    return std::round(atomicValue_.load(std::memory_order_relaxed));
}

void IntParameter::setDenormalized(float value) noexcept {
    value = std::clamp(std::round(value), static_cast<float>(min_), static_cast<float>(max_));
    atomicValue_.store(value, std::memory_order_relaxed);
}

std::string IntParameter::getDisplayValue() const {
    std::ostringstream oss;
    oss << static_cast<int>(getDenormalized());
    if (!units_.empty()) oss << " " << units_;
    return oss.str();
}

Parameter::Type IntParameter::getType() const noexcept { return Type::Int; }

// --- BoolParameter ---

BoolParameter::BoolParameter(ParameterID id, std::string name,
                             bool defaultValue, std::string units)
    : Parameter(std::move(id), std::move(name), std::move(units))
    , default_(defaultValue)
    , atomicValue_(defaultValue ? 1.0f : 0.0f) {}

bool BoolParameter::getValue() const noexcept {
    return atomicValue_.load(std::memory_order_relaxed) >= 0.5f;
}

float BoolParameter::getNormalized() const noexcept {
    return getValue() ? 1.0f : 0.0f;
}

void BoolParameter::setNormalized(float normalized) noexcept {
    atomicValue_.store(normalized >= 0.5f ? 1.0f : 0.0f, std::memory_order_relaxed);
}

float BoolParameter::getDenormalized() const noexcept {
    return getValue() ? 1.0f : 0.0f;
}

void BoolParameter::setDenormalized(float value) noexcept {
    atomicValue_.store(value >= 0.5f ? 1.0f : 0.0f, std::memory_order_relaxed);
}

std::string BoolParameter::getDisplayValue() const {
    return getValue() ? "On" : "Off";
}

Parameter::Type BoolParameter::getType() const noexcept { return Type::Bool; }

// --- ChoiceParameter ---

ChoiceParameter::ChoiceParameter(ParameterID id, std::string name,
                                 std::vector<std::string> choices, int defaultIndex)
    : Parameter(std::move(id), std::move(name))
    , choices_(std::move(choices))
    , default_(defaultIndex)
    , atomicValue_(static_cast<float>(defaultIndex)) {}

int ChoiceParameter::getChoiceIndex() const noexcept {
    return static_cast<int>(std::round(atomicValue_.load(std::memory_order_relaxed)));
}

const std::vector<std::string>& ChoiceParameter::getChoices() const noexcept {
    return choices_;
}

float ChoiceParameter::getNormalized() const noexcept {
    int numChoices = static_cast<int>(choices_.size());
    if (numChoices <= 1) return 0.0f;
    return static_cast<float>(getChoiceIndex()) / static_cast<float>(numChoices - 1);
}

void ChoiceParameter::setNormalized(float normalized) noexcept {
    normalized = std::clamp(normalized, 0.0f, 1.0f);
    int numChoices = static_cast<int>(choices_.size());
    int index = static_cast<int>(std::round(normalized * static_cast<float>(numChoices - 1)));
    atomicValue_.store(static_cast<float>(index), std::memory_order_relaxed);
}

float ChoiceParameter::getDenormalized() const noexcept {
    return static_cast<float>(getChoiceIndex());
}

void ChoiceParameter::setDenormalized(float value) noexcept {
    int index = std::clamp(static_cast<int>(std::round(value)),
                           0, static_cast<int>(choices_.size()) - 1);
    atomicValue_.store(static_cast<float>(index), std::memory_order_relaxed);
}

std::string ChoiceParameter::getDisplayValue() const {
    int index = getChoiceIndex();
    if (index >= 0 && index < static_cast<int>(choices_.size())) {
        return choices_[static_cast<size_t>(index)];
    }
    return "";
}

Parameter::Type ChoiceParameter::getType() const noexcept { return Type::Choice; }

} // namespace body
