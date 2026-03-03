// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "body/core/Parameter.hpp"
#include "body/core/ParameterGroup.hpp"

using namespace Catch::Matchers;

// --- FloatParameter ---

TEST_CASE("FloatParameter default value", "[Parameter]") {
    body::FloatParameter param(body::ParameterID("gain"), "Gain", -60.0f, 24.0f, 0.0f, "dB");

    REQUIRE_THAT(param.getDenormalized(), WithinAbs(0.0f, 1e-6f));
    REQUIRE(param.getType() == body::Parameter::Type::Float);
    REQUIRE(param.getId() == "gain");
    REQUIRE(param.getName() == "Gain");
    REQUIRE(param.getUnits() == "dB");
}

TEST_CASE("FloatParameter normalized mapping", "[Parameter]") {
    body::FloatParameter param(body::ParameterID("freq"), "Frequency", 20.0f, 20000.0f, 1000.0f, "Hz");

    // Default value
    REQUIRE_THAT(param.getDenormalized(), WithinAbs(1000.0f, 1e-3f));

    // Set normalized
    param.setNormalized(0.0f);
    REQUIRE_THAT(param.getDenormalized(), WithinAbs(20.0f, 1e-3f));

    param.setNormalized(1.0f);
    REQUIRE_THAT(param.getDenormalized(), WithinAbs(20000.0f, 1e-1f));

    param.setNormalized(0.5f);
    REQUIRE_THAT(param.getDenormalized(), WithinAbs(10010.0f, 1e-1f));
}

TEST_CASE("FloatParameter denormalized clamping", "[Parameter]") {
    body::FloatParameter param(body::ParameterID("x"), "X", 0.0f, 10.0f, 5.0f);

    param.setDenormalized(15.0f);
    REQUIRE_THAT(param.getDenormalized(), WithinAbs(10.0f, 1e-6f));

    param.setDenormalized(-5.0f);
    REQUIRE_THAT(param.getDenormalized(), WithinAbs(0.0f, 1e-6f));
}

TEST_CASE("FloatParameter display string", "[Parameter]") {
    body::FloatParameter param(body::ParameterID("g"), "Gain", -60.0f, 24.0f, 0.0f, "dB");
    auto display = param.getDisplayValue();
    REQUIRE(display.find("dB") != std::string::npos);
}

// --- IntParameter ---

TEST_CASE("IntParameter default and range", "[Parameter]") {
    body::IntParameter param(body::ParameterID("oct"), "Octave", -3, 3, 0, "oct");

    REQUIRE_THAT(param.getDenormalized(), WithinAbs(0.0f, 1e-6f));
    REQUIRE(param.getType() == body::Parameter::Type::Int);
    REQUIRE(param.getMinValue() == -3);
    REQUIRE(param.getMaxValue() == 3);
}

TEST_CASE("IntParameter normalized mapping", "[Parameter]") {
    body::IntParameter param(body::ParameterID("v"), "Value", 0, 10, 5);

    param.setNormalized(0.0f);
    REQUIRE_THAT(param.getDenormalized(), WithinAbs(0.0f, 0.5f));

    param.setNormalized(1.0f);
    REQUIRE_THAT(param.getDenormalized(), WithinAbs(10.0f, 0.5f));
}

// --- BoolParameter ---

TEST_CASE("BoolParameter default", "[Parameter]") {
    body::BoolParameter param(body::ParameterID("bypass"), "Bypass", false);

    REQUIRE(param.getValue() == false);
    REQUIRE(param.getType() == body::Parameter::Type::Bool);
}

TEST_CASE("BoolParameter toggle", "[Parameter]") {
    body::BoolParameter param(body::ParameterID("bypass"), "Bypass", false);

    param.setNormalized(1.0f);
    REQUIRE(param.getValue() == true);
    REQUIRE(param.getDisplayValue() == "On");

    param.setNormalized(0.0f);
    REQUIRE(param.getValue() == false);
    REQUIRE(param.getDisplayValue() == "Off");
}

// --- ChoiceParameter ---

TEST_CASE("ChoiceParameter default", "[Parameter]") {
    body::ChoiceParameter param(body::ParameterID("mode"), "Mode",
                                {"Normal", "Vintage", "Modern"}, 0);

    REQUIRE(param.getChoiceIndex() == 0);
    REQUIRE(param.getDisplayValue() == "Normal");
    REQUIRE(param.getType() == body::Parameter::Type::Choice);
}

TEST_CASE("ChoiceParameter selection", "[Parameter]") {
    body::ChoiceParameter param(body::ParameterID("mode"), "Mode",
                                {"A", "B", "C"}, 0);

    param.setDenormalized(2.0f);
    REQUIRE(param.getChoiceIndex() == 2);
    REQUIRE(param.getDisplayValue() == "C");

    param.setNormalized(0.5f);
    REQUIRE(param.getChoiceIndex() == 1);
    REQUIRE(param.getDisplayValue() == "B");
}

// --- ParameterGroup ---

TEST_CASE("ParameterGroup add and find", "[ParameterGroup]") {
    body::ParameterGroup group;

    auto& gain = group.addParameter<body::FloatParameter>(
        body::ParameterID("gain"), "Gain", -60.0f, 24.0f, 0.0f, "dB");

    auto& bypass = group.addParameter<body::BoolParameter>(
        body::ParameterID("bypass"), "Bypass", false);

    REQUIRE(group.size() == 2);

    auto* found = group.findParameter("gain");
    REQUIRE(found != nullptr);
    REQUIRE(found == &gain);

    auto* found2 = group.findParameter("bypass");
    REQUIRE(found2 != nullptr);
    REQUIRE(found2 == &bypass);

    auto* notFound = group.findParameter("nonexistent");
    REQUIRE(notFound == nullptr);
}

TEST_CASE("ParameterGroup getAllParameters", "[ParameterGroup]") {
    body::ParameterGroup group;
    group.addParameter<body::FloatParameter>(
        body::ParameterID("a"), "A", 0.0f, 1.0f, 0.5f);
    group.addParameter<body::FloatParameter>(
        body::ParameterID("b"), "B", 0.0f, 1.0f, 0.5f);

    auto all = group.getAllParameters();
    REQUIRE(all.size() == 2);
}

TEST_CASE("FloatParameter atomic access", "[Parameter]") {
    body::FloatParameter param(body::ParameterID("x"), "X", 0.0f, 100.0f, 50.0f);

    // Simulate host setting via normalized
    param.setNormalized(0.25f);
    float value = param.getDenormalized();
    REQUIRE_THAT(value, WithinAbs(25.0f, 1e-3f));

    // Normalized round-trip
    float norm = param.getNormalized();
    REQUIRE_THAT(norm, WithinAbs(0.25f, 1e-3f));
}
