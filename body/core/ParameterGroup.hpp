// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include "Parameter.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace body {

/// @brief Owns and manages a collection of parameters with lookup by ID
class ParameterGroup {
public:
    /// @brief Add a parameter of the given type, constructed in place
    /// @return Reference to the newly created parameter
    template<typename ParamType, typename... Args>
    ParamType& addParameter(Args&&... args) {
        auto param = std::make_unique<ParamType>(std::forward<Args>(args)...);
        auto* raw = param.get();
        lookup_[raw->getId()] = raw;
        parameters_.push_back(std::move(param));
        return *raw;
    }

    /// @brief Find a parameter by its string ID
    [[nodiscard]] Parameter* findParameter(const std::string& id);
    [[nodiscard]] const Parameter* findParameter(const std::string& id) const;

    /// @brief Get all parameters as raw pointers
    [[nodiscard]] std::vector<Parameter*> getAllParameters();

    /// @brief Get the number of parameters
    [[nodiscard]] int size() const noexcept;

    // Range-based iteration
    [[nodiscard]] auto begin() noexcept { return parameters_.begin(); }
    [[nodiscard]] auto end() noexcept { return parameters_.end(); }
    [[nodiscard]] auto begin() const noexcept { return parameters_.begin(); }
    [[nodiscard]] auto end() const noexcept { return parameters_.end(); }

private:
    std::vector<std::unique_ptr<Parameter>> parameters_;
    std::unordered_map<std::string, Parameter*> lookup_;
};

} // namespace body
