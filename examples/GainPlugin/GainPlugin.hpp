// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include "body/core/AudioProcessor.hpp"

namespace body { class PluginEditor; }

/// @brief Simple gain plugin — validates the entire BODY stack
class GainPlugin : public body::AudioProcessor {
public:
    GainPlugin();

    [[nodiscard]] std::string getName() const override { return "Gain"; }
    [[nodiscard]] std::string getVendor() const override { return "BODY Examples"; }
    [[nodiscard]] std::string getVersion() const override { return "1.0.0"; }

    void prepare(double sampleRate, int maxBlockSize) override;
    void processBlock(body::ProcessContext& context) override;
    void reset() override;

    [[nodiscard]] std::unique_ptr<body::PluginEditor> createEditor() override;

private:
    body::FloatParameter* gainParam_ = nullptr;
    body::BoolParameter* bypassParam_ = nullptr;

    float currentGain_ = 1.0f;
    float targetGain_ = 1.0f;
    float smoothingCoeff_ = 0.0f;
};
