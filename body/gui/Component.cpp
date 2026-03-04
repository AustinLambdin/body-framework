// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "Component.hpp"
#include <algorithm>

namespace body {

Component::~Component() {
    for (auto* child : children_) {
        child->parent_ = nullptr;
    }
}

void Component::paint(Graphics& /*g*/) {}

void Component::paintEntireComponent(Graphics& g) {
    if (!visible_) return;

    g.saveState();
    g.translate(bounds_.x, bounds_.y);
    g.setClipBounds({0.0f, 0.0f, bounds_.width, bounds_.height});

    paint(g);

    for (auto* child : children_) {
        child->paintEntireComponent(g);
    }

    g.restoreState();
    dirty_ = false;
}

void Component::repaint() {
    dirty_ = true;
    if (parent_) parent_->repaint();
}

void Component::setBounds(const Rectangle<float>& bounds) {
    bounds_ = bounds;
    resized();
}

void Component::setBounds(float x, float y, float w, float h) {
    setBounds({x, y, w, h});
}

void Component::resized() {}

void Component::addChild(Component* child) {
    if (!child || child == this) return;
    if (child->parent_) child->parent_->removeChild(child);
    child->parent_ = this;
    children_.push_back(child);
    repaint();
}

void Component::removeChild(Component* child) {
    if (!child) return;
    auto it = std::find(children_.begin(), children_.end(), child);
    if (it != children_.end()) {
        (*it)->parent_ = nullptr;
        children_.erase(it);
        repaint();
    }
}

void Component::setVisible(bool visible) {
    if (visible_ != visible) {
        visible_ = visible;
        repaint();
    }
}

bool Component::hitTest(float x, float y) const {
    return x >= 0.0f && x < bounds_.width && y >= 0.0f && y < bounds_.height;
}

Component* Component::findComponentAt(Point<float> position) {
    if (!visible_) return nullptr;
    if (!hitTest(position.x - bounds_.x, position.y - bounds_.y)) return nullptr;

    // Check children in reverse order (front-most first)
    Point<float> local{position.x - bounds_.x, position.y - bounds_.y};
    for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
        if (auto* found = (*it)->findComponentAt(local)) {
            return found;
        }
    }
    return this;
}

void Component::mouseDown(const MouseEvent& /*event*/) {}
void Component::mouseUp(const MouseEvent& /*event*/) {}
void Component::mouseDrag(const MouseEvent& /*event*/) {}
void Component::mouseMove(const MouseEvent& /*event*/) {}
void Component::mouseWheel(const MouseEvent& /*event*/) {}
void Component::keyDown(const KeyEvent& /*event*/) {}
void Component::keyUp(const KeyEvent& /*event*/) {}

} // namespace body
