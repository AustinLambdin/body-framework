// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include "Identifiers.hpp"
#include <atomic>
#include <string>
#include <vector>

namespace body {

/// @brief Base class for all plugin parameters
class Parameter {
public:
    enum class Type { Float, Int, Bool, Choice };

    Parameter(ParameterID id, std::string name, std::string units = "");
    virtual ~Parameter() = default;

    [[nodiscard]] const std::string& getId() const noexcept;
    [[nodiscard]] const std::string& getName() const noexcept;
    [[nodiscard]] const std::string& getUnits() const noexcept;

    /// @brief Get the normalized value [0.0, 1.0] — what the host sees
    [[nodiscard]] virtual float getNormalized() const noexcept = 0;

    /// @brief Set from a normalized value [0.0, 1.0]
    virtual void setNormalized(float normalized) noexcept = 0;

    /// @brief Get the denormalized (actual) value — what plugin code reads
    [[nodiscard]] virtual float getDenormalized() const noexcept = 0;

    /// @brief Set from a denormalized (actual) value
    virtual void setDenormalized(float value) noexcept = 0;

    /// @brief Human-readable string for the current value
    [[nodiscard]] virtual std::string getDisplayValue() const = 0;

    /// @brief Get the parameter type
    [[nodiscard]] virtual Type getType() const noexcept = 0;

protected:
    ParameterID id_;
    std::string name_;
    std::string units_;
};

/// @brief Float parameter with min/max range
class FloatParameter : public Parameter {
public:
    FloatParameter(ParameterID id,
                   std::string name,
                   float minValue,
                   float maxValue,
                   float defaultValue,
                   std::string units = "");

    [[nodiscard]] float getMinValue() const noexcept;
    [[nodiscard]] float getMaxValue() const noexcept;
    [[nodiscard]] float getDefaultValue() const noexcept;

    [[nodiscard]] float getNormalized() const noexcept override;
    void setNormalized(float normalized) noexcept override;
    [[nodiscard]] float getDenormalized() const noexcept override;
    void setDenormalized(float value) noexcept override;
    [[nodiscard]] std::string getDisplayValue() const override;
    [[nodiscard]] Type getType() const noexcept override;

private:
    float min_;
    float max_;
    float default_;
    std::atomic<float> atomicValue_;
};

/// @brief Integer parameter with min/max range
class IntParameter : public Parameter {
public:
    IntParameter(ParameterID id,
                 std::string name,
                 int minValue,
                 int maxValue,
                 int defaultValue,
                 std::string units = "");

    [[nodiscard]] int getMinValue() const noexcept;
    [[nodiscard]] int getMaxValue() const noexcept;
    [[nodiscard]] int getDefaultValue() const noexcept;

    [[nodiscard]] float getNormalized() const noexcept override;
    void setNormalized(float normalized) noexcept override;
    [[nodiscard]] float getDenormalized() const noexcept override;
    void setDenormalized(float value) noexcept override;
    [[nodiscard]] std::string getDisplayValue() const override;
    [[nodiscard]] Type getType() const noexcept override;

private:
    int min_;
    int max_;
    int default_;
    std::atomic<float> atomicValue_;
};

/// @brief Boolean parameter (on/off, bypass, etc.)
class BoolParameter : public Parameter {
public:
    BoolParameter(ParameterID id,
                  std::string name,
                  bool defaultValue,
                  std::string units = "");

    [[nodiscard]] bool getValue() const noexcept;

    [[nodiscard]] float getNormalized() const noexcept override;
    void setNormalized(float normalized) noexcept override;
    [[nodiscard]] float getDenormalized() const noexcept override;
    void setDenormalized(float value) noexcept override;
    [[nodiscard]] std::string getDisplayValue() const override;
    [[nodiscard]] Type getType() const noexcept override;

private:
    bool default_;
    std::atomic<float> atomicValue_;
};

/// @brief Choice parameter (enum-style selection)
class ChoiceParameter : public Parameter {
public:
    ChoiceParameter(ParameterID id,
                    std::string name,
                    std::vector<std::string> choices,
                    int defaultIndex = 0);

    [[nodiscard]] int getChoiceIndex() const noexcept;
    [[nodiscard]] const std::vector<std::string>& getChoices() const noexcept;

    [[nodiscard]] float getNormalized() const noexcept override;
    void setNormalized(float normalized) noexcept override;
    [[nodiscard]] float getDenormalized() const noexcept override;
    void setDenormalized(float value) noexcept override;
    [[nodiscard]] std::string getDisplayValue() const override;
    [[nodiscard]] Type getType() const noexcept override;

private:
    std::vector<std::string> choices_;
    int default_;
    std::atomic<float> atomicValue_;
};

} // namespace body
