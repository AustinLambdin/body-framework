# DSP Guide

BODY's DSP library lives in `body/dsp/` under the `body::dsp` namespace. Link the `body::dsp` CMake target to use it.

All DSP classes follow the same pattern: `prepare(sampleRate)` → configure → `process()` per sample → `reset()` when needed. Everything is real-time safe.

---

## Oscillator

**Header:** `body/dsp/Oscillator.hpp`

Band-limited oscillator with PolyBLEP anti-aliasing. Supports sine, saw, square, and triangle waveforms. Output range: [-1, 1].

### API

```cpp
namespace body::dsp {
class Oscillator {
    enum class Waveform { Sine, Saw, Square, Triangle };

    void prepare(double sampleRate) noexcept;
    void setFrequency(float hz) noexcept;
    void setWaveform(Waveform waveform) noexcept;
    float process() noexcept;          // Returns next sample
    void reset() noexcept;             // Reset phase to zero
    float getFrequency() const noexcept;
    Waveform getWaveform() const noexcept;
};
}
```

### Example

```cpp
body::dsp::Oscillator osc;
osc.prepare(44100.0);
osc.setWaveform(body::dsp::Oscillator::Waveform::Saw);
osc.setFrequency(440.0f);  // A4

for (int i = 0; i < blockSize; ++i) {
    float sample = osc.process();
    output[i] = sample * 0.5f;  // -6 dB
}
```

---

## Envelope

**Header:** `body/dsp/Envelope.hpp`

ADSR envelope generator with exponential curves using one-pole targeting. Supports retriggering mid-envelope. Output range: [0, 1].

### API

```cpp
namespace body::dsp {
class Envelope {
    enum class Stage { Idle, Attack, Decay, Sustain, Release };

    void prepare(double sampleRate) noexcept;
    void setParameters(float attackSec, float decaySec,
                       float sustainLevel, float releaseSec) noexcept;
    void noteOn() noexcept;            // Start attack
    void noteOff() noexcept;           // Start release
    float process() noexcept;          // Returns next envelope value
    void reset() noexcept;             // Return to idle
    Stage getStage() const noexcept;
    bool isActive() const noexcept;    // True unless idle
};
}
```

### Example

```cpp
body::dsp::Envelope env;
env.prepare(44100.0);
env.setParameters(0.01f, 0.1f, 0.7f, 0.3f);  // A=10ms, D=100ms, S=0.7, R=300ms

env.noteOn();
for (int i = 0; i < blockSize; ++i) {
    output[i] *= env.process();  // Shape amplitude
}
// Later...
env.noteOff();  // Release phase begins
```

---

## SVFilter

**Header:** `body/dsp/SVFilter.hpp`

State-variable filter using trapezoidal integration (Cytomic/Simper algorithm). Better behaved at high frequencies than biquad. Seven filter types available.

### API

```cpp
namespace body::dsp {
class SVFilter {
    enum class Type {
        LowPass, HighPass, BandPass, Notch,
        Peak, LowShelf, HighShelf
    };

    void prepare(double sampleRate) noexcept;
    void setParameters(Type type, float frequency, float q,
                       float gainDb = 0.0f) noexcept;
    float process(float sample) noexcept;
    void reset() noexcept;
};
}
```

### Example

```cpp
body::dsp::SVFilter filter;
filter.prepare(44100.0);
filter.setParameters(body::dsp::SVFilter::Type::LowPass, 2000.0f, 0.707f);

for (int i = 0; i < blockSize; ++i) {
    output[i] = filter.process(input[i]);
}
```

---

## BiquadFilter

**Header:** `body/dsp/BiquadFilter.hpp`

Direct Form II Transposed biquad filter using Audio EQ Cookbook coefficients. Same seven filter types as SVFilter.

### API

```cpp
namespace body::dsp {
class BiquadFilter {
    enum class Type {
        LowPass, HighPass, BandPass, Notch,
        Peak, LowShelf, HighShelf
    };

    void prepare(double sampleRate) noexcept;
    void setParameters(Type type, float frequency, float q,
                       float gainDb = 0.0f) noexcept;
    float process(float sample) noexcept;
    void reset() noexcept;
};
}
```

### Example

```cpp
body::dsp::BiquadFilter eq;
eq.prepare(44100.0);
eq.setParameters(body::dsp::BiquadFilter::Type::Peak, 1000.0f, 1.5f, 6.0f);

for (int i = 0; i < blockSize; ++i)
    output[i] = eq.process(input[i]);
```

---

## DelayLine

**Header:** `body/dsp/DelayLine.hpp`

Circular buffer delay with fractional delay via linear interpolation.

### API

```cpp
namespace body::dsp {
class DelayLine {
    void prepare(double sampleRate, float maxDelaySeconds) noexcept;
    void setDelay(float delaySamples) noexcept;
    void push(float sample) noexcept;
    float read() const noexcept;       // Read at current delay time
    float process(float sample) noexcept;  // push + read combined
    void reset() noexcept;
};
}
```

### Example

```cpp
body::dsp::DelayLine delay;
delay.prepare(44100.0, 1.0f);   // Up to 1 second of delay
delay.setDelay(22050.0f);        // 500ms delay

for (int i = 0; i < blockSize; ++i) {
    float delayed = delay.process(input[i]);
    output[i] = input[i] + delayed * 0.5f;  // Mix dry + 50% wet
}
```

---

## FFT

**Header:** `body/dsp/FFT.hpp`

RAII wrapper around pffft (Pretty Fast FFT). Move-only, supports power-of-two sizes.

### API

```cpp
namespace body::dsp {
class FFT {
    explicit FFT(int size);           // Must be power of 2
    FFT(FFT&& other) noexcept;
    FFT& operator=(FFT&& other) noexcept;

    void forward(const float* input, float* output) const;
    void inverse(const float* input, float* output) const;
    int getSize() const noexcept;
};
}
```

### Example

```cpp
body::dsp::FFT fft(1024);

std::vector<float> timeDomain(1024);
std::vector<float> freqDomain(1024);

// Fill timeDomain with audio...
fft.forward(timeDomain.data(), freqDomain.data());

// Process in frequency domain...
fft.inverse(freqDomain.data(), timeDomain.data());
```

---

## SIMD Utilities

**Header:** `body/dsp/SIMD.hpp` (header-only)

Portable SIMD operations that compile to SSE2 (x86), NEON (ARM), or scalar fallback.

### Functions

```cpp
namespace body::dsp::simd {
    void add(const float* a, const float* b, float* out, int count);
    void multiply(const float* a, const float* b, float* out, int count);
    void applyGain(float* data, float gain, int count);
}
```

### Example

```cpp
std::vector<float> a(256), b(256), result(256);
// Fill a and b...

body::dsp::simd::add(a.data(), b.data(), result.data(), 256);
body::dsp::simd::applyGain(result.data(), 0.5f, 256);
```
