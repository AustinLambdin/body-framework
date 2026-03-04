// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "body/gui/ParameterAttachment.hpp"

using namespace body;
using Catch::Approx;

TEST_CASE("ParameterAttachment reads initial value", "[gui][attachment]") {
    FloatParameter param(ParameterID("gain"), "Gain", -60.0f, 24.0f, 0.0f, "dB");
    ParameterAttachment attachment(param);
    CHECK(attachment.getValue() == Approx(0.0f));
}

TEST_CASE("ParameterAttachment setValue round-trips", "[gui][attachment]") {
    FloatParameter param(ParameterID("gain"), "Gain", -60.0f, 24.0f, 0.0f, "dB");
    ParameterAttachment attachment(param);

    attachment.setValue(12.0f);
    CHECK(attachment.getValue() == Approx(12.0f));
    CHECK(param.getDenormalized() == Approx(12.0f));
}

TEST_CASE("ParameterAttachment detects external changes", "[gui][attachment]") {
    FloatParameter param(ParameterID("gain"), "Gain", -60.0f, 24.0f, 0.0f, "dB");
    ParameterAttachment attachment(param);

    float callbackValue = -999.0f;
    int callbackCount = 0;
    attachment.setCallback([&](float v) {
        callbackValue = v;
        callbackCount++;
    });

    // No change yet
    attachment.checkForUpdates();
    CHECK(callbackCount == 0);

    // External change
    param.setDenormalized(6.0f);
    attachment.checkForUpdates();
    CHECK(callbackCount == 1);
    CHECK(callbackValue == Approx(6.0f));

    // No change
    attachment.checkForUpdates();
    CHECK(callbackCount == 1);
}

TEST_CASE("ParameterAttachment normalized access", "[gui][attachment]") {
    FloatParameter param(ParameterID("gain"), "Gain", 0.0f, 100.0f, 50.0f);
    ParameterAttachment attachment(param);

    attachment.setNormalizedValue(0.75f);
    CHECK(attachment.getValue() == Approx(75.0f));
    CHECK(attachment.getNormalizedValue() == Approx(0.75f));
}

TEST_CASE("ParameterAttachment display value", "[gui][attachment]") {
    FloatParameter param(ParameterID("gain"), "Gain", -60.0f, 24.0f, 0.0f, "dB");
    ParameterAttachment attachment(param);
    auto display = attachment.getDisplayValue();
    CHECK_FALSE(display.empty());
}
