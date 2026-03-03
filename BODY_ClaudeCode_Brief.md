# BODY Framework — Claude Code Project Brief
### Bringing Open Development to You

> **Instructions for Claude Code:** This document is the complete technical specification and project brief for the BODY audio plugin framework. Read it entirely before writing a single line of code. Every architectural decision documented here was made deliberately. When in doubt, refer back to this document. Ask for clarification before deviating from the structure defined here.

---

## 1. Project Overview

**BODY** (Bringing Open Development to You) is an open-source, MIT-licensed C++ audio plugin development framework. It allows developers to write a plugin once and export it as VST3, AU (Audio Unit), and CLAP formats without changing their plugin code.

### Core Mission
To make audio plugin development accessible to everyone — indie developers, students, bedroom producers, and developers in markets where commercial framework licensing fees are prohibitive. BODY takes no revenue share, has no licensing tiers, and places no restrictions on commercial use.

### What Makes BODY Different from JUCE
- **MIT Licensed** — no fees, no revenue share, ever
- **Modern C++20** from day one — no legacy baggage
- **CLAP-first mentality** — CLAP is the open, community-owned format of the future
- **Designed to be learned from** — the codebase should be readable and educational
- **No monolithic God-class design** — clean separation of concerns throughout

### Repository
- **GitHub:** Public from day one
- **License:** MIT
- **Language:** C++20
- **Build System:** CMake 3.24+

---

## 2. Technology Stack

| Component | Choice | Reason |
|---|---|---|
| Language | C++20 | Concepts, ranges, constexpr improvements, span — modern idioms throughout |
| Build System | CMake 3.24+ | Universal support across VST3/AU/CLAP SDKs and all major IDEs |
| Plugin Formats | VST3, AU, CLAP | Full market coverage |
| GUI Rendering | OpenGL (via glad/glfw) initially | Cross-platform, well-understood |
| Package Management | Git Submodules for SDKs | Keeps third-party explicit and version-locked |
| Testing | Catch2 | Lightweight, header-friendly, good C++ test framework |
| CI | GitHub Actions | Free, integrates with the repo naturally |

---

## 3. Repository Structure

This is the canonical structure. Do not deviate from it without strong reason.

