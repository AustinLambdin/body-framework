// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include <catch2/catch_test_macros.hpp>
#include "body/gui/Component.hpp"

using namespace body;

TEST_CASE("Component default state", "[gui][component]") {
    Component c;
    CHECK(c.isVisible());
    CHECK(c.needsRepaint());
    CHECK(c.getParent() == nullptr);
    CHECK(c.getChildren().empty());
    CHECK(c.getWidth() == 0.0f);
    CHECK(c.getHeight() == 0.0f);
}

TEST_CASE("Component setBounds", "[gui][component]") {
    Component c;
    c.setBounds(10.0f, 20.0f, 100.0f, 50.0f);
    CHECK(c.getBounds().x == 10.0f);
    CHECK(c.getBounds().y == 20.0f);
    CHECK(c.getWidth() == 100.0f);
    CHECK(c.getHeight() == 50.0f);
}

TEST_CASE("Component add/remove children", "[gui][component]") {
    Component parent;
    Component child1;
    Component child2;

    parent.addChild(&child1);
    parent.addChild(&child2);

    CHECK(parent.getChildren().size() == 2);
    CHECK(child1.getParent() == &parent);
    CHECK(child2.getParent() == &parent);

    parent.removeChild(&child1);
    CHECK(parent.getChildren().size() == 1);
    CHECK(child1.getParent() == nullptr);
}

TEST_CASE("Component reparenting", "[gui][component]") {
    Component parent1;
    Component parent2;
    Component child;

    parent1.addChild(&child);
    CHECK(child.getParent() == &parent1);

    parent2.addChild(&child);
    CHECK(child.getParent() == &parent2);
    CHECK(parent1.getChildren().empty());
    CHECK(parent2.getChildren().size() == 1);
}

TEST_CASE("Component visibility", "[gui][component]") {
    Component c;
    CHECK(c.isVisible());
    c.setVisible(false);
    CHECK_FALSE(c.isVisible());
}

TEST_CASE("Component repaint propagates to parent", "[gui][component]") {
    Component parent;
    Component child;
    parent.addChild(&child);
    parent.clearDirtyFlag();
    CHECK_FALSE(parent.needsRepaint());

    child.repaint();
    CHECK(parent.needsRepaint());
}

TEST_CASE("Component hitTest", "[gui][component]") {
    Component c;
    c.setBounds(0.0f, 0.0f, 100.0f, 50.0f);
    CHECK(c.hitTest(50.0f, 25.0f));
    CHECK_FALSE(c.hitTest(-1.0f, 25.0f));
    CHECK_FALSE(c.hitTest(50.0f, 60.0f));
}

TEST_CASE("Component findComponentAt", "[gui][component]") {
    Component parent;
    parent.setBounds(0.0f, 0.0f, 200.0f, 200.0f);

    Component child;
    child.setBounds(50.0f, 50.0f, 100.0f, 100.0f);
    parent.addChild(&child);

    // Hit inside child
    auto* found = parent.findComponentAt({75.0f, 75.0f});
    CHECK(found == &child);

    // Hit outside child but inside parent
    found = parent.findComponentAt({10.0f, 10.0f});
    CHECK(found == &parent);

    // Hit outside parent
    found = parent.findComponentAt({250.0f, 250.0f});
    CHECK(found == nullptr);
}

TEST_CASE("Component invisible children not hit-tested", "[gui][component]") {
    Component parent;
    parent.setBounds(0.0f, 0.0f, 200.0f, 200.0f);

    Component child;
    child.setBounds(50.0f, 50.0f, 100.0f, 100.0f);
    child.setVisible(false);
    parent.addChild(&child);

    auto* found = parent.findComponentAt({75.0f, 75.0f});
    CHECK(found == &parent);
}
