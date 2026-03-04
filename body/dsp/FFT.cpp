// MIT License
// Copyright (c) 2025 BODY Framework Contributors

#include "body/dsp/FFT.hpp"

#include <pffft.h>
#include <stdexcept>
#include <utility>

namespace body::dsp {

struct FFT::Impl {
    PFFFT_Setup* setup = nullptr;
    float* work = nullptr;

    Impl(int size)
    {
        setup = pffft_new_setup(size, PFFFT_REAL);
        if (!setup)
            throw std::invalid_argument("FFT: invalid size (must be power of 2, >= 32)");

        work = static_cast<float*>(pffft_aligned_malloc(static_cast<std::size_t>(size) * sizeof(float)));
    }

    ~Impl()
    {
        if (work)
            pffft_aligned_free(work);
        if (setup)
            pffft_destroy_setup(setup);
    }

    Impl(const Impl&) = delete;
    Impl& operator=(const Impl&) = delete;
};

FFT::FFT(int size)
    : impl_(std::make_unique<Impl>(size))
    , size_(size)
{
}

FFT::~FFT() = default;

FFT::FFT(FFT&& other) noexcept
    : impl_(std::move(other.impl_))
    , size_(other.size_)
{
    other.size_ = 0;
}

FFT& FFT::operator=(FFT&& other) noexcept
{
    if (this != &other) {
        impl_ = std::move(other.impl_);
        size_ = other.size_;
        other.size_ = 0;
    }
    return *this;
}

void FFT::forward(const float* input, float* output) const
{
    pffft_transform_ordered(impl_->setup, input, output, impl_->work, PFFFT_FORWARD);
}

void FFT::inverse(const float* input, float* output) const
{
    pffft_transform_ordered(impl_->setup, input, output, impl_->work, PFFFT_BACKWARD);
}

} // namespace body::dsp
