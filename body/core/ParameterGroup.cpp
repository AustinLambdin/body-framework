// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "ParameterGroup.hpp"

namespace body {

Parameter* ParameterGroup::findParameter(const std::string& id) {
    auto it = lookup_.find(id);
    return (it != lookup_.end()) ? it->second : nullptr;
}

const Parameter* ParameterGroup::findParameter(const std::string& id) const {
    auto it = lookup_.find(id);
    return (it != lookup_.end()) ? it->second : nullptr;
}

std::vector<Parameter*> ParameterGroup::getAllParameters() {
    std::vector<Parameter*> result;
    result.reserve(parameters_.size());
    for (auto& p : parameters_) {
        result.push_back(p.get());
    }
    return result;
}

int ParameterGroup::size() const noexcept {
    return static_cast<int>(parameters_.size());
}

} // namespace body