```
BODY/
│
├── CMakeLists.txt                  # Root CMake — orchestrates everything
├── README.md                       # Already written — do not modify
├── LICENSE                         # MIT
├── .gitmodules                     # Tracks all submodules
├── .gitignore                      # Standard C++/CMake ignores
│
├── body/                           # THE FRAMEWORK LIBRARY
│   │
│   ├── CMakeLists.txt              # Defines body::body target
│   │
│   ├── core/                       # Platform-agnostic fundamentals
│   │   ├── AudioBuffer.hpp         # Audio buffer abstraction
│   │   ├── AudioProcessor.hpp      # Base class all plugins inherit from
│   │   ├── MidiMessage.hpp         # MIDI message representation
│   │   ├── MidiBuffer.hpp          # Collection of MIDI messages for a block
│   │   ├── Parameter.hpp           # Plugin parameter base + types
│   │   ├── ParameterGroup.hpp      # Groups of parameters
│   │   ├── ProcessContext.hpp      # Data passed to processBlock()
│   │   ├── BusLayout.hpp           # Input/output bus configuration
│   │   └── Identifiers.hpp         # String IDs, type-safe identifiers
│   │
│   ├── gui/                        # GUI component system (Phase 3)
│   │   ├── Component.hpp           # Base GUI component
│   │   ├── ComponentTree.hpp       # Parent/child component hierarchy
│   │   ├── GraphicsContext.hpp     # Drawing API abstraction
│   │   ├── Layout.hpp              # Flexbox-inspired layout engine
│   │   ├── EventSystem.hpp         # Mouse, keyboard, touch events
│   │   └── components/
│   │       ├── Knob.hpp
│   │       ├── Slider.hpp
│   │       ├── Button.hpp
│   │       ├── Label.hpp
│   │       └── LevelMeter.hpp
│   │
│   ├── dsp/                        # DSP building blocks (Phase 3+)
│   │   ├── Filter.hpp              # Biquad, SVF, etc.
│   │   ├── Oscillator.hpp          # Basic waveforms
│   │   ├── Envelope.hpp            # ADSR and friends
│   │   ├── Delay.hpp               # Delay line
│   │   └── FFT.hpp                 # FFT wrapper
│   │
│   └── formats/                    # Format-specific wrappers
│       │
│       ├── clap/
│       │   ├── ClapEntry.hpp       # clap_plugin_entry implementation
│       │   ├── ClapPlugin.hpp      # Bridges body::AudioProcessor → CLAP API
│       │   └── ClapExtensions.hpp  # CLAP extension implementations
│       │
│       ├── vst3/
│       │   ├── Vst3Plugin.hpp      # Bridges body::AudioProcessor → VST3
│       │   ├── Vst3Controller.hpp  # Parameter sync for VST3
│       │   └── Vst3Factory.hpp     # Plugin factory registration
│       │
│       └── au/
│           ├── AuPlugin.hpp        # Bridges body::AudioProcessor → AU
│           └── AuComponent.hpp     # AU component registration
│
├── examples/
│   │
│   └── GainPlugin/                 # First example plugin — validates entire stack
│       ├── CMakeLists.txt
│       ├── GainPlugin.hpp
│       ├── GainPlugin.cpp
│       └── README.md               # How to build and load this plugin
│
├── third_party/                    # All external dependencies — git submodules
│   ├── clap/                       # https://github.com/free-audio/clap
│   ├── vst3sdk/                    # https://github.com/steinbergmedia/vst3sdk
│   ├── catch2/                     # https://github.com/catchorg/Catch2
│   └── glad/                       # OpenGL loader (for GUI phase)
│
├── tests/
│   ├── CMakeLists.txt
│   ├── test_AudioBuffer.cpp
│   ├── test_Parameter.cpp
│   ├── test_MidiBuffer.cpp
│   └── test_ProcessContext.cpp
│
├── docs/
│   ├── getting-started.md
│   ├── architecture.md
│   └── writing-your-first-plugin.md
│
└── .github/
    └── workflows/
        ├── build-windows.yml
        ├── build-macos.yml
        └── build-linux.yml
```

---

## 4. Build System Specification

### Root CMakeLists.txt Requirements

```cmake
cmake_minimum_required(VERSION 3.24)
project(BODY VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
```

- **C++20 is non-negotiable.** Do not allow fallback to older standards.
- All targets should be compiled with strict warnings: `-Wall -Wextra -Wpedantic` (GCC/Clang), `/W4` (MSVC)
- The framework itself (`body::body`) is an **INTERFACE or STATIC library** — plugin developers link against it
- Format wrappers (CLAP, VST3, AU) are separate CMake targets that depend on `body::body`
- Examples are optional and toggled with `BODY_BUILD_EXAMPLES` CMake option (default ON)
- Tests are toggled with `BODY_BUILD_TESTS` CMake option (default ON)

### CMake Targets to Define

| Target | Type | Description |
|---|---|---|
| `body::core` | STATIC/INTERFACE | Core framework headers + implementation |
| `body::clap` | STATIC | CLAP format wrapper |
| `body::vst3` | STATIC | VST3 format wrapper |
| `body::au` | STATIC | AU wrapper (macOS only) |
| `body::body` | INTERFACE | Meta-target: links all of the above |
| `body_tests` | EXECUTABLE | Test suite via Catch2 |
| `GainPlugin_CLAP` | MODULE | Example plugin, CLAP format |
| `GainPlugin_VST3` | MODULE | Example plugin, VST3 format |

### Platform Guards
```cmake
if(APPLE)
    # Include AU targets
endif()

if(WIN32 OR APPLE OR UNIX)
    # VST3 and CLAP on all platforms
endif()
```

---

## 5. Core Architecture

### 5.1 The AudioProcessor Base Class

This is the heart of BODY. Every plugin is a class that inherits from `body::AudioProcessor`. It should be clean, obvious, and hard to misuse.

