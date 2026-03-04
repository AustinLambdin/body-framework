// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "body/core/StateSerializer.hpp"

using namespace body;

TEST_CASE("StateSerializer round-trip with FloatParameter", "[StateSerializer]") {
    ParameterGroup params;
    params.addParameter<FloatParameter>(ParameterID("gain"), "Gain", -60.0f, 24.0f, 0.0f, "dB");
    params.addParameter<BoolParameter>(ParameterID("bypass"), "Bypass", false);

    // Set non-default values
    params.findParameter("gain")->setDenormalized(12.5f);
    params.findParameter("bypass")->setDenormalized(1.0f);

    auto data = StateSerializer::serialize(params);
    REQUIRE(!data.empty());

    // Reset to defaults
    params.findParameter("gain")->setDenormalized(0.0f);
    params.findParameter("bypass")->setDenormalized(0.0f);

    REQUIRE(StateSerializer::deserialize(params, data.data(), data.size()));

    REQUIRE_THAT(params.findParameter("gain")->getDenormalized(),
                 Catch::Matchers::WithinAbs(12.5f, 0.001f));
    REQUIRE_THAT(params.findParameter("bypass")->getDenormalized(),
                 Catch::Matchers::WithinAbs(1.0f, 0.001f));
}

TEST_CASE("StateSerializer round-trip with all parameter types", "[StateSerializer]") {
    ParameterGroup params;
    params.addParameter<FloatParameter>(ParameterID("freq"), "Frequency", 20.0f, 20000.0f, 1000.0f, "Hz");
    params.addParameter<IntParameter>(ParameterID("mode"), "Mode", 0, 3, 0);
    params.addParameter<BoolParameter>(ParameterID("enable"), "Enable", true);
    params.addParameter<ChoiceParameter>(ParameterID("type"), "Type",
        std::vector<std::string>{"Sine", "Square", "Saw"}, 0);

    params.findParameter("freq")->setDenormalized(440.0f);
    params.findParameter("mode")->setDenormalized(2.0f);
    params.findParameter("enable")->setDenormalized(0.0f);
    params.findParameter("type")->setDenormalized(2.0f);

    auto data = StateSerializer::serialize(params);

    // Reset
    params.findParameter("freq")->setDenormalized(1000.0f);
    params.findParameter("mode")->setDenormalized(0.0f);
    params.findParameter("enable")->setDenormalized(1.0f);
    params.findParameter("type")->setDenormalized(0.0f);

    REQUIRE(StateSerializer::deserialize(params, data.data(), data.size()));

    REQUIRE_THAT(params.findParameter("freq")->getDenormalized(),
                 Catch::Matchers::WithinAbs(440.0f, 0.01f));
    REQUIRE_THAT(params.findParameter("mode")->getDenormalized(),
                 Catch::Matchers::WithinAbs(2.0f, 0.01f));
    REQUIRE_THAT(params.findParameter("enable")->getDenormalized(),
                 Catch::Matchers::WithinAbs(0.0f, 0.01f));
    REQUIRE_THAT(params.findParameter("type")->getDenormalized(),
                 Catch::Matchers::WithinAbs(2.0f, 0.01f));
}

TEST_CASE("StateSerializer skips unknown parameters", "[StateSerializer]") {
    // Serialize with two params
    ParameterGroup source;
    source.addParameter<FloatParameter>(ParameterID("known"), "Known", 0.0f, 1.0f, 0.0f);
    source.addParameter<FloatParameter>(ParameterID("unknown"), "Unknown", 0.0f, 1.0f, 0.0f);

    source.findParameter("known")->setDenormalized(0.75f);
    source.findParameter("unknown")->setDenormalized(0.5f);

    auto data = StateSerializer::serialize(source);

    // Deserialize into group with only "known"
    ParameterGroup target;
    target.addParameter<FloatParameter>(ParameterID("known"), "Known", 0.0f, 1.0f, 0.0f);

    REQUIRE(StateSerializer::deserialize(target, data.data(), data.size()));
    REQUIRE_THAT(target.findParameter("known")->getDenormalized(),
                 Catch::Matchers::WithinAbs(0.75f, 0.001f));
}

TEST_CASE("StateSerializer rejects invalid data", "[StateSerializer]") {
    ParameterGroup params;
    params.addParameter<FloatParameter>(ParameterID("x"), "X", 0.0f, 1.0f, 0.0f);

    SECTION("Empty data") {
        REQUIRE_FALSE(StateSerializer::deserialize(params, nullptr, 0));
    }

    SECTION("Wrong magic") {
        std::vector<uint8_t> data = {'N', 'O', 'P', 'E', 1, 0, 0, 0, 0, 0, 0, 0};
        REQUIRE_FALSE(StateSerializer::deserialize(params, data.data(), data.size()));
    }

    SECTION("Truncated header") {
        std::vector<uint8_t> data = {'B', 'O', 'D', 'Y', 1, 0};
        REQUIRE_FALSE(StateSerializer::deserialize(params, data.data(), data.size()));
    }

    SECTION("Version zero") {
        std::vector<uint8_t> data = {'B', 'O', 'D', 'Y', 0, 0, 0, 0, 0, 0, 0, 0};
        REQUIRE_FALSE(StateSerializer::deserialize(params, data.data(), data.size()));
    }

    SECTION("Future version") {
        std::vector<uint8_t> data = {'B', 'O', 'D', 'Y', 99, 0, 0, 0, 0, 0, 0, 0};
        REQUIRE_FALSE(StateSerializer::deserialize(params, data.data(), data.size()));
    }

    SECTION("Truncated param data") {
        // Valid header, claims 1 param, but no param data follows
        std::vector<uint8_t> data = {'B', 'O', 'D', 'Y', 1, 0, 0, 0, 1, 0, 0, 0};
        REQUIRE_FALSE(StateSerializer::deserialize(params, data.data(), data.size()));
    }
}

TEST_CASE("StateSerializer binary format starts with BODY magic", "[StateSerializer]") {
    ParameterGroup params;
    auto data = StateSerializer::serialize(params);

    REQUIRE(data.size() >= 12);
    REQUIRE(data[0] == 'B');
    REQUIRE(data[1] == 'O');
    REQUIRE(data[2] == 'D');
    REQUIRE(data[3] == 'Y');
}
