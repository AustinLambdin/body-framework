// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#pragma once

#include <cassert>
#include <cstring>
#include <span>
#include <vector>

namespace body {

/// @brief Audio buffer with owning and non-owning modes
/// @tparam SampleType The sample format (typically float)
///
/// Owning mode allocates and manages its own memory.
/// Non-owning mode wraps external pointers (used by format wrappers).
template<typename SampleType>
class AudioBuffer {
public:
    AudioBuffer() = default;

    /// @brief Construct an owning buffer
    /// @param numChannels Number of audio channels
    /// @param numSamples Number of samples per channel
    AudioBuffer(int numChannels, int numSamples)
        : numChannels_(numChannels), numSamples_(numSamples) {
        ownedData_.resize(static_cast<size_t>(numChannels));
        channelPointers_.resize(static_cast<size_t>(numChannels));
        for (int ch = 0; ch < numChannels; ++ch) {
            ownedData_[static_cast<size_t>(ch)].resize(static_cast<size_t>(numSamples), SampleType{});
            channelPointers_[static_cast<size_t>(ch)] = ownedData_[static_cast<size_t>(ch)].data();
        }
    }

    /// @brief Construct a non-owning view wrapping external channel data
    /// @param channelData Array of pointers to channel sample data
    /// @param numChannels Number of channels
    /// @param numSamples Number of samples per channel
    AudioBuffer(SampleType** channelData, int numChannels, int numSamples)
        : numChannels_(numChannels), numSamples_(numSamples) {
        channelPointers_.resize(static_cast<size_t>(numChannels));
        for (int ch = 0; ch < numChannels; ++ch) {
            channelPointers_[static_cast<size_t>(ch)] = channelData[ch];
        }
    }

    [[nodiscard]] int getNumChannels() const noexcept { return numChannels_; }
    [[nodiscard]] int getNumSamples() const noexcept { return numSamples_; }

    /// @brief Get raw pointer to a channel's sample data
    [[nodiscard]] SampleType* getChannelPointer(int channel) noexcept {
        assert(channel >= 0 && channel < numChannels_);
        return channelPointers_[static_cast<size_t>(channel)];
    }

    /// @brief Get const raw pointer to a channel's sample data
    [[nodiscard]] const SampleType* getChannelPointer(int channel) const noexcept {
        assert(channel >= 0 && channel < numChannels_);
        return channelPointers_[static_cast<size_t>(channel)];
    }

    /// @brief Get a span view of a channel's sample data
    [[nodiscard]] std::span<SampleType> getChannel(int channel) noexcept {
        assert(channel >= 0 && channel < numChannels_);
        return {channelPointers_[static_cast<size_t>(channel)],
                static_cast<size_t>(numSamples_)};
    }

    /// @brief Get a const span view of a channel's sample data
    [[nodiscard]] std::span<const SampleType> getChannel(int channel) const noexcept {
        assert(channel >= 0 && channel < numChannels_);
        return {channelPointers_[static_cast<size_t>(channel)],
                static_cast<size_t>(numSamples_)};
    }

    /// @brief Clear all channels to zero
    void clear() noexcept {
        for (int ch = 0; ch < numChannels_; ++ch) {
            std::memset(channelPointers_[static_cast<size_t>(ch)], 0,
                        static_cast<size_t>(numSamples_) * sizeof(SampleType));
        }
    }

    /// @brief Clear a range of samples on a specific channel
    void clear(int channel, int startSample, int numSamples) noexcept {
        assert(channel >= 0 && channel < numChannels_);
        assert(startSample >= 0 && startSample + numSamples <= numSamples_);
        std::memset(channelPointers_[static_cast<size_t>(channel)] + startSample, 0,
                    static_cast<size_t>(numSamples) * sizeof(SampleType));
    }

    /// @brief Apply gain to all channels
    /// @param gain Linear gain multiplier. 1.0 = unity, 0.0 = silence
    /// @note Real-time safe
    void applyGain(SampleType gain) noexcept {
        for (int ch = 0; ch < numChannels_; ++ch) {
            auto* data = channelPointers_[static_cast<size_t>(ch)];
            for (int s = 0; s < numSamples_; ++s) {
                data[s] *= gain;
            }
        }
    }

    /// @brief Apply gain to a range of samples on a specific channel
    void applyGain(int channel, int startSample, int numSamples, SampleType gain) noexcept {
        assert(channel >= 0 && channel < numChannels_);
        assert(startSample >= 0 && startSample + numSamples <= numSamples_);
        auto* data = channelPointers_[static_cast<size_t>(channel)] + startSample;
        for (int s = 0; s < numSamples; ++s) {
            data[s] *= gain;
        }
    }

    /// @brief Add samples from another buffer, optionally with gain
    /// @param source The source buffer to add from
    /// @param gain Gain applied to source samples before adding
    /// @note Real-time safe
    void addFrom(const AudioBuffer& source, SampleType gain = SampleType(1)) noexcept {
        int channels = std::min(numChannels_, source.numChannels_);
        int samples = std::min(numSamples_, source.numSamples_);
        for (int ch = 0; ch < channels; ++ch) {
            auto* dst = channelPointers_[static_cast<size_t>(ch)];
            const auto* src = source.channelPointers_[static_cast<size_t>(ch)];
            for (int s = 0; s < samples; ++s) {
                dst[s] += src[s] * gain;
            }
        }
    }

private:
    std::vector<std::vector<SampleType>> ownedData_;
    std::vector<SampleType*>             channelPointers_;
    int                                  numChannels_ = 0;
    int                                  numSamples_  = 0;
};

} // namespace body