```cpp
namespace body {

class AudioProcessor {
public:
    virtual ~AudioProcessor() = default;

    // Called once when the plugin is initialized
    virtual void prepare(double sampleRate, int maxBlockSize) = 0;

    // Called for every block of audio — THE hot path
    // Must be real-time safe — no allocations, no locks, no I/O
    virtual void processBlock(ProcessContext& context) = 0;

    // Called when the plugin is reset (transport stop, etc.)
    virtual void reset() {}

    // Plugin metadata — must be implemented
    virtual std::string getName() const = 0;
    virtual std::string getVendor() const = 0;
    virtual std::string getVersion() const = 0;

    // Parameter access
    ParameterGroup& getParameters() { return parameters_; }
    const ParameterGroup& getParameters() const { return parameters_; }

    // Bus layout — override to customize
    virtual BusLayout getDefaultBusLayout() const;
    virtual bool isBusLayoutSupported(const BusLayout& layout) const;

protected:
    ParameterGroup parameters_;
};

} // namespace body
```

**Design rules for AudioProcessor:**
- `processBlock()` must NEVER allocate memory
- `processBlock()` must NEVER call virtual functions if avoidable in the hot path
- `prepare()` is where allocations happen
- Parameters are stored in `parameters_` — the format wrappers read from this

### 5.2 ProcessContext

The object passed into `processBlock()`. Contains everything the plugin needs for one block.

```cpp
namespace body {

struct ProcessContext {
    // Audio buffers
    AudioBuffer<float>  audioIn;
    AudioBuffer<float>  audioOut;

    // MIDI
    MidiBuffer          midiIn;
    MidiBuffer          midiOut;

    // Transport
    double              sampleRate      = 44100.0;
    int                 blockSize       = 0;
    bool                isPlaying       = false;
    bool                isRecording     = false;
    double              bpm             = 120.0;
    double              ppqPosition     = 0.0;    // Pulses per quarter note
    int64_t             samplePosition  = 0;      // Absolute sample position

    // Timing
    bool                isRealtime      = true;
};

} // namespace body
```

### 5.3 AudioBuffer

Template class, float by default. Designed for zero-copy where possible.

```cpp
namespace body {

template<typename SampleType>
class AudioBuffer {
public:
    AudioBuffer() = default;
    AudioBuffer(int numChannels, int numSamples);

    // Non-owning view constructor (used by format wrappers to wrap host buffers)
    AudioBuffer(SampleType** channelData, int numChannels, int numSamples);

    int getNumChannels() const noexcept;
    int getNumSamples() const noexcept;

    SampleType* getChannelPointer(int channel) noexcept;
    const SampleType* getChannelPointer(int channel) const noexcept;

    std::span<SampleType> getChannel(int channel) noexcept;
    std::span<const SampleType> getChannel(int channel) const noexcept;

    void clear() noexcept;
    void clear(int channel, int startSample, int numSamples) noexcept;

    void applyGain(SampleType gain) noexcept;
    void applyGain(int channel, int startSample, int numSamples, SampleType gain) noexcept;

    void addFrom(const AudioBuffer& source, SampleType gain = SampleType(1)) noexcept;

private:
    std::vector<std::vector<SampleType>> ownedData_;
    std::vector<SampleType*>             channelPointers_;
    int                                  numChannels_ = 0;
    int                                  numSamples_  = 0;
};

} // namespace body
```

**Key design note:** Use `std::span` throughout — it's a C++20 idiom that eliminates pointer/length pairs and is zero-cost. The non-owning view constructor is critical — when format wrappers receive host buffers, we wrap them without copying.

### 5.4 Parameter System

Parameters are one of the most critical pieces of the framework. They bridge plugin state ↔ DAW automation ↔ host communication.

