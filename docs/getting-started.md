# Getting Started with BODY

## Prerequisites

- **C++20 compiler:** Clang 13+, GCC 11+, or MSVC 2022+
- **CMake 3.24+**
- **Git** (for submodules)
- **macOS:** Xcode Command Line Tools (for AU support and CoreGraphics)
- **Linux:** ALSA development headers (`libasound2-dev` on Debian/Ubuntu)
- **Windows:** Visual Studio 2022 with C++ workload

## Clone & Build

```bash
git clone --recurse-submodules https://github.com/yourusername/BODY.git
cd BODY
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

The `--recurse-submodules` flag pulls the CLAP, VST3 SDK, pffft, and Catch2 dependencies into `third_party/`.

## Run Tests

```bash
ctest --test-dir build --output-on-failure
```

All tests should pass. The test suite covers core types, GUI primitives, DSP modules, and integration scenarios.

## Project Layout

```
BODY/
├── body/
│   ├── core/           # AudioProcessor, Parameter, MidiBuffer, StateSerializer
│   ├── gui/            # Component, PluginEditor, Graphics, Colour, Font
│   │   ├── components/ # Knob, Slider, Label, Button, Toggle
│   │   └── platform/   # PlatformViewMac (CoreGraphics backend)
│   ├── dsp/            # Oscillator, SVFilter, BiquadFilter, Envelope, DelayLine, FFT
│   └── formats/
│       ├── clap/       # CLAP format wrapper
│       ├── vst3/       # VST3 format wrapper
│       └── au/         # Audio Unit format wrapper (macOS)
├── examples/
│   ├── GainPlugin/     # Minimal effect — good starting point
│   └── SynthPlugin/    # Subtractive synth — demonstrates DSP library
├── tests/              # Catch2 test suite
├── third_party/        # Git submodules (clap, vst3sdk, pffft, catch2)
└── docs/               # You are here
```

## Your First Plugin

The fastest way to learn BODY is to study `examples/GainPlugin/`. Here's the pattern:

### 1. Subclass `AudioProcessor`

```cpp
#include "body/core/AudioProcessor.hpp"

class MyPlugin : public body::AudioProcessor {
public:
    MyPlugin() {
        // Register parameters in the constructor
        gainParam_ = &parameters_.addParameter<body::FloatParameter>(
            body::ParameterID("gain"), "Gain", -60.0f, 24.0f, 0.0f, "dB");
    }

    std::string getName() const override { return "MyPlugin"; }
    std::string getVendor() const override { return "My Company"; }
    std::string getVersion() const override { return "1.0.0"; }

    void prepare(double sampleRate, int maxBlockSize) override {
        // Allocate resources, compute coefficients
    }

    void processBlock(body::ProcessContext& ctx) override {
        // Real-time audio processing — no allocations, no locks
        float gain = std::pow(10.0f, gainParam_->getDenormalized() / 20.0f);
        for (int ch = 0; ch < ctx.audioOut.getNumChannels(); ++ch) {
            for (auto& sample : ctx.audioOut.getChannel(ch))
                sample *= gain;
        }
    }

private:
    body::FloatParameter* gainParam_ = nullptr;
};
```

### 2. Add a GUI (optional)

Subclass `PluginEditor`, add `Knob`/`Slider`/`Label` children, and override `createEditor()` in your processor.

### 3. Register for each format

Create thin registration files — one per format:

- `MyPlugin_CLAP.cpp` — uses `BODY_REGISTER_CLAP_PLUGIN(...)`
- `MyPlugin_VST3.cpp` — uses `BODY_REGISTER_VST3_PLUGIN(...)`
- `MyPlugin_AU.mm` — uses `BODY_REGISTER_AU_PLUGIN(...)` (macOS only)

### 4. CMakeLists.txt

Follow the pattern in `examples/GainPlugin/CMakeLists.txt` — create one `MODULE` library per format, link the appropriate `body::clap`, `body::vst3`, or `body::au` target.

## CMake Options

| Option | Default | Description |
|--------|---------|-------------|
| `BODY_BUILD_TESTS` | `ON` | Build the Catch2 test suite |
| `BODY_BUILD_EXAMPLES` | `ON` | Build GainPlugin and SynthPlugin |
| `BODY_ENABLE_VST3` | `ON` | Enable VST3 format support |
| `BODY_ENABLE_AU` | `ON` | Enable Audio Unit support (macOS only) |

Example with all options:

```bash
cmake -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DBODY_BUILD_TESTS=OFF \
  -DBODY_BUILD_EXAMPLES=ON \
  -DBODY_ENABLE_VST3=ON
```
