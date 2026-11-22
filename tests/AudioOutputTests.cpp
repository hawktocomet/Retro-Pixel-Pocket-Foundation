// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 hawktocomet

#include <retropixel/platform/linux/AlsaAudioOutput.hpp>

#include <array>
#include <cstdint>
#include <iostream>
#include <limits>
#include <vector>

static bool check(bool condition, const char* description)
{
    if (!condition)
        std::cerr << "FAIL: " << description << '\n';
    else
        std::cout << "PASS: " << description << '\n';

    return condition;
}

int main()
{
    bool passed = true;
    RetroPixel::AlsaAudioOutput output;

    passed &= check(
        !output.isOpen(),
        "starts closed");

    const auto closedWrite = output.write(nullptr, 1);
    passed &= check(
        static_cast<bool>(closedWrite.error),
        "rejects writes while closed");

    RetroPixel::AudioConfiguration invalid;
    invalid.device.clear();

    passed &= check(
        static_cast<bool>(output.open(invalid)) && !output.isOpen(),
        "rejects an empty device name");

    invalid.device = "null";
    invalid.sampleRate = 0;

    passed &= check(
        static_cast<bool>(output.open(invalid)) && !output.isOpen(),
        "rejects a zero sample rate");

    invalid.sampleRate = 44100;
    invalid.channels = 0;

    passed &= check(
        static_cast<bool>(output.open(invalid)) && !output.isOpen(),
        "rejects zero channels");

    invalid.channels = 2;
    invalid.format = static_cast<RetroPixel::AudioSampleFormat>(999);

    passed &= check(
        static_cast<bool>(output.open(invalid)) && !output.isOpen(),
        "rejects an unsupported sample format");

    const std::array<RetroPixel::AudioSampleFormat, 3> formats = {
        RetroPixel::AudioSampleFormat::Signed16LittleEndian,
        RetroPixel::AudioSampleFormat::Signed24LittleEndian,
        RetroPixel::AudioSampleFormat::Signed32LittleEndian
    };

    const std::array<std::size_t, 3> sampleBytes = {2, 4, 4};

    for (std::size_t i = 0; i < formats.size(); ++i) {
        RetroPixel::AudioConfiguration configuration;
        configuration.device = "null";
        configuration.format = formats[i];
        configuration.sampleRate = 44100;
        configuration.channels = 2;

        const auto openError = output.open(configuration);
        passed &= check(
            !openError && output.isOpen(),
            "opens ALSA null device with requested format");

        if (openError)
            continue;

        std::vector<std::uint8_t> frames(256 * sampleBytes[i] * 2, 0);

        const auto result = output.write(frames.data(), 256);
        passed &= check(
            !result.error && result.framesWritten == 256,
            "writes PCM frames successfully");

        const auto oversizedWrite = output.write(
            frames.data(), std::numeric_limits<std::size_t>::max());
        passed &= check(
            oversizedWrite.error == std::make_error_code(std::errc::value_too_large) &&
                oversizedWrite.framesWritten == 0,
            "rejects oversized frame counts");

        const auto emptyWrite = output.write(nullptr, 0);
        passed &= check(
            !emptyWrite.error && emptyWrite.framesWritten == 0,
            "accepts a zero-length write");

        const auto nullWrite = output.write(nullptr, 1);
        passed &= check(
            static_cast<bool>(nullWrite.error),
            "rejects a null buffer for nonempty writes");

        output.close();
        passed &= check(
            !output.isOpen(),
            "closes the ALSA device");
    }

    output.close();

    if (!passed) {
        std::cerr << "\nAudio output tests failed.\n";
        return 1;
    }

    std::cout << "\nAll audio output tests passed.\n";
    return 0;
}
