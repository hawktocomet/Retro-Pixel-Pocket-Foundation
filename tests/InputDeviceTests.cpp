// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 hawktocomet

#include <retropixel/platform/linux/EvdevInputDevice.hpp>

#include <linux/input.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

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
    const fs::path root = fs::temp_directory_path() /
        ("retropixel-input-test-" +
         std::to_string(
             std::chrono::steady_clock::now()
                 .time_since_epoch().count()));

    fs::create_directories(root);
    const fs::path eventFile = root / "event-test";

    input_event raw{};
    raw.time.tv_sec = 123;
    raw.time.tv_usec = 456789;
    raw.type = EV_KEY;
    raw.code = BTN_SOUTH;
    raw.value = 1;

    {
        std::ofstream file(eventFile, std::ios::binary);
        file.write(
            reinterpret_cast<const char*>(&raw),
            sizeof(raw));
    }

    bool passed = true;
    RetroPixel::EvdevInputDevice device;

    const auto missingError = device.open((root / "missing").string());
    passed &= check(
        static_cast<bool>(missingError) && !device.isOpen(),
        "reports failure when the device path does not exist");

    const auto openError = device.open(eventFile.string());
    passed &= check(
        !openError && device.isOpen(),
        "opens an input event source");

    const auto info = device.info();
    passed &= check(
        !info.nameValid && !info.hardwareIdValid && !info.eventTypesValid,
        "reports unavailable hardware metadata without failing to open");

    auto result = device.read();
    passed &= check(
        result.status == RetroPixel::InputReadStatus::EventAvailable &&
        result.event.type == EV_KEY &&
        result.event.code == BTN_SOUTH &&
        result.event.value == 1 &&
        result.event.timestampSeconds == 123 &&
        result.event.timestampMicroseconds == 456789,
        "translates a Linux input event");

    result = device.read();
    passed &= check(
        result.status == RetroPixel::InputReadStatus::Closed,
        "reports end of input source");

    device.close();
    passed &= check(
        !device.isOpen() &&
        device.read().status == RetroPixel::InputReadStatus::Closed,
        "closes cleanly and reports closed state");

    fs::remove_all(root);

    if (!passed) {
        std::cerr << "\nInput device tests failed.\n";
        return 1;
    }

    std::cout << "\nAll input device tests passed.\n";
    return 0;
}