```cpp
namespace body {

// Type-safe parameter ID
struct ParameterID {
    std::string id;
    explicit ParameterID(std::string id) : id(std::move(id)) {}
};

// Base parameter
class Parameter {
public:
    enum class Type { Float, Int, Bool, Choice };

    Parameter(ParameterID id, std::string name, std::string units = "");
    virtual ~Parameter() = default;

    const std::string& getId() const noexcept;
    const std::string& getName() const noexcept;

    // Normalized value [0.0, 1.0] — what the host sees
    virtual float getNormalized() const noexcept = 0;
    virtual void  setNormalized(float normalized) noexcept = 0;

    // Denormalized (actual) value — what your plugin code reads
    virtual float getDenormalized() const noexcept = 0;
    virtual void  setDenormalized(float value) noexcept = 0;

    // Human-readable string for the current value
    virtual std::string getDisplayValue() const = 0;

    virtual Type getType() const noexcept = 0;

protected:
    ParameterID id_;
    std::string name_;
    std::string units_;
};

// Float parameter with range
class FloatParameter : public Parameter {
public:
    FloatParameter(ParameterID id,
                   std::string name,
                   float minValue,
                   float maxValue,
                   float defaultValue,
                   std::string units = "");

    float getMinValue() const noexcept;
    float getMaxValue() const noexcept;
    float getDefaultValue() const noexcept;

    float getNormalized() const noexcept override;
    void  setNormalized(float normalized) noexcept override;
    float getDenormalized() const noexcept override;
    void  setDenormalized(float value) noexcept override;

    std::string getDisplayValue() const override;

private:
    float min_, max_, default_, current_;
    std::atomic<float> atomicValue_;  // Real-time safe reads from audio thread
};

// Also implement: IntParameter, BoolParameter, ChoiceParameter

// Parameter group — contains and owns parameters
class ParameterGroup {
public:
    template<typename ParamType, typename... Args>
    ParamType& addParameter(Args&&... args);

    Parameter*       findParameter(const std::string& id);
    const Parameter* findParameter(const std::string& id) const;

    std::vector<Parameter*> getAllParameters();

    // Range-based iteration
    auto begin();
    auto end();

private:
    std::vector<std::unique_ptr<Parameter>> parameters_;
    std::unordered_map<std::string, Parameter*> lookup_;
};

} // namespace body
```

**Critical design note:** Parameter values read from the audio thread (`getDenormalized()` inside `processBlock()`) must use `std::atomic<float>`. Never use a mutex on the audio thread. This is a hard rule.

### 5.5 MidiMessage and MidiBuffer

```cpp
namespace body {

struct MidiMessage {
    enum class Type {
        NoteOn, NoteOff, ControlChange, PitchBend,
        ProgramChange, ChannelPressure, PolyPressure,
        SysEx, Unknown
    };

    uint8_t  data[3]    = {};
    int      numBytes   = 0;
    int      samplePosition = 0;   // Position within the current block

    Type     getType() const noexcept;
    uint8_t  getChannel() const noexcept;     // 0-15
    uint8_t  getNoteNumber() const noexcept;
    uint8_t  getVelocity() const noexcept;
    uint8_t  getCCNumber() const noexcept;
    uint8_t  getCCValue() const noexcept;
    float    getPitchBendValue() const noexcept;  // -1.0 to 1.0

    // Factory methods
    static MidiMessage noteOn(uint8_t channel, uint8_t note, uint8_t velocity, int samplePos = 0);
    static MidiMessage noteOff(uint8_t channel, uint8_t note, uint8_t velocity = 0, int samplePos = 0);
    static MidiMessage cc(uint8_t channel, uint8_t number, uint8_t value, int samplePos = 0);
};

class MidiBuffer {
public:
    void addMessage(MidiMessage message);
    void clear() noexcept;
    bool isEmpty() const noexcept;
    int  size() const noexcept;

    // Iterate in sample-position order
    auto begin() const;
    auto end() const;

    // Get all messages in a sample range
    std::span<const MidiMessage> getMessagesInRange(int startSample, int endSamples) const;

private:
    std::vector<MidiMessage> messages_;
};

} // namespace body
```

---

## 6. Format Wrapper Architecture

Each format wrapper does one job: **translate between the host's plugin API and `body::AudioProcessor`**.

The wrapper should know nothing about DSP. The AudioProcessor should know nothing about CLAP, VST3, or AU. Clean separation.

### 6.1 CLAP Wrapper

CLAP is the primary and most important target. It is open source, community-maintained, and the format BODY most naturally aligns with philosophically.

