# BODY
### Bringing Open Development to You

**BODY is a free, open-source C++ framework for building audio plugins.**

VST3 · AU · CLAP · MIT Licensed · No royalties. No subscriptions. No gatekeeping.

---

## Why BODY Exists

Making music is human. Building tools for music should be too.

The audio plugin development space has long been dominated by frameworks that charge developers to share their work — licensing fees that make perfect sense for large studios and no sense at all for the bedroom producer who just wants to build something, the student learning DSP for the first time, or the indie developer in a country where $50/month is a real barrier.

BODY exists because we believe the tools for making music should be as open as music itself.

No revenue share. No license tiers. No "you can build it, you just can't sell it" fine print. Write a plugin, ship it, give it away, sell it — BODY doesn't take a cut and doesn't ask permission.

That's the whole idea.

---

## What BODY Is

BODY is a modern C++ framework (C++20) for developing audio plugins across all major formats. It provides:

- **Format wrappers** for VST3, AU, and CLAP — write your plugin once, export to all three
- **A parameter system** built for automation, DAW integration, and real-time safety
- **Audio processing primitives** — buffers, sample rate management, block processing
- **MIDI I/O** handling across formats
- **A GUI component system** designed to be intuitive, flexible, and genuinely enjoyable to work with
- **DSP utilities** — filters, oscillators, envelopes, FFT — the building blocks every plugin needs

BODY is not a wrapper around another framework. It is built from scratch, with modern C++ at its core, and designed for the way plugin developers actually work today.

---

## What BODY Is Not

BODY is young. The core vision — write once, export everywhere, with a real parameter system, GUI toolkit, and DSP library — is complete and working. But there's always more to build: more components, more platform backends, more real-world battle-testing.

If you're looking for a mature, decade-old ecosystem — JUCE exists. We're not pretending otherwise.

If you're looking for something modern, growing, and completely free to use — you're in the right place.

---

## Getting Started

See the [Getting Started](docs/getting-started.md) guide for a full walkthrough.

**Requirements:**
- C++20 compatible compiler (Clang 13+, GCC 11+, MSVC 2022+)
- CMake 3.24+
- Git (for submodules)

```bash
git clone --recurse-submodules https://github.com/AustinLambdin/BODY.git
cd BODY
cmake -B build
cmake --build build
```

---

## Project Structure

```
BODY/
├── body/
│   ├── core/         # Parameters, processor base, MIDI, state
│   ├── gui/          # Component system, platform views
│   ├── dsp/          # Oscillator, filters, envelope, FFT
│   └── formats/      # VST3, AU, CLAP wrappers
├── examples/
│   ├── GainPlugin/   # Minimal effect plugin — start here
│   └── SynthPlugin/  # Subtractive synth — demonstrates DSP
├── tests/            # Catch2 unit + integration tests
├── third_party/      # SDKs as git submodules
└── docs/
    ├── getting-started.md
    ├── dsp-guide.md
    └── gui-guide.md
```

---

## Roadmap

- [x] Repo initialized
- [x] CMake build system + submodules
- [x] CLAP wrapper (first format target)
- [x] Core parameter system
- [x] Audio processor base class
- [x] GainPlugin example (validates the whole stack)
- [x] VST3 wrapper
- [x] AU wrapper
- [x] GUI component system
- [x] DSP utility library
- [x] SynthPlugin example (validates DSP library)
- [x] Documentation site

---

## Contributing

BODY is early. That means your contributions matter more here than they would anywhere else — you're not fixing typos in a mature codebase, you're shaping what this becomes.

If you find a bug, open an issue. If you want to build a feature, open a discussion first so we can make sure it fits the direction. If you just want to follow along, star the repo and check back.

All contributions are welcome regardless of experience level. If you're learning C++ through plugin development, this is a good place to do it.

---

## License

MIT License — see [LICENSE](./LICENSE) for details.

Use it. Modify it. Ship plugins with it. Sell them. Give them away.
Just don't remove the license header. That's it.

---

## About

BODY is built and maintained by [Austin](https://github.com/AustinLambdin) — a DevSecOps engineer, music producer, and firm believer that the tools for making music should belong to everyone.

*Bringing Open Development to You.*
