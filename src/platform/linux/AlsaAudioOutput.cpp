// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 hawktocomet

#include <retropixel/platform/linux/AlsaAudioOutput.hpp>

#include <alsa/asoundlib.h>

#include <cerrno>
#include <limits>
#include <new>
#include <system_error>

namespace RetroPixel {

struct AlsaAudioOutput::Impl {
    snd_pcm_t* handle = nullptr;
    std::size_t frameBytes = 0;
};

namespace {

std::error_code alsaError(int result)
{
    return {result < 0 ? -result : result, std::generic_category()};
}

bool sampleFormat(
    AudioSampleFormat format,
    snd_pcm_format_t& alsaFormat,
    std::size_t& sampleBytes)
{
    switch (format) {
    case AudioSampleFormat::Signed16LittleEndian:
        alsaFormat = SND_PCM_FORMAT_S16_LE;
        sampleBytes = 2;
        return true;

    case AudioSampleFormat::Signed24LittleEndian:
        alsaFormat = SND_PCM_FORMAT_S24_LE;
        sampleBytes = 4;
        return true;

    case AudioSampleFormat::Signed32LittleEndian:
        alsaFormat = SND_PCM_FORMAT_S32_LE;
        sampleBytes = 4;
        return true;
    }

    return false;
}

} // namespace

AlsaAudioOutput::AlsaAudioOutput()
    : impl_(new Impl)
{
}

AlsaAudioOutput::~AlsaAudioOutput()
{
    close();
}

std::error_code AlsaAudioOutput::open(
    const AudioConfiguration& configuration)
{
    close();

    if (configuration.device.empty() ||
        configuration.sampleRate == 0 ||
        configuration.channels == 0) {
        return std::make_error_code(std::errc::invalid_argument);
    }

    snd_pcm_format_t format;
    std::size_t sampleBytes = 0;

    if (!sampleFormat(configuration.format, format, sampleBytes))
        return std::make_error_code(std::errc::invalid_argument);

    if (sampleBytes >
        std::numeric_limits<std::size_t>::max() / configuration.channels) {
        return std::make_error_code(std::errc::value_too_large);
    }

    snd_pcm_t* handle = nullptr;

    const int openResult = snd_pcm_open(
        &handle,
        configuration.device.c_str(),
        SND_PCM_STREAM_PLAYBACK,
        0);

    if (openResult < 0)
        return alsaError(openResult);

    const int paramsResult = snd_pcm_set_params(
        handle,
        format,
        SND_PCM_ACCESS_RW_INTERLEAVED,
        configuration.channels,
        configuration.sampleRate,
        1,
        100000);

    if (paramsResult < 0) {
        snd_pcm_close(handle);
        return alsaError(paramsResult);
    }

    impl_->handle = handle;
    impl_->frameBytes = sampleBytes * configuration.channels;

    return {};
}

void AlsaAudioOutput::close() noexcept
{
    if (impl_ && impl_->handle) {
        snd_pcm_close(impl_->handle);
        impl_->handle = nullptr;
    }

    if (impl_)
        impl_->frameBytes = 0;
}

bool AlsaAudioOutput::isOpen() const noexcept
{
    return impl_ && impl_->handle;
}

AudioWriteResult AlsaAudioOutput::write(
    const void* frames,
    std::size_t frameCount)
{
    if (!isOpen())
        return {0, std::make_error_code(std::errc::bad_file_descriptor)};

    if (frameCount == 0)
        return {};

    if (!frames)
        return {0, std::make_error_code(std::errc::invalid_argument)};

    if (frameCount > std::numeric_limits<std::size_t>::max() / impl_->frameBytes ||
        frameCount > std::numeric_limits<snd_pcm_uframes_t>::max()) {
        return {0, std::make_error_code(std::errc::value_too_large)};
    }

    const auto* data = static_cast<const unsigned char*>(frames);
    std::size_t writtenFrames = 0;

    while (writtenFrames < frameCount) {
        const snd_pcm_uframes_t remaining =
            static_cast<snd_pcm_uframes_t>(frameCount - writtenFrames);

        const snd_pcm_sframes_t result = snd_pcm_writei(
            impl_->handle,
            data + writtenFrames * impl_->frameBytes,
            remaining);

        if (result > 0) {
            writtenFrames += static_cast<std::size_t>(result);
            continue;
        }

        if (result == 0)
            return {writtenFrames, std::make_error_code(std::errc::io_error)};

        const int recoveryResult = snd_pcm_recover(
            impl_->handle,
            static_cast<int>(result),
            1);

        if (recoveryResult < 0) {
            return {
                writtenFrames,
                alsaError(recoveryResult)
            };
        }
    }

    return {writtenFrames, {}};
}

} // namespace RetroPixel
