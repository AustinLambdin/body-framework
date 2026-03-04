// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include "body/core/Parameter.hpp"
#include <functional>
#include <string>

namespace body {

/// @brief Binds a UI component to a Parameter via polling
///
/// Reads the parameter's atomic value at UI refresh rate (e.g. 60Hz).
/// If the value changed since last check, fires the callback.
/// No locks, no listener system — just atomic reads.
class ParameterAttachment {
public:
    using ValueChangedCallback = std::function<void(float)>;

    explicit ParameterAttachment(Parameter& parameter);

    void setCallback(ValueChangedCallback callback);

    /// @brief Call periodically (e.g. 60Hz) to detect external changes
    void checkForUpdates();

    [[nodiscard]] float getValue() const noexcept;
    void setValue(float denormalized);

    [[nodiscard]] float getNormalizedValue() const noexcept;
    void setNormalizedValue(float normalized);

    [[nodiscard]] std::string getDisplayValue() const;

    [[nodiscard]] Parameter& getParameter() noexcept { return parameter_; }

private:
    Parameter& parameter_;
    ValueChangedCallback callback_;
    float lastKnownValue_ = 0.0f;
};

} // namespace body
