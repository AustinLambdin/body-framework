// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "body/gui/Colour.hpp"
#include "body/gui/Point.hpp"
#include "body/gui/Rectangle.hpp"
#include "body/gui/Font.hpp"

using namespace body;
using Catch::Approx;

// --- Colour Tests ---

TEST_CASE("Colour default is black opaque", "[gui][colour]") {
    Colour c;
    CHECK(c.r == 0.0f);
    CHECK(c.g == 0.0f);
    CHECK(c.b == 0.0f);
    CHECK(c.a == 1.0f);
}

TEST_CASE("Colour construct from floats", "[gui][colour]") {
    Colour c(0.2f, 0.4f, 0.6f, 0.8f);
    CHECK(c.r == Approx(0.2f));
    CHECK(c.g == Approx(0.4f));
    CHECK(c.b == Approx(0.6f));
    CHECK(c.a == Approx(0.8f));
}

TEST_CASE("Colour fromRGBA", "[gui][colour]") {
    auto c = Colour::fromRGBA(255, 128, 0, 255);
    CHECK(c.r == Approx(1.0f));
    CHECK(c.g == Approx(128.0f / 255.0f));
    CHECK(c.b == Approx(0.0f));
    CHECK(c.a == Approx(1.0f));
}

TEST_CASE("Colour withAlpha", "[gui][colour]") {
    Colour c(1.0f, 0.0f, 0.0f);
    auto semi = c.withAlpha(0.5f);
    CHECK(semi.r == 1.0f);
    CHECK(semi.a == Approx(0.5f));
}

TEST_CASE("Colour constants", "[gui][colour]") {
    CHECK(Colours::kBlack == Colour(0.0f, 0.0f, 0.0f, 1.0f));
    CHECK(Colours::kWhite == Colour(1.0f, 1.0f, 1.0f, 1.0f));
    CHECK(Colours::kTransparent.a == 0.0f);
}

// --- Point Tests ---

TEST_CASE("Point default is zero", "[gui][point]") {
    Point<float> p;
    CHECK(p.x == 0.0f);
    CHECK(p.y == 0.0f);
}

TEST_CASE("Point arithmetic", "[gui][point]") {
    Point<float> a(1.0f, 2.0f);
    Point<float> b(3.0f, 4.0f);

    auto sum = a + b;
    CHECK(sum.x == Approx(4.0f));
    CHECK(sum.y == Approx(6.0f));

    auto diff = b - a;
    CHECK(diff.x == Approx(2.0f));
    CHECK(diff.y == Approx(2.0f));

    auto scaled = a * 3.0f;
    CHECK(scaled.x == Approx(3.0f));
    CHECK(scaled.y == Approx(6.0f));
}

TEST_CASE("Point distanceTo", "[gui][point]") {
    Point<float> a(0.0f, 0.0f);
    Point<float> b(3.0f, 4.0f);
    CHECK(a.distanceTo(b) == Approx(5.0f));
}

// --- Rectangle Tests ---

TEST_CASE("Rectangle default is zero", "[gui][rectangle]") {
    Rectangle<float> r;
    CHECK(r.x == 0.0f);
    CHECK(r.width == 0.0f);
}

TEST_CASE("Rectangle contains point", "[gui][rectangle]") {
    Rectangle<float> r(10.0f, 20.0f, 100.0f, 50.0f);
    CHECK(r.contains({50.0f, 40.0f}));
    CHECK_FALSE(r.contains({5.0f, 40.0f}));
    CHECK_FALSE(r.contains({50.0f, 80.0f}));
}

TEST_CASE("Rectangle intersects", "[gui][rectangle]") {
    Rectangle<float> a(0.0f, 0.0f, 100.0f, 100.0f);
    Rectangle<float> b(50.0f, 50.0f, 100.0f, 100.0f);
    Rectangle<float> c(200.0f, 200.0f, 10.0f, 10.0f);
    CHECK(a.intersects(b));
    CHECK_FALSE(a.intersects(c));
}

TEST_CASE("Rectangle reduced/expanded", "[gui][rectangle]") {
    Rectangle<float> r(10.0f, 20.0f, 100.0f, 50.0f);
    auto reduced = r.reduced(5.0f);
    CHECK(reduced.x == Approx(15.0f));
    CHECK(reduced.y == Approx(25.0f));
    CHECK(reduced.width == Approx(90.0f));
    CHECK(reduced.height == Approx(40.0f));

    auto expanded = r.expanded(5.0f);
    CHECK(expanded.x == Approx(5.0f));
    CHECK(expanded.width == Approx(110.0f));
}

TEST_CASE("Rectangle translated", "[gui][rectangle]") {
    Rectangle<float> r(10.0f, 20.0f, 100.0f, 50.0f);
    auto moved = r.translated(5.0f, -10.0f);
    CHECK(moved.x == Approx(15.0f));
    CHECK(moved.y == Approx(10.0f));
    CHECK(moved.width == Approx(100.0f));
}

TEST_CASE("Rectangle getCentre/getRight/getBottom", "[gui][rectangle]") {
    Rectangle<float> r(10.0f, 20.0f, 100.0f, 50.0f);
    auto centre = r.getCentre();
    CHECK(centre.x == Approx(60.0f));
    CHECK(centre.y == Approx(45.0f));
    CHECK(r.getRight() == Approx(110.0f));
    CHECK(r.getBottom() == Approx(70.0f));
}

// --- Font Tests ---

TEST_CASE("Font defaults", "[gui][font]") {
    Font f;
    CHECK(f.family == "Helvetica");
    CHECK(f.size == Approx(14.0f));
    CHECK(f.weight == Font::Weight::kRegular);
}

TEST_CASE("Font custom construction", "[gui][font]") {
    Font f("Arial", 24.0f, Font::Weight::kBold);
    CHECK(f.family == "Arial");
    CHECK(f.size == Approx(24.0f));
    CHECK(f.weight == Font::Weight::kBold);
}

TEST_CASE("Font size-only construction", "[gui][font]") {
    Font f(18.0f);
    CHECK(f.size == Approx(18.0f));
    CHECK(f.family == "Helvetica");
}