The CLAP wrapper must implement:
- `clap_plugin_t` lifecycle: `init`, `destroy`, `activate`, `deactivate`, `start_processing`, `stop_processing`, `process`, `reset`
- `clap_plugin_params` extension — maps `body::ParameterGroup` to CLAP parameter info
- `clap_plugin_audio_ports` extension — maps `body::BusLayout` to CLAP port info
- `clap_plugin_note_ports` extension — if the plugin has MIDI
- `clap_plugin_factory` — for registering plugins with the host

**CLAP process function flow:**
```
host calls clap_plugin.process()
    → wrapper extracts audio buffers from clap_process_t
    → wrapper wraps them in body::AudioBuffer (non-owning view, no copy)
    → wrapper extracts MIDI events from in_events queue
    → wrapper builds body::MidiBuffer
    → wrapper builds body::ProcessContext
    → wrapper calls audioProcessor.processBlock(context)
    → wrapper writes MIDI output back to out_events queue
    → done
```

### 6.2 VST3 Wrapper

VST3 is more complex than CLAP due to its separated Processor/Controller architecture. The wrapper must implement:

- `IComponent` — the audio processing side
- `IAudioProcessor` — `setupProcessing`, `process`, `setBusArrangements`
- `IEditController` — parameter management, UI communication
- `IPluginFactory` — registration

The Processor/Controller split is a VST3 design requirement. Our wrapper must bridge this to the unified `body::AudioProcessor` model without leaking VST3 concepts into plugin code.

**Important:** VST3 requires plugin state to be serializable. The wrapper handles this by iterating `body::ParameterGroup` and serializing all parameter values.

### 6.3 AU Wrapper (macOS only)

Audio Units are Apple's format, built on CoreAudio. The wrapper uses `AUAudioUnit` (the modern V3 API, not the deprecated V2).

- Implement `AUAudioUnit` subclass
- Map `body::ParameterGroup` to `AUParameterTree`
- Handle `allocateRenderResourcesAndReturnError` → call `prepare()`
- Implement `internalRenderBlock` → call `processBlock()`

**Build guard:** All AU code must be wrapped in `#if defined(__APPLE__)` and only compiled on macOS. The CMake target for AU should only exist on Apple platforms.

---

## 7. The GainPlugin Example

This is the first plugin built with BODY. It validates the entire stack end-to-end. It must be simple enough to be readable in 5 minutes but complete enough to exercise all of the core systems.

**What it does:** Applies a gain (volume) multiplier to the audio signal.

**What it exercises:**
- AudioProcessor subclass
- FloatParameter (gain amount in dB)
- BoolParameter (bypass)
- processBlock() with real audio
- Building as CLAP, VST3, and AU

```cpp
// GainPlugin.hpp
#include <body/body.hpp>

class GainPlugin : public body::AudioProcessor {
public:
    GainPlugin();

    std::string getName() const override    { return "Gain"; }
    std::string getVendor() const override  { return "BODY Examples"; }
    std::string getVersion() const override { return "1.0.0"; }

    void prepare(double sampleRate, int maxBlockSize) override;
    void processBlock(body::ProcessContext& context) override;
    void reset() override;

private:
    body::FloatParameter* gainParam_   = nullptr;
    body::BoolParameter*  bypassParam_ = nullptr;

    // Smoothed gain value — avoids clicks when gain changes
    float currentGain_   = 1.0f;
    float targetGain_    = 1.0f;
    float smoothingCoeff_ = 0.0f;
};
```

**processBlock() implementation target:**
```cpp
void GainPlugin::processBlock(body::ProcessContext& ctx) {
    if (bypassParam_->getValue()) return;

    float targetDb    = gainParam_->getDenormalized();
    targetGain_       = std::pow(10.0f, targetDb / 20.0f);

    for (int ch = 0; ch < ctx.audioOut.getNumChannels(); ++ch) {
        auto samples = ctx.audioOut.getChannel(ch);
        for (auto& sample : samples) {
            currentGain_ += smoothingCoeff_ * (targetGain_ - currentGain_);
            sample       *= currentGain_;
        }
    }
}
```

---

## 8. Testing Strategy

Every core class needs tests. Use Catch2.

### Required Tests (Phase 1)

