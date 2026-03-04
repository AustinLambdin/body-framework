// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include "Graphics.hpp"
#include "MouseEvent.hpp"
#include "KeyEvent.hpp"
#include "Rectangle.hpp"
#include <vector>

namespace body {

/// @brief Base class for all UI components
///
/// Components form a tree. Each component has bounds relative to its parent.
/// Children are stored as raw pointers — the owner (typically PluginEditor)
/// holds them as member variables.
class Component {
public:
    Component() = default;
    virtual ~Component();

    // --- Painting ---

    virtual void paint(Graphics& g);
    void paintEntireComponent(Graphics& g);

    void repaint();
    [[nodiscard]] bool needsRepaint() const noexcept { return dirty_; }
    void clearDirtyFlag() noexcept { dirty_ = false; }

    // --- Bounds ---

    void setBounds(const Rectangle<float>& bounds);
    void setBounds(float x, float y, float w, float h);
    [[nodiscard]] const Rectangle<float>& getBounds() const noexcept { return bounds_; }

    [[nodiscard]] float getWidth() const noexcept { return bounds_.width; }
    [[nodiscard]] float getHeight() const noexcept { return bounds_.height; }

    virtual void resized();

    // --- Child management ---

    void addChild(Component* child);
    void removeChild(Component* child);
    [[nodiscard]] Component* getParent() const noexcept { return parent_; }
    [[nodiscard]] const std::vector<Component*>& getChildren() const noexcept { return children_; }

    // --- Visibility ---

    void setVisible(bool visible);
    [[nodiscard]] bool isVisible() const noexcept { return visible_; }

    // --- Hit testing ---

    [[nodiscard]] virtual bool hitTest(float x, float y) const;
    [[nodiscard]] Component* findComponentAt(Point<float> position);

    // --- Event handling ---

    virtual void mouseDown(const MouseEvent& event);
    virtual void mouseUp(const MouseEvent& event);
    virtual void mouseDrag(const MouseEvent& event);
    virtual void mouseMove(const MouseEvent& event);
    virtual void mouseWheel(const MouseEvent& event);
    virtual void keyDown(const KeyEvent& event);
    virtual void keyUp(const KeyEvent& event);

private:
    Rectangle<float> bounds_;
    Component* parent_ = nullptr;
    std::vector<Component*> children_;
    bool visible_ = true;
    bool dirty_ = true;
};

} // namespace body
