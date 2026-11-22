/* SPDX-License-Identifier: GPL-3.0-or-later */
/* Copyright (C) 2026 hawktocomet */

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <system_error>

namespace RetroPixel {

enum class AudioSampleFormat {
    Signed16LittleEndian,
    Signed24LittleEndian,
    Signed32LittleEndian
};

struct AudioConfiguration {
    std::string device = "default";
    AudioSampleFormat format = AudioSampleFormat::Signed16LittleEndian;
    std::uint32_t sampleRate = 44100;
    std::uint32_t channels = 2;
};

struct AudioWriteResult {
    std::size_t framesWritten = 0;
    std::error_code error;
};

class AudioOutput {
public:
    virtual ~AudioOutput() = default;

    virtual std::error_code open(const AudioConfiguration& configuration) = 0;
    virtual void close() noexcept = 0;
    virtual bool isOpen() const noexcept = 0;
    virtual AudioWriteResult write(
        const void* frames,
        std::size_t frameCount) = 0;
};

} // namespace RetroPixel