| Test File | What It Tests |
|---|---|
| `test_AudioBuffer.cpp` | Construction, channel access, clear, applyGain, non-owning view |
| `test_Parameter.cpp` | Normalization/denormalization, atomic access, display strings |
| `test_MidiBuffer.cpp` | Message insertion, ordering by samplePosition, range queries |
| `test_ProcessContext.cpp` | Construction, default values |

### Test Infrastructure

```cmake
# tests/CMakeLists.txt
if(BODY_BUILD_TESTS)
    find_package(Catch2 3 REQUIRED)
    add_executable(body_tests
        test_AudioBuffer.cpp
        test_Parameter.cpp
        test_MidiBuffer.cpp
        test_ProcessContext.cpp
    )
    target_link_libraries(body_tests PRIVATE body::core Catch2::Catch2WithMain)
    include(CTest)
    include(Catch)
    catch_discover_tests(body_tests)
endif()
```

---

## 9. CI/CD (GitHub Actions)

Three workflow files — one per platform.

### `build-macos.yml`
```yaml
name: Build (macOS)
on: [push, pull_request]
jobs:
  build:
    runs-on: macos-latest
    steps:
      - uses: actions/checkout@v4
        with:
          submodules: recursive
      - name: Configure
        run: cmake -B build -DBODY_BUILD_TESTS=ON -DBODY_BUILD_EXAMPLES=ON
      - name: Build
        run: cmake --build build --config Release
      - name: Test
        run: cd build && ctest --output-on-failure
```

### `build-windows.yml`
Same structure, `runs-on: windows-latest`

### `build-linux.yml`
Same structure, `runs-on: ubuntu-latest`
Add step: `sudo apt-get install -y libasound2-dev` (ALSA headers for Linux audio)

---

## 10. Coding Standards

These are non-negotiable. Every line of code in BODY follows these rules.

### Naming Conventions
```cpp
// Types (classes, structs, enums): PascalCase
class AudioProcessor {};
struct ProcessContext {};

// Functions and methods: camelCase
void processBlock();
float getDenormalized();

// Member variables: camelCase with trailing underscore
float currentGain_;
int numChannels_;

// Constants: kPascalCase
constexpr int kMaxChannels = 64;

// Namespaces: lowercase
namespace body {}
namespace body::dsp {}

// Macros: SCREAMING_SNAKE (avoid macros where possible)
#define BODY_VERSION_MAJOR 0
```

### File Organization
- One class per file where practical
- `.hpp` for headers, `.cpp` for implementations
- All framework code in `body/` directory
- All headers use `#pragma once`
- No `using namespace` in headers — ever

### C++20 Idioms to Use
- `std::span` instead of pointer + length
- `[[nodiscard]]` on all functions where ignoring return value is a bug
- `constexpr` wherever possible
- Concepts for template constraints when needed
- `std::atomic` for any value shared between audio thread and UI thread
- Range-based for loops over raw index loops

### C++ Patterns to Avoid
- No raw `new`/`delete` — use smart pointers or stack allocation
- No `std::shared_ptr` in the audio hot path (atomic ref count is too expensive)
- No exceptions in audio thread code
- No virtual functions in the hot path if avoidable
- No `dynamic_cast` in the hot path
- No mutexes on the audio thread — use lock-free structures or atomics

### Comments
- Public API functions get doc comments:
```cpp
/// @brief Apply gain to all channels in the buffer
/// @param gain Linear gain multiplier. 1.0 = unity, 0.0 = silence
/// @note Real-time safe — no allocations
void applyGain(float gain) noexcept;
```
- Implementation comments explain *why*, not *what*
- TODO comments include the author and date: `// TODO(austin 2025-01): replace with SIMD`

---

## 11. Phase Roadmap

### Phase 1 — The Skeleton (Do This First)
**Goal:** A GainPlugin that compiles and loads in at least one DAW.

