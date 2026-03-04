# GUI Guide

BODY's GUI system lives in `body/gui/` and provides a component tree, platform-native rendering, and parameter binding. Link the `body::gui` CMake target to use it.

---

## Component Tree

All UI elements inherit from `body::Component`. Components form a parent-child tree. Each component has:

- **Bounds** — position and size relative to its parent
- **Paint** — `paint(Graphics& g)` draws this component
- **Children** — added via `addChild()`, painted after the parent
- **Events** — mouse and keyboard events dispatched through the tree

```cpp
class MyPanel : public body::Component {
public:
    MyPanel() {
        addChild(&label_);
        addChild(&knob_);
    }

    void paint(body::Graphics& g) override {
        g.setColour(body::Colour(0.2f, 0.2f, 0.2f));
        g.fillRect({0, 0, getWidth(), getHeight()});
    }

    void resized() override {
        label_.setBounds(10, 10, 100, 20);
        knob_.setBounds(10, 40, 80, 80);
    }

private:
    body::Label label_{"My Param"};
    body::Knob knob_{someParameter};
};
```

## PluginEditor

`body::PluginEditor` is the top-level component for plugin GUIs. It holds a reference to the `AudioProcessor` and defines the editor size.

```cpp
class MyEditor : public body::PluginEditor {
public:
    explicit MyEditor(body::AudioProcessor& processor,
                      body::FloatParameter& gain)
        : body::PluginEditor(processor), gainKnob_(gain) {
        setSize(400.0f, 300.0f);
        addChild(&gainKnob_);
        resized();
    }

    void paint(body::Graphics& g) override {
        g.setColour(body::Colour(0.15f, 0.15f, 0.15f));
        g.fillRect({0, 0, getWidth(), getHeight()});
    }

    void resized() override {
        gainKnob_.setBounds(160, 90, 80, 80);
    }

private:
    body::Knob gainKnob_;
};
```

Override `createEditor()` in your processor:

```cpp
std::unique_ptr<body::PluginEditor> MyPlugin::createEditor() {
    return std::make_unique<MyEditor>(*this, *gainParam_);
}
```

---

## Built-in Components

### Knob

Rotary control bound to a `Parameter`. Vertical drag changes value. Arc spans 225 degrees (7 o'clock to 5 o'clock).

```cpp
body::Knob knob(myFloatParam);
knob.setValueColour(body::Colour(0.4f, 0.7f, 1.0f));  // Blue arc
knob.setTrackColour(body::Colour(0.3f, 0.3f, 0.3f));   // Background arc
knob.setThumbColour(body::Colour(1.0f, 1.0f, 1.0f));   // Indicator dot
```

### Slider

Linear slider (horizontal or vertical) bound to a `Parameter`.

```cpp
body::Slider slider(myParam, body::Slider::Orientation::kHorizontal);
slider.setFillColour(body::Colour(0.4f, 0.7f, 1.0f));
```

### Label

Static text display, or bind to a `Parameter` to show its current value.

```cpp
body::Label title("My Plugin");
title.setFont(body::Font("Helvetica", 24.0f, body::Font::Weight::kBold));
title.setTextColour(body::Colour(1.0f, 1.0f, 1.0f));
title.setJustification(body::Label::Justification::kCentre);

// Or bind to a parameter to show its value:
body::Label valueDisplay;
valueDisplay.setParameter(&myParam);
```

### Button

Clickable button with a callback.

```cpp
body::Button btn("Reset");
btn.setBackgroundColour(body::Colour(0.3f, 0.3f, 0.3f));
btn.setOnClick([this]() { /* handle click */ });
```

### Toggle

On/off switch bound to a `BoolParameter`.

```cpp
body::Toggle bypass(myBoolParam);
bypass.setOnColour(body::Colour(0.4f, 0.7f, 1.0f));
bypass.setOffColour(body::Colour(0.3f, 0.3f, 0.3f));
```

---

## ParameterAttachment

`ParameterAttachment` bridges parameters and UI components via polling. It reads the parameter's atomic value at the UI refresh rate (60 Hz) and fires a callback when the value changes. No locks involved.

The built-in components (Knob, Slider, Toggle) create their own attachments internally — you don't need to manage them manually. Use `ParameterAttachment` directly only when building custom components.

```cpp
body::ParameterAttachment attachment(myParam);
attachment.setCallback([this](float newValue) {
    // Update your custom component's visual state
    repaint();
});

// Call periodically (the 60Hz timer does this for built-in components)
attachment.checkForUpdates();
```

---

## Colour

`body::Colour` stores RGBA as floats in [0, 1].

```cpp
body::Colour red(1.0f, 0.0f, 0.0f);
body::Colour semiTransparent(1.0f, 1.0f, 1.0f, 0.5f);
body::Colour fromBytes = body::Colour::fromRGBA(255, 128, 0);

// Pre-defined constants in body::Colours namespace
body::Colours::kWhite
body::Colours::kBlack
body::Colours::kRed
body::Colours::kBlue
body::Colours::kGrey
```

## Font

```cpp
body::Font defaultFont;                                    // Helvetica 14pt Regular
body::Font title("Helvetica", 24.0f, body::Font::Weight::kBold);
body::Font small(12.0f);                                   // Default family, 12pt
```

## Graphics

`body::Graphics` is the abstract rendering context passed to `paint()`. Platform backends (CoreGraphics on macOS) provide the implementation.

```cpp
void MyComponent::paint(body::Graphics& g) {
    // Shapes
    g.setColour(body::Colour(0.2f, 0.2f, 0.2f));
    g.fillRect({0, 0, getWidth(), getHeight()});
    g.fillRoundedRect({10, 10, 100, 50}, 8.0f);
    g.fillEllipse({120, 10, 50, 50});

    // Outlines
    g.setColour(body::Colours::kWhite);
    g.drawRect({10, 10, 100, 50}, 2.0f);
    g.drawLine({0, 0}, {100, 100}, 1.0f);

    // Arcs (used by Knob)
    g.drawArc({50, 50}, 30.0f, startAngle, endAngle, 3.0f);

    // Text
    g.drawText("Hello", {10, 70, 200, 20}, body::Font(16.0f));

    // State management
    g.saveState();
    g.translate(50, 50);
    g.setClipBounds({0, 0, 100, 100});
    // ... draw clipped content ...
    g.restoreState();
}
```

---

## Example: GainEditor

The GainPlugin editor demonstrates the full pattern — dark background, centered knob, labels, and a bypass toggle:

```cpp
GainEditor::GainEditor(body::AudioProcessor& processor,
                       body::FloatParameter& gainParam,
                       body::BoolParameter& bypassParam)
    : body::PluginEditor(processor)
    , gainKnob_(gainParam)
    , titleLabel_("Gain")
    , bypassToggle_(bypassParam) {

    setSize(400.0f, 300.0f);

    titleLabel_.setFont(body::Font("Helvetica", 24.0f, body::Font::Weight::kBold));
    titleLabel_.setTextColour(body::Colour(1.0f, 1.0f, 1.0f));
    addChild(&titleLabel_);

    gainKnob_.setValueColour(body::Colour(0.4f, 0.7f, 1.0f));
    addChild(&gainKnob_);

    addChild(&bypassToggle_);
    resized();
}
```

See also: `examples/SynthPlugin/SynthEditor.cpp` for a more complex layout with multiple knob rows.
