// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 hawktocomet

#pragma once

#include <retropixel/Audio.hpp>

#include <memory>

namespace RetroPixel {

class AlsaAudioOutput final : public AudioOutput {
public:
    AlsaAudioOutput();
    ~AlsaAudioOutput() override;

    AlsaAudioOutput(const AlsaAudioOutput&) = delete;
    AlsaAudioOutput& operator=(const AlsaAudioOutput&) = delete;

    std::error_code open(const AudioConfiguration& configuration) override;
    void close() noexcept override;
    bool isOpen() const noexcept override;

    AudioWriteResult write(
        const void* frames,
        std::size_t frameCount) override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace RetroPixel