Tasks:
1. Set up CMakeLists.txt (root + body/ + examples/)
2. Add git submodules: clap, vst3sdk, catch2
3. Implement `AudioBuffer<float>`
4. Implement `Parameter`, `FloatParameter`, `BoolParameter`
5. Implement `ParameterGroup`
6. Implement `MidiMessage`, `MidiBuffer`
7. Implement `ProcessContext`
8. Implement `AudioProcessor` base class
9. Implement `BusLayout`
10. Build CLAP wrapper (first format)
11. Implement GainPlugin using CLAP wrapper
12. Verify it loads in a CLAP-compatible DAW (Bitwig, Reaper, etc.)
13. Write Phase 1 tests
14. Set up GitHub Actions CI

### Phase 2 — Full Format Support
**Goal:** GainPlugin builds as VST3, AU, and CLAP.

Tasks:
1. VST3 wrapper (Processor + Controller split)
2. AU wrapper (macOS only)
3. Plugin metadata system (categories, features)
4. State save/restore across all formats
5. Full CI on all three platforms

### Phase 3 — GUI System
**Goal:** GainPlugin has a custom GUI that works across formats.

Tasks:
1. OpenGL context per platform (CoreGL/WGL/GLX)
2. Component base class + tree
3. Layout engine (flexbox-inspired)
4. Event system (mouse, keyboard)
5. Core components: Knob, Slider, Button, Label, LevelMeter
6. Connect GUI parameters to `body::Parameter` with thread-safe messaging

### Phase 4 — DSP Library
**Goal:** Developers building plugins with BODY have a solid DSP toolkit.

Tasks:
1. Biquad filter (LPF, HPF, BPF, notch, peak, shelf)
2. State-variable filter
3. ADSR envelope
4. Basic oscillators (sine, saw, square, triangle)
5. Delay line (circular buffer based)
6. FFT wrapper (around pffft or KissFFT)
7. SIMD utilities for vectorized processing

---

## 12. Third-Party Dependencies

All dependencies are git submodules. No package manager required.

| Dependency | URL | Purpose |
|---|---|---|
| clap | https://github.com/free-audio/clap | CLAP plugin format headers |
| vst3sdk | https://github.com/steinbergmedia/vst3sdk | VST3 SDK |
| catch2 | https://github.com/catchorg/Catch2 | Testing framework |

**Adding submodules:**
```bash
git submodule add https://github.com/free-audio/clap third_party/clap
git submodule add https://github.com/steinbergmedia/vst3sdk third_party/vst3sdk
git submodule add https://github.com/catchorg/Catch2 third_party/catch2
```

**AU headers** come from the macOS system SDK — no submodule needed. CMake finds them via `find_library(AUDIO_UNIT AudioUnit)` and `find_library(CORE_AUDIO CoreAudio)`.

---

## 13. What NOT to Build Right Now

Explicitly out of scope for Phase 1. Do not implement these yet, do not create files for them:

- GUI system (Phase 3)
- DSP library (Phase 4)
- Plugin scanner or host
- Network features
- Python bindings
- Any abstraction that wraps the framework wrapping the format — keep it flat

---

## 14. The Non-Negotiables

These rules must be respected in every PR, every file, every function:

1. **MIT License header in every source file**
2. **No allocations in processBlock() — ever**
3. **No mutexes on the audio thread — ever**
4. **C++20 minimum — no legacy fallback**
5. **All three formats build — no format is second-class**
6. **Tests before merging new core systems**
7. **The framework must be learnable from reading it** — clarity over cleverness

---

## 15. First Task for Claude Code

Read this document entirely. Then execute Phase 1 in order:

1. Create `CMakeLists.txt` (root)
2. Create `body/CMakeLists.txt`
3. Create all core headers: `AudioBuffer.hpp`, `AudioProcessor.hpp`, `Parameter.hpp`, `ParameterGroup.hpp`, `MidiMessage.hpp`, `MidiBuffer.hpp`, `ProcessContext.hpp`, `BusLayout.hpp`
4. Implement all of the above in corresponding `.cpp` files
5. Add the CLAP submodule and build the CLAP wrapper
6. Implement GainPlugin
7. Write tests for all core classes
8. Set up GitHub Actions

**Do not skip ahead to the GUI. Do not add features not described here. Build what is described, build it well, and make it compile cleanly with zero warnings on all three platforms.**

---

*BODY — Bringing Open Development to You*
*MIT Licensed. Built for everyone.*
